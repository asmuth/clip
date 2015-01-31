/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef ANALYZER_H
#define ANALYZER_H

#include "fnord-fts/util/CloseableThreadLocal.h"

namespace fnord {
namespace fts {

/// An Analyzer builds TokenStreams, which analyze text.  It thus represents a policy for extracting index terms
/// from text.
///
/// Typical implementations first build a Tokenizer, which breaks the stream of characters from the Reader into
/// raw Tokens.  One or more TokenFilters may then be applied to the output of the Tokenizer.
class Analyzer : public LuceneObject {
public:
    virtual ~Analyzer();
    LUCENE_CLASS(Analyzer);

protected:
    CloseableThreadLocal<LuceneObject> tokenStreams;

public:
    /// Creates a TokenStream which tokenizes all the text in the provided Reader.  Must be able to handle null
    /// field name for backward compatibility.
    virtual TokenStreamPtr tokenStream(const String& fieldName, const ReaderPtr& reader) = 0;

    /// Creates a TokenStream that is allowed to be re-used from the previous time that the same thread called
    /// this method.  Callers that do not need to use more than one TokenStream at the same time from this analyzer
    /// should use this method for better performance.
    virtual TokenStreamPtr reusableTokenStream(const String& fieldName, const ReaderPtr& reader);

    /// Invoked before indexing a Fieldable instance if terms have already been added to that field.  This allows
    /// custom analyzers to place an automatic position increment gap between Fieldable instances using the same
    /// field name.  The default value position increment gap is 0.  With a 0 position increment gap and the typical
    /// default token position increment of 1, all terms in a field, including across Fieldable instances, are in
    /// successive positions, allowing exact PhraseQuery matches, for instance, across Fieldable instance boundaries.
    ///
    /// @param fieldName Fieldable name being indexed.
    /// @return position increment gap, added to the next token emitted from {@link #tokenStream(String,Reader)}
    virtual int32_t getPositionIncrementGap(const String& fieldName);

    /// Just like {@link #getPositionIncrementGap}, except for Token offsets instead.  By default this returns 1 for
    /// tokenized fields and, as if the fields were joined with an extra space character, and 0 for un-tokenized
    /// fields.  This method is only called if the field produced at least one token for indexing.
    ///
    /// @param field the field just indexed
    /// @return offset gap, added to the next token emitted from {@link #tokenStream(String,Reader)}
    virtual int32_t getOffsetGap(const FieldablePtr& field);

    /// Frees persistent resources used by this Analyzer
    virtual void close();

protected:
    /// Used by Analyzers that implement reusableTokenStream to retrieve previously saved TokenStreams for re-use
    /// by the same thread.
    virtual LuceneObjectPtr getPreviousTokenStream();

    /// Used by Analyzers that implement reusableTokenStream to save a TokenStream for later re-use by the
    /// same thread.
    virtual void setPreviousTokenStream(const LuceneObjectPtr& stream);
};

}

}
#endif
