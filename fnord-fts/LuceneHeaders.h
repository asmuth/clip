/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef LUCENEHEADERS_H
#define LUCENEHEADERS_H

#include "fnord-fts/Lucene.h"

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
#include "fnord-fts/analysis/Token.h"
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
#include "fnord-fts/index/IndexReader.h"
#include "IndexWriter.h"
#include "KeepOnlyLastCommitDeletionPolicy.h"
#include "LogByteSizeMergePolicy.h"
#include "LogDocMergePolicy.h"
#include "LogMergePolicy.h"
#include "MergeScheduler.h"
#include "MultiReader.h"
#include "ParallelReader.h"
#include "fnord-fts/index/Term.h"
#include "TermDocs.h"
#include "fnord-fts/index/TermEnum.h"

// Include most common files: queryparser
#include "MultiFieldQueryParser.h"
#include "QueryParseError.h"
#include "QueryParser.h"

// Include most common files: search
#include "fnord-fts/search/BooleanClause.h"
#include "fnord-fts/search/BooleanQuery.h"
#include "fnord-fts/search/DocIdSet.h"
#include "DocIdSetIterator.h"
#include "Explanation.h"
#include "IndexSearcher.h"
#include "MatchAllDocsQuery.h"
#include "fnord-fts/search/MultiPhraseQuery.h"
#include "MultiSearcher.h"
#include "MultiTermQuery.h"
#include "NumericRangeFilter.h"
#include "NumericRangeQuery.h"
#include "ParallelMultiSearcher.h"
#include "fnord-fts/search/PhraseQuery.h"
#include "PrefixFilter.h"
#include "PrefixQuery.h"
#include "ScoreDoc.h"
#include "fnord-fts/search/Scorer.h"
#include "Searcher.h"
#include "Sort.h"
#include "fnord-fts/search/TermQuery.h"
#include "TermRangeFilter.h"
#include "TermRangeQuery.h"
#include "TopDocs.h"
#include "TopDocsCollector.h"
#include "TopFieldCollector.h"
#include "TopScoreDocCollector.h"
#include "fnord-fts/search/Weight.h"
#include "WildcardQuery.h"
#include "fnord-fts/search/spans/SpanFirstQuery.h"
#include "fnord-fts/search/spans/SpanNearQuery.h"
#include "fnord-fts/search/spans/SpanNotQuery.h"
#include "fnord-fts/search/spans/SpanOrQuery.h"
#include "fnord-fts/search/spans/SpanQuery.h"

// Include most common files: store
#include "FSDirectory.h"
#include "MMapDirectory.h"
#include "RAMDirectory.h"
#include "RAMFile.h"
#include "RAMInputStream.h"
#include "RAMOutputStream.h"
#include "SimpleFSDirectory.h"

// Include most common files: util
#include "fnord-fts/util/MiscUtils.h"
#include "fnord-fts/util/StringUtils.h"
#include "BufferedReader.h"
#include "DocIdBitSet.h"
#include "FileReader.h"
#include "fnord-fts/util/InfoStream.h"
#include "fnord-fts/util/LuceneThread.h"
#include "OpenBitSet.h"
#include "fnord-fts/util/OpenBitSetDISI.h"
#include "OpenBitSetIterator.h"
#include "StringReader.h"
#include "fnord-fts/util/ThreadPool.h"

#endif
