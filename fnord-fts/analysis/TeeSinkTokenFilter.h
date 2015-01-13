/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TEESINKTOKENFILTER_H
#define TEESINKTOKENFILTER_H

#include "fnord-fts/analysis/TokenFilter.h"
#include "fnord-fts/analysis/TokenStream.h"

namespace Lucene {

/// This TokenFilter provides the ability to set aside attribute states that have already been analyzed.  This is
/// useful in situations where multiple fields share many common analysis steps and then go their separate ways.
///
/// It is also useful for doing things like entity extraction or proper noun analysis as part of the analysis workflow
/// and saving off those tokens for use in another field.
///
/// <pre>
/// TeeSinkTokenFilterPtr source1 = newLucene<TeeSinkTokenFilter>(newLucene<WhitespaceTokenizer>(reader1));
/// SinkTokenStreamPtr sink1 = source1->newSinkTokenStream();
/// SinkTokenStreamPtr sink2 = source1->newSinkTokenStream();
///
/// TeeSinkTokenFilterPtr source2 = newLucene<TeeSinkTokenFilter>(newLucene<WhitespaceTokenizer>(reader2));
/// source2->addSinkTokenStream(sink1);
/// source2->addSinkTokenStream(sink2);
///
/// TokenStreamPtr final1 = newLucene<LowerCaseFilter>(source1);
/// TokenStreamPtr final2 = source2;
/// TokenStreamPtr final3 = newLucene<EntityDetect>(sink1);
/// TokenStreamPtr final4 = newLucene<URLDetect>(sink2);
///
/// d->add(newLucene<Field>(L"f1", final1));
/// d->add(newLucene<Field>(L"f2", final2));
/// d->add(newLucene<Field>(L"f3", final3));
/// d->add(newLucene<Field>(L"f4", final4));
/// </pre>
///
/// In this example, sink1 and sink2 will both get tokens from both reader1 and reader2 after whitespace tokenizer
/// and now we can further wrap any of these in extra analysis, and more "sources" can be inserted if desired.
/// It is important, that tees are consumed before sinks (in the above example, the field names must be less the
/// sink's field names). If you are not sure, which stream is consumed first, you can simply add another sink and
/// then pass all tokens to the sinks at once using {@link #consumeAllTokens}.
///
/// This TokenFilter is exhausted after this. In the above example, change the example above to:
///
/// <pre>
/// ...
/// TokenStreamPtr final1 = newLucene<LowerCaseFilter>(source1->newSinkTokenStream());
/// TokenStreamPtr final2 = source2->newSinkTokenStream();
/// sink1->consumeAllTokens();
/// sink2->consumeAllTokens();
/// ...
/// </pre>
///
/// In this case, the fields can be added in any order, because the sources are not used anymore and all sinks are
/// ready.
///
/// Note, the EntityDetect and URLDetect TokenStreams are for the example and do not currently exist in Lucene.
class TeeSinkTokenFilter : public TokenFilter {
public:
    /// Instantiates a new TeeSinkTokenFilter.
    TeeSinkTokenFilter(const TokenStreamPtr& input);
    virtual ~TeeSinkTokenFilter();

    LUCENE_CLASS(TeeSinkTokenFilter);

protected:
    Collection<SinkTokenStreamPtr> sinks;

public:
    /// Returns a new {@link SinkTokenStream} that receives all tokens consumed by this stream.
    SinkTokenStreamPtr newSinkTokenStream();

    /// Returns a new {@link SinkTokenStream} that receives all tokens consumed by this stream that pass
    /// the supplied filter.
    /// @see SinkFilter
    SinkTokenStreamPtr newSinkTokenStream(const SinkFilterPtr& filter);

    /// Adds a {@link SinkTokenStream} created by another TeeSinkTokenFilter to this one. The supplied stream will
    /// also receive all consumed tokens.  This method can be used to pass tokens from two different tees to one sink.
    void addSinkTokenStream(const SinkTokenStreamPtr& sink);

    /// TeeSinkTokenFilter passes all tokens to the added sinks when itself is consumed. To be sure, that all tokens
    /// from the input stream are passed to the sinks, you can call this methods.  This instance is exhausted after this,
    /// but all sinks are instant available.
    void consumeAllTokens();

    virtual bool incrementToken();
    virtual void end();
};

class SinkFilter : public LuceneObject {
public:
    virtual ~SinkFilter();

    LUCENE_CLASS(SinkFilter);

public:
    /// Returns true, if the current state of the passed-in {@link AttributeSource} shall be stored in the sink.
    virtual bool accept(const AttributeSourcePtr& source) = 0;

    /// Called by {@link SinkTokenStream#reset()}. This method does nothing by default and can optionally be overridden.
    virtual void reset();
};

class AcceptAllSinkFilter : public SinkFilter {
public:
    virtual ~AcceptAllSinkFilter();

    LUCENE_CLASS(AcceptAllSinkFilter);

public:
    virtual bool accept(const AttributeSourcePtr& source);
};

/// A filter that decides which {@link AttributeSource} states to store in the sink.
class SinkTokenStream : public TokenStream {
public:
    SinkTokenStream(const AttributeSourcePtr& source, const SinkFilterPtr& filter);
    virtual ~SinkTokenStream();

    LUCENE_CLASS(SinkTokenStream);

protected:
    Collection<AttributeSourceStatePtr> cachedStates;
    AttributeSourceStatePtr finalState;
    bool initIterator;
    Collection<AttributeSourceStatePtr>::iterator it;
    SinkFilterPtr filter;

protected:
    bool accept(const AttributeSourcePtr& source);
    void addState(const AttributeSourceStatePtr& state);
    void setFinalState(const AttributeSourceStatePtr& finalState);

public:
    virtual bool incrementToken();
    virtual void end();
    virtual void reset();

    friend class TeeSinkTokenFilter;
};

}

#endif
