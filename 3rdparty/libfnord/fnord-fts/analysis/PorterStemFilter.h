/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef PORTERSTEMFILTER_H
#define PORTERSTEMFILTER_H

#include "fnord-fts/analysis/TokenFilter.h"

namespace fnord {
namespace fts {

/// Transforms the token stream as per the Porter stemming algorithm.  Note: the input to the stemming filter must
/// already be in lower case, so you will need to use LowerCaseFilter or LowerCaseTokenizer further down the Tokenizer
/// chain in order for this to work properly.
///
/// To use this filter with other analyzers, you'll want to write an Analyzer class that sets up the TokenStream chain
/// as you want it.  To use this with LowerCaseTokenizer, for example, you'd write an analyzer like this:
///
/// <pre>
/// class MyAnalyzer : public Analyzer
/// {
/// public:
///     virtual TokenStreamPtr tokenStream(const String& fieldName, const ReaderPtr& reader)
///     {
///         return newLucene<PorterStemFilter>(newLucene<LowerCaseTokenizer>(reader));
///     }
/// };
/// </pre>
class PorterStemFilter : public TokenFilter {
public:
    PorterStemFilter(const TokenStreamPtr& input);
    virtual ~PorterStemFilter();

    LUCENE_CLASS(PorterStemFilter);

protected:
    PorterStemmerPtr stemmer;
    TermAttributePtr termAtt;

public:
    virtual bool incrementToken();
};

}

}
#endif
