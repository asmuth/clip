/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "NearSpansUnordered.h"
#include "_NearSpansUnordered.h"
#include "NearSpansOrdered.h"
#include "SpanNearQuery.h"
#include "StringUtils.h"

namespace Lucene {

NearSpansUnordered::NearSpansUnordered(const SpanNearQueryPtr& query, const IndexReaderPtr& reader) {
    this->query = query;
    this->reader = reader;
}

NearSpansUnordered::~NearSpansUnordered() {
}

void NearSpansUnordered::initialize() {
    this->slop = query->getSlop();
    this->totalLength = 0;
    this->more = true;
    this->firstTime = true;

    Collection<SpanQueryPtr> clauses(query->getClauses());
    queue = newLucene<CellQueue>(clauses.size());
    subSpans = Collection<SpansPtr>::newInstance(clauses.size());
    ordered = Collection<SpansCellPtr>::newInstance();

    for (int32_t i = 0; i < clauses.size(); ++i) {
        SpansCellPtr cell(newLucene<SpansCell>(shared_from_this(), clauses[i]->getSpans(reader), i));
        ordered.add(cell);
        subSpans[i] = cell->spans;
    }
}

Collection<SpansPtr> NearSpansUnordered::getSubSpans() {
    return subSpans;
}

bool NearSpansUnordered::next() {
    if (firstTime) {
        initList(true);
        listToQueue(); // initialize queue
        firstTime = false;
    } else if (more) {
        if (min()->next()) { // trigger further scanning
            queue->updateTop();    // maintain queue
        } else {
            more = false;
        }
    }

    while (more) {
        bool queueStale = false;

        if (min()->doc() != max->doc()) { // maintain list
            queueToList();
            queueStale = true;
        }

        // skip to doc with all clauses

        while (more && first->doc() < last->doc()) {
            more = first->skipTo(last->doc()); // skip first upto last
            firstToLast(); // and move it to the end
            queueStale = true;
        }

        if (!more) {
            return false;
        }

        // found doc with all clauses

        if (queueStale) { // maintain the queue
            listToQueue();
            queueStale = false;
        }

        if (atMatch()) {
            return true;
        }

        more = min()->next();
        if (more) {
            queue->updateTop();    // maintain queue
        }
    }
    return false; // no more matches
}

bool NearSpansUnordered::skipTo(int32_t target) {
    if (firstTime) { // initialize
        initList(false);
        for (SpansCellPtr cell(first); more && cell; cell = cell->_next) {
            more = cell->skipTo(target);    // skip all
        }
        if (more) {
            listToQueue();
        }
        firstTime = false;
    } else { // normal case
        while (more && min()->doc() < target) { // skip as needed
            if (min()->skipTo(target)) {
                queue->updateTop();
            } else {
                more = false;
            }
        }
    }
    return (more && (atMatch() || next()));
}

SpansCellPtr NearSpansUnordered::min() {
    return queue->top();
}

int32_t NearSpansUnordered::doc() {
    return min()->doc();
}

int32_t NearSpansUnordered::start() {
    return min()->start();
}

int32_t NearSpansUnordered::end() {
    return max->end();
}

Collection<ByteArray> NearSpansUnordered::getPayload() {
    SetByteArray matchPayload(SetByteArray::newInstance());
    for (SpansCellPtr cell(first); cell; cell = cell->_next) {
        if (cell->isPayloadAvailable()) {
            Collection<ByteArray> payload(cell->getPayload());
            matchPayload.addAll(payload.begin(), payload.end());
        }
    }
    return Collection<ByteArray>::newInstance(matchPayload.begin(), matchPayload.end());
}

bool NearSpansUnordered::isPayloadAvailable() {
    SpansCellPtr pointer(min());
    while (pointer) {
        if (pointer->isPayloadAvailable()) {
            return true;
        }
        pointer = pointer->_next;
    }
    return false;
}

String NearSpansUnordered::toString() {
    StringStream buffer;
    buffer << getClassName() << L"(" << query->toString() << L")@";
    if (firstTime) {
        buffer << L"START";
    } else {
        if (more) {
            buffer << doc() << L":" << start() << L"-" << end();
        } else {
            buffer << L"END";
        }
    }
    return buffer.str();
}

void NearSpansUnordered::initList(bool next) {
    for (Collection<SpansCellPtr>::iterator cell = ordered.begin(); more && cell != ordered.end(); ++cell) {
        if (next) {
            more = (*cell)->next();    // move to first entry
        }
        if (more) {
            addToList(*cell);    // add to list
        }
    }
}

void NearSpansUnordered::addToList(const SpansCellPtr& cell) {
    if (last) { // add next to end of list
        last->_next = cell;
    } else {
        first = cell;
    }
    last = cell;
    cell->_next.reset();
}

void NearSpansUnordered::firstToLast() {
    last->_next = first; // move first to end of list
    last = first;
    first = first->_next;
    last->_next.reset();
}

void NearSpansUnordered::queueToList() {
    first.reset();
    last.reset();
    while (queue->top()) {
        addToList(queue->pop());
    }
}

void NearSpansUnordered::listToQueue() {
    queue->clear(); // rebuild queue
    for (SpansCellPtr cell(first); cell; cell = cell->_next) {
        queue->add(cell);    // add to queue from list
    }
}

bool NearSpansUnordered::atMatch() {
    return ((min()->doc() == max->doc()) && ((max->end() - min()->start() - totalLength) <= slop));
}

SpansCell::SpansCell(const NearSpansUnorderedPtr& unordered, const SpansPtr& spans, int32_t index) {
    this->_unordered = unordered;
    this->spans = spans;
    this->index = index;
    this->length = -1;
}

SpansCell::~SpansCell() {
}

bool SpansCell::next() {
    return adjust(spans->next());
}

bool SpansCell::skipTo(int32_t target) {
    return adjust(spans->skipTo(target));
}

bool SpansCell::adjust(bool condition) {
    NearSpansUnorderedPtr unordered(_unordered);
    if (length != -1) {
        unordered->totalLength -= length;    // subtract old length
    }
    if (condition) {
        length = end() - start();
        unordered->totalLength += length; // add new length

        if (!unordered->max || doc() > unordered->max->doc() || ((doc() == unordered->max->doc()) && (end() > unordered->max->end()))) {
            unordered->max = shared_from_this();
        }
    }
    unordered->more = condition;
    return condition;
}

int32_t SpansCell::doc() {
    return spans->doc();
}

int32_t SpansCell::start() {
    return spans->start();
}

int32_t SpansCell::end() {
    return spans->end();
}

Collection<ByteArray> SpansCell::getPayload() {
    Collection<ByteArray> payload(spans->getPayload());
    return Collection<ByteArray>::newInstance(payload.begin(), payload.end());
}

bool SpansCell::isPayloadAvailable() {
    return spans->isPayloadAvailable();
}

String SpansCell::toString() {
    return spans->toString() + L"#" + StringUtils::toString(index);
}

CellQueue::CellQueue(int32_t size) : PriorityQueue<SpansCellPtr>(size) {
}

CellQueue::~CellQueue() {
}

bool CellQueue::lessThan(const SpansCellPtr& first, const SpansCellPtr& second) {
    if (first->doc() == second->doc()) {
        return NearSpansOrdered::docSpansOrdered(first, second);
    } else {
        return (first->doc() < second->doc());
    }
}

}
