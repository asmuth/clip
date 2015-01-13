/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "TermSpans.h"
#include "TermPositions.h"
#include "Term.h"

namespace Lucene {

TermSpans::TermSpans(const TermPositionsPtr& positions, const TermPtr& term) {
    this->positions = positions;
    this->term = term;
    this->_doc = -1;
    this->freq = 0;
    this->count = 0;
    this->position = 0;
}

TermSpans::~TermSpans() {
}

bool TermSpans::next() {
    if (count == freq) {
        if (!positions->next()) {
            _doc = INT_MAX;
            return false;
        }
        _doc = positions->doc();
        freq = positions->freq();
        count = 0;
    }
    position = positions->nextPosition();
    ++count;
    return true;
}

bool TermSpans::skipTo(int32_t target) {
    if (!positions->skipTo(target)) {
        _doc = INT_MAX;
        return false;
    }

    _doc = positions->doc();
    freq = positions->freq();
    count = 0;

    position = positions->nextPosition();
    ++count;

    return true;
}

int32_t TermSpans::doc() {
    return _doc;
}

int32_t TermSpans::start() {
    return position;
}

int32_t TermSpans::end() {
    return position + 1;
}

Collection<ByteArray> TermSpans::getPayload() {
    Collection<ByteArray> payload(newCollection<ByteArray>(ByteArray::newInstance(positions->getPayloadLength())));
    payload[0] = positions->getPayload(payload[0], 0);
    return payload;
}

bool TermSpans::isPayloadAvailable() {
    return positions->isPayloadAvailable();
}

String TermSpans::toString() {
    StringStream buffer;
    buffer << L"spans(" << term->toString() << L")@";
    if (_doc == -1) {
        buffer << L"START";
    } else if (_doc == INT_MAX) {
        buffer << L"END";
    } else {
        buffer << _doc << L"-" << position;
    }
    return buffer.str();
}

TermPositionsPtr TermSpans::getPositions() {
    return positions;
}

}
