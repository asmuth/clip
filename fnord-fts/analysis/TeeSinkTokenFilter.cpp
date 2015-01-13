/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "TeeSinkTokenFilter.h"
#include "Attribute.h"

namespace Lucene {

TeeSinkTokenFilter::TeeSinkTokenFilter(const TokenStreamPtr& input) : TokenFilter(input) {
    this->sinks = Collection<SinkTokenStreamPtr>::newInstance();
}

TeeSinkTokenFilter::~TeeSinkTokenFilter() {
}

SinkTokenStreamPtr TeeSinkTokenFilter::newSinkTokenStream() {
    static SinkFilterPtr ACCEPT_ALL_FILTER;
    if (!ACCEPT_ALL_FILTER) {
        ACCEPT_ALL_FILTER = newLucene<AcceptAllSinkFilter>();
        CycleCheck::addStatic(ACCEPT_ALL_FILTER);
    }
    return newSinkTokenStream(ACCEPT_ALL_FILTER);
}

SinkTokenStreamPtr TeeSinkTokenFilter::newSinkTokenStream(const SinkFilterPtr& filter) {
    SinkTokenStreamPtr sink(newLucene<SinkTokenStream>(this->cloneAttributes(), filter));
    this->sinks.add(sink);
    return sink;
}

void TeeSinkTokenFilter::addSinkTokenStream(const SinkTokenStreamPtr& sink) {
    // check that sink has correct factory
    if (this->getAttributeFactory() != sink->getAttributeFactory()) {
        boost::throw_exception(IllegalArgumentException(L"The supplied sink is not compatible to this tee."));
    }
    // add eventually missing attribute impls to the existing sink
    Collection<AttributePtr> attrImpls(this->cloneAttributes()->getAttributes());
    for (Collection<AttributePtr>::iterator it = attrImpls.begin(); it != attrImpls.end(); ++it) {
        sink->addAttribute((*it)->getClassName(), *it);
    }
    this->sinks.add(sink);
}

void TeeSinkTokenFilter::consumeAllTokens() {
    while (incrementToken()) {
    }
}

bool TeeSinkTokenFilter::incrementToken() {
    if (input->incrementToken()) {
        // capture state lazily - maybe no SinkFilter accepts this state
        AttributeSourceStatePtr state;
        for (Collection<SinkTokenStreamPtr>::iterator ref = sinks.begin(); ref != sinks.end(); ++ref) {
            if (*ref) {
                if ((*ref)->accept(shared_from_this())) {
                    if (!state) {
                        state = this->captureState();
                    }
                    (*ref)->addState(state);
                }
            }
        }
        return true;
    }

    return false;
}

void TeeSinkTokenFilter::end() {
    TokenFilter::end();
    AttributeSourceStatePtr finalState(captureState());
    for (Collection<SinkTokenStreamPtr>::iterator ref = sinks.begin(); ref != sinks.end(); ++ref) {
        if (*ref) {
            (*ref)->setFinalState(finalState);
        }
    }
}

SinkFilter::~SinkFilter() {
}

void SinkFilter::reset() {
    // nothing to do; can be overridden
}

AcceptAllSinkFilter::~AcceptAllSinkFilter() {
}

bool AcceptAllSinkFilter::accept(const AttributeSourcePtr& source) {
    return true;
}

SinkTokenStream::SinkTokenStream(const AttributeSourcePtr& source, const SinkFilterPtr& filter) : TokenStream(source) {
    this->filter = filter;
    this->cachedStates = Collection<AttributeSourceStatePtr>::newInstance();
    this->it = cachedStates.begin();
    this->initIterator = false;
}

SinkTokenStream::~SinkTokenStream() {
}

bool SinkTokenStream::accept(const AttributeSourcePtr& source) {
    return filter->accept(source);
}

void SinkTokenStream::addState(const AttributeSourceStatePtr& state) {
    if (initIterator) {
        boost::throw_exception(IllegalStateException(L"The tee must be consumed before sinks are consumed."));
    }
    cachedStates.add(state);
}

void SinkTokenStream::setFinalState(const AttributeSourceStatePtr& finalState) {
    this->finalState = finalState;
}

bool SinkTokenStream::incrementToken() {
    // lazy init the iterator
    if (!initIterator) {
        it = cachedStates.begin();
        initIterator = true;
    }

    if (it == cachedStates.end()) {
        return false;
    }

    AttributeSourceStatePtr state = *it++;
    restoreState(state);
    return true;
}

void SinkTokenStream::end() {
    if (finalState) {
        restoreState(finalState);
    }
}

void SinkTokenStream::reset() {
    it = cachedStates.begin();
    initIterator = false;
}

}
