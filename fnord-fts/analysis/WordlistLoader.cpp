/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include <boost/algorithm/string.hpp>
#include "WordlistLoader.h"
#include "FileReader.h"
#include "BufferedReader.h"

namespace Lucene {

WordlistLoader::~WordlistLoader() {
}

HashSet<String> WordlistLoader::getWordSet(const String& wordfile, const String& comment) {
    HashSet<String> result(HashSet<String>::newInstance());
    FileReaderPtr reader;
    LuceneException finally;
    try {
        reader = newLucene<FileReader>(wordfile);
        result = getWordSet(reader, comment);
    } catch (LuceneException& e) {
        finally = e;
    }
    if (reader) {
        reader->close();
    }
    finally.throwException();
    return result;
}

HashSet<String> WordlistLoader::getWordSet(const ReaderPtr& reader, const String& comment) {
    HashSet<String> result(HashSet<String>::newInstance());
    LuceneException finally;
    BufferedReaderPtr bufferedReader(boost::dynamic_pointer_cast<BufferedReader>(reader));
    try {
        if (!bufferedReader) {
            bufferedReader = newLucene<BufferedReader>(reader);
        }
        String word;
        while (bufferedReader->readLine(word)) {
            if (comment.empty() || !boost::starts_with(word, comment)) {
                boost::trim(word);
                result.add(word);
            }
        }
    } catch (LuceneException& e) {
        finally = e;
    }
    if (bufferedReader) {
        bufferedReader->close();
    }
    finally.throwException();
    return result;
}

MapStringString WordlistLoader::getStemDict(const String& wordstemfile) {
    MapStringString result(MapStringString::newInstance());
    BufferedReaderPtr bufferedReader;
    FileReaderPtr reader;
    LuceneException finally;
    try {
        reader = newLucene<FileReader>(wordstemfile);
        bufferedReader = newLucene<BufferedReader>(reader);
        String line;
        while (bufferedReader->readLine(line)) {
            String::size_type sep = line.find(L'\t');
            if (sep != String::npos) {
                result.put(line.substr(0, sep), line.substr(sep + 1));
            }
        }
    } catch (LuceneException& e) {
        finally = e;
    }
    if (reader) {
        reader->close();
    }
    if (bufferedReader) {
        bufferedReader->close();
    }
    finally.throwException();
    return result;
}

}
