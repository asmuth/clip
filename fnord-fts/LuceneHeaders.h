/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef LUCENEHEADERS_H
#define LUCENEHEADERS_H

#include "Lucene.h"

// Include most common files: analysis
#include "StandardAnalyzer.h"
#include "StandardFilter.h"
#include "StandardTokenizer.h"
#include "KeywordAnalyzer.h"
#include "KeywordTokenizer.h"
#include "LowerCaseFilter.h"
#include "LowerCaseTokenizer.h"
#include "PerFieldAnalyzerWrapper.h"
#include "PorterStemFilter.h"
#include "SimpleAnalyzer.h"
#include "StopAnalyzer.h"
#include "StopFilter.h"
#include "Token.h"
#include "TokenFilter.h"
#include "WhitespaceAnalyzer.h"
#include "WhitespaceTokenizer.h"

// Include most common files: document
#include "DateField.h"
#include "DateTools.h"
#include "Document.h"
#include "Field.h"
#include "NumberTools.h"
#include "NumericField.h"

// Include most common files: index
#include "IndexCommit.h"
#include "IndexDeletionPolicy.h"
#include "IndexReader.h"
#include "IndexWriter.h"
#include "KeepOnlyLastCommitDeletionPolicy.h"
#include "LogByteSizeMergePolicy.h"
#include "LogDocMergePolicy.h"
#include "LogMergePolicy.h"
#include "MergeScheduler.h"
#include "MultiReader.h"
#include "ParallelReader.h"
#include "Term.h"
#include "TermDocs.h"
#include "TermEnum.h"

// Include most common files: queryparser
#include "MultiFieldQueryParser.h"
#include "QueryParseError.h"
#include "QueryParser.h"

// Include most common files: search
#include "BooleanClause.h"
#include "BooleanQuery.h"
#include "DocIdSet.h"
#include "DocIdSetIterator.h"
#include "Explanation.h"
#include "IndexSearcher.h"
#include "MatchAllDocsQuery.h"
#include "MultiPhraseQuery.h"
#include "MultiSearcher.h"
#include "MultiTermQuery.h"
#include "NumericRangeFilter.h"
#include "NumericRangeQuery.h"
#include "ParallelMultiSearcher.h"
#include "PhraseQuery.h"
#include "PrefixFilter.h"
#include "PrefixQuery.h"
#include "ScoreDoc.h"
#include "Scorer.h"
#include "Searcher.h"
#include "Sort.h"
#include "TermQuery.h"
#include "TermRangeFilter.h"
#include "TermRangeQuery.h"
#include "TopDocs.h"
#include "TopDocsCollector.h"
#include "TopFieldCollector.h"
#include "TopScoreDocCollector.h"
#include "Weight.h"
#include "WildcardQuery.h"
#include "SpanFirstQuery.h"
#include "SpanNearQuery.h"
#include "SpanNotQuery.h"
#include "SpanOrQuery.h"
#include "SpanQuery.h"

// Include most common files: store
#include "FSDirectory.h"
#include "MMapDirectory.h"
#include "RAMDirectory.h"
#include "RAMFile.h"
#include "RAMInputStream.h"
#include "RAMOutputStream.h"
#include "SimpleFSDirectory.h"

// Include most common files: util
#include "MiscUtils.h"
#include "StringUtils.h"
#include "BufferedReader.h"
#include "DocIdBitSet.h"
#include "FileReader.h"
#include "InfoStream.h"
#include "LuceneThread.h"
#include "OpenBitSet.h"
#include "OpenBitSetDISI.h"
#include "OpenBitSetIterator.h"
#include "StringReader.h"
#include "ThreadPool.h"

#endif
