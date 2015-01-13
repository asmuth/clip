/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef WORDLISTLOADER_H
#define WORDLISTLOADER_H

#include "fnord-fts/util/LuceneObject.h"

namespace Lucene {

/// Loader for text files that represent a list of stopwords.
class WordlistLoader : public LuceneObject {
public:
    virtual ~WordlistLoader();

    LUCENE_CLASS(WordlistLoader);

public:
    /// Loads a text file and adds every line as an entry to a HashSet (omitting leading and trailing whitespace).
    /// Every line of the file should contain only one word. The words need to be in lowercase if you make use of an
    /// Analyzer which uses LowerCaseFilter (like StandardAnalyzer).
    ///
    /// @param wordfile File name containing the wordlist
    /// @param comment The comment string to ignore
    /// @return A set with the file's words
    static HashSet<String> getWordSet(const String& wordfile, const String& comment = EmptyString);

    /// Loads a text file and adds every line as an entry to a HashSet (omitting leading and trailing whitespace).
    /// Every line of the file should contain only one word. The words need to be in lowercase if you make use of an
    /// Analyzer which uses LowerCaseFilter (like StandardAnalyzer).
    ///
    /// @param reader Reader containing the wordlist
    /// @param comment The comment string to ignore
    /// @return A set with the file's words
    static HashSet<String> getWordSet(const ReaderPtr& reader, const String& comment = EmptyString);

    /// Reads a stem dictionary. Each line contains:
    /// <pre>word\tstem</pre>
    /// (ie. two tab separated words)
    /// @return stem dictionary that overrules the stemming algorithm
    static MapStringString getStemDict(const String& wordstemfile);
};

}

#endif
