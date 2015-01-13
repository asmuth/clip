/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "NearSpansOrdered.h"
#include "SpanNearQuery.h"

namespace Lucene {

NearSpansOrdered::NearSpansOrdered(const SpanNearQueryPtr& spanNearQuery, const IndexReaderPtr& reader, bool collectPayloads) {
    if (spanNearQuery->getClauses().size() < 2) {
        boost::throw_exception(IllegalArgumentException(L"Less than 2 clauses: " + spanNearQuery->toString()));
    }
    this->firstTime = true;
    this->more = false;
    this->inSameDoc = false;
    this->matchDoc = -1;
    this->matchStart = -1;
    this->matchEnd = -1;
    this->collectPayloads = collectPayloads;
    this->allowedSlop = spanNearQuery->getSlop();
    Collection<SpanQueryPtr> clauses(spanNearQuery->getClauses());
    this->subSpans = Collection<SpansPtr>::newInstance(clauses.size());
    this->matchPayload = Collection<ByteArray>::newInstance();
    this->subSpansByDoc = Collection<SpansPtr>::newInstance(clauses.size());
    for (int32_t i = 0; i < clauses.size(); ++i) {
        subSpans[i] = clauses[i]->getSpans(reader);
        subSpansByDoc[i] = subSpans[i]; // used in toSameDoc()
    }
    this->query = spanNearQuery; // kept for toString() only.
}

NearSpansOrdered::~NearSpansOrdered() {
}

int32_t NearSpansOrdered::doc() {
    return matchDoc;
}

int32_t NearSpansOrdered::start() {
    return matchStart;
}

int32_t NearSpansOrdered::end() {
    return matchEnd;
}

Collection<SpansPtr> NearSpansOrdered::getSubSpans() {
    return subSpans;
}

Collection<ByteArray> NearSpansOrdered::getPayload() {
    return matchPayload;
}

bool NearSpansOrdered::isPayloadAvailable() {
    return !matchPayload.empty();
}

bool NearSpansOrdered::next() {
    if (firstTime) {
        firstTime = false;
        for (int32_t i = 0; i < subSpans.size(); ++i) {
            if (!subSpans[i]->next()) {
                more = false;
                return false;
            }
        }
        more = true;
    }
    if (collectPayloads) {
        matchPayload.clear();
    }
    return advanceAfterOrdered();
}

bool NearSpansOrdered::skipTo(int32_t target) {
    if (firstTime) {
        firstTime = false;
        for (int32_t i = 0; i < subSpans.size(); ++i) {
            if (!subSpans[i]->skipTo(target)) {
                more = false;
                return false;
            }
        }
        more = true;
    } else if (more && (subSpans[0]->doc() < target)) {
        if (subSpans[0]->skipTo(target)) {
            inSameDoc = false;
        } else {
            more = false;
            return false;
        }
    }
    if (collectPayloads) {
        matchPayload.clear();
    }
    return advanceAfterOrdered();
}

bool NearSpansOrdered::advanceAfterOrdered() {
    while (more && (inSameDoc || toSameDoc())) {
        if (stretchToOrder() && shrinkToAfterShortestMatch()) {
            return true;
        }
    }
    return false; // no more matches
}

struct lessSpanDoc {
    inline bool operator()(const SpansPtr& first, const SpansPtr& second) const {
        return ((first->doc() - second->doc()) < 0);
    }
};

bool NearSpansOrdered::toSameDoc() {
    std::sort(subSpansByDoc.begin(), subSpansByDoc.end(), lessSpanDoc());
    int32_t firstIndex = 0;
    int32_t maxDoc = subSpansByDoc[subSpansByDoc.size() - 1]->doc();
    while (subSpansByDoc[firstIndex]->doc() != maxDoc) {
        if (!subSpansByDoc[firstIndex]->skipTo(maxDoc)) {
            more = false;
            inSameDoc = false;
            return false;
        }
        maxDoc = subSpansByDoc[firstIndex]->doc();
        if (++firstIndex == subSpansByDoc.size()) {
            firstIndex = 0;
        }
    }
    for (int32_t i = 0; i < subSpansByDoc.size(); ++i) {
        BOOST_ASSERT(subSpansByDoc[i]->doc() == maxDoc);
    }
    inSameDoc = true;
    return true;
}

bool NearSpansOrdered::docSpansOrdered(const SpansPtr& spans1, const SpansPtr& spans2) {
    BOOST_ASSERT(spans1->doc() == spans2->doc());
    int32_t start1 = spans1->start();
    int32_t start2 = spans2->start();
    // Do not call docSpansOrdered(int,int,int,int) to avoid invoking .end()
    return start1 == start2 ? (spans1->end() < spans2->end()) : (start1 < start2);
}

bool NearSpansOrdered::docSpansOrdered(int32_t start1, int32_t end1, int32_t start2, int32_t end2) {
    return start1 == start2 ? (end1 < end2) : (start1 < start2);
}

bool NearSpansOrdered::stretchToOrder() {
    matchDoc = subSpans[0]->doc();
    for (int32_t i = 1; inSameDoc && (i < subSpans.size()); ++i) {
        while (!docSpansOrdered(subSpans[i - 1], subSpans[i])) {
            if (!subSpans[i]->next()) {
                inSameDoc = false;
                more = false;
                break;
            } else if (matchDoc != subSpans[i]->doc()) {
                inSameDoc = false;
                break;
            }
        }
    }
    return inSameDoc;
}

bool NearSpansOrdered::shrinkToAfterShortestMatch() {
    SpansPtr subSpan(subSpans[subSpans.size() - 1]);
    matchStart = subSpan->start();
    matchEnd = subSpan->end();
    SetByteArray possibleMatchPayloads(SetByteArray::newInstance());
    if (subSpan->isPayloadAvailable()) {
        Collection<ByteArray> payload(subSpan->getPayload());
        possibleMatchPayloads.addAll(payload.begin(), payload.end());
    }

    Collection<ByteArray> possiblePayload;

    int32_t matchSlop = 0;
    int32_t lastStart = matchStart;
    int32_t lastEnd = matchEnd;
    for (int32_t i = subSpans.size() - 2; i >= 0; --i) {
        SpansPtr prevSpans(subSpans[i]);
        if (collectPayloads && prevSpans->isPayloadAvailable()) {
            Collection<ByteArray> payload(prevSpans->getPayload());
            possiblePayload = Collection<ByteArray>::newInstance(payload.begin(), payload.end());
        }

        int32_t prevStart = prevSpans->start();
        int32_t prevEnd = prevSpans->end();
        while (true) { // Advance prevSpans until after (lastStart, lastEnd)
            if (!prevSpans->next()) {
                inSameDoc = false;
                more = false;
                break; // Check remaining subSpans for final match.
            } else if (matchDoc != prevSpans->doc()) {
                inSameDoc = false; // The last subSpans is not advanced here.
                break; // Check remaining subSpans for last match in this document.
            } else {
                int32_t ppStart = prevSpans->start();
                int32_t ppEnd = prevSpans->end(); // Cannot avoid invoking .end()
                if (!docSpansOrdered(ppStart, ppEnd, lastStart, lastEnd)) {
                    break;    // Check remaining subSpans.
                } else {
                    prevStart = ppStart;
                    prevEnd = ppEnd;
                    if (collectPayloads && prevSpans->isPayloadAvailable()) {
                        Collection<ByteArray> payload(prevSpans->getPayload());
                        possiblePayload = Collection<ByteArray>::newInstance(payload.begin(), payload.end());
                    }
                }
            }
        }

        if (collectPayloads && possiblePayload) {
            possibleMatchPayloads.addAll(possiblePayload.begin(), possiblePayload.end());
        }

        BOOST_ASSERT(prevStart <= matchStart);
        if (matchStart > prevEnd) { // Only non overlapping spans add to slop.
            matchSlop += (matchStart - prevEnd);
        }

        // Do not break on (matchSlop > allowedSlop) here to make sure that subSpans[0] is
        // advanced after the match, if any.
        matchStart = prevStart;
        lastStart = prevStart;
        lastEnd = prevEnd;
    }

    bool match = (matchSlop <= allowedSlop);

    if (collectPayloads && match && !possibleMatchPayloads.empty()) {
        matchPayload.addAll(possibleMatchPayloads.begin(), possibleMatchPayloads.end());
    }

    return match; // ordered and allowed slop
}

String NearSpansOrdered::toString() {
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

}
