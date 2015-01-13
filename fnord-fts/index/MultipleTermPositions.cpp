/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "MultipleTermPositions.h"
#include "_MultipleTermPositions.h"
#include "IndexReader.h"
#include "Term.h"

namespace Lucene {

MultipleTermPositions::MultipleTermPositions(const IndexReaderPtr& indexReader, Collection<TermPtr> terms) {
    Collection<TermPositionsPtr> termPositions(Collection<TermPositionsPtr>::newInstance());

    for (Collection<TermPtr>::iterator term = terms.begin(); term != terms.end(); ++term) {
        termPositions.add(indexReader->termPositions(*term));
    }

    termPositionsQueue = newLucene<TermPositionsQueue>(termPositions);
    posList = newLucene<IntQueue>();
    _doc = 0;
    _freq = 0;
}

MultipleTermPositions::~MultipleTermPositions() {
}

bool MultipleTermPositions::next() {
    if (termPositionsQueue->empty()) {
        return false;
    }

    posList->clear();
    _doc = termPositionsQueue->top()->doc();

    TermPositionsPtr tp;
    do {
        tp = termPositionsQueue->top();

        for (int32_t i = 0; i < tp->freq(); ++i) {
            posList->add(tp->nextPosition());
        }

        if (tp->next()) {
            termPositionsQueue->updateTop();
        } else {
            termPositionsQueue->pop();
            tp->close();
        }
    } while (!termPositionsQueue->empty() && termPositionsQueue->top()->doc() == _doc);

    posList->sort();
    _freq = posList->size();

    return true;
}

int32_t MultipleTermPositions::nextPosition() {
    return posList->next();
}

bool MultipleTermPositions::skipTo(int32_t target) {
    while (termPositionsQueue->top() && target > termPositionsQueue->top()->doc()) {
        TermPositionsPtr tp(termPositionsQueue->top());
        termPositionsQueue->pop();

        if (tp->skipTo(target)) {
            termPositionsQueue->add(tp);
        } else {
            tp->close();
        }
    }
    return next();
}

int32_t MultipleTermPositions::doc() {
    return _doc;
}

int32_t MultipleTermPositions::freq() {
    return _freq;
}

void MultipleTermPositions::close() {
    while (!termPositionsQueue->empty()) {
        termPositionsQueue->pop()->close();
    }
}

void MultipleTermPositions::seek(const TermPtr& term) {
    boost::throw_exception(UnsupportedOperationException());
}

void MultipleTermPositions::seek(const TermEnumPtr& termEnum) {
    boost::throw_exception(UnsupportedOperationException());
}

int32_t MultipleTermPositions::read(Collection<int32_t> docs, Collection<int32_t> freqs) {
    boost::throw_exception(UnsupportedOperationException());
    return 0;
}

ByteArray MultipleTermPositions::getPayload(ByteArray data, int32_t offset) {
    boost::throw_exception(UnsupportedOperationException());
    return ByteArray();
}

bool MultipleTermPositions::isPayloadAvailable() {
    return false;
}

TermPositionsQueue::TermPositionsQueue(Collection<TermPositionsPtr> termPositions) : PriorityQueue<TermPositionsPtr>(termPositions.size()) {
    this->termPositions = termPositions;
}

TermPositionsQueue::~TermPositionsQueue() {
}

void TermPositionsQueue::initialize() {
    PriorityQueue<TermPositionsPtr>::initialize();
    for (Collection<TermPositionsPtr>::iterator tp = termPositions.begin(); tp != termPositions.end(); ++tp) {
        if ((*tp)->next()) {
            add(*tp);
        }
    }
}

bool TermPositionsQueue::lessThan(const TermPositionsPtr& first, const TermPositionsPtr& second) {
    return (first->doc() < second->doc());
}

IntQueue::IntQueue() {
    arraySize = 16;
    index = 0;
    lastIndex = 0;
    array = Collection<int32_t>::newInstance(arraySize);
}

IntQueue::~IntQueue() {
}

void IntQueue::add(int32_t i) {
    if (lastIndex == arraySize) {
        growArray();
    }
    array[lastIndex++] = i;
}

int32_t IntQueue::next() {
    return array[index++];
}

void IntQueue::sort() {
    std::sort(array.begin() + index, array.begin() + lastIndex);
}

void IntQueue::clear() {
    index = 0;
    lastIndex = 0;
}

int32_t IntQueue::size() {
    return (lastIndex - index);
}

void IntQueue::growArray() {
    array.resize(arraySize * 2);
    arraySize *= 2;
}

}
