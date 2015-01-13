/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef LUCENE_H
#define LUCENE_H

//#include "Config.h"

#include <wctype.h>
#include <wchar.h>
#include <sys/types.h>
#include <sstream>
#include <string>
#include <algorithm>
#include <limits>
#include <stdexcept>

#include <boost/cstdint.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/variant.hpp>

#define SIZEOF_ARRAY(arr) (sizeof(arr) / sizeof((arr)[0]))

#include "LuceneTypes.h"
#include "fnord-fts/util/LuceneAllocator.h"

namespace boost {

struct blank;
class thread;
class any;
template < typename Signature > class function;
namespace interprocess {

class file_lock;
}
namespace posix_time {

class ptime;
}

}

namespace Lucene {

typedef std::basic_string< char, std::char_traits<char> > SingleString;
typedef std::basic_ostringstream< char, std::char_traits<char> > SingleStringStream;
typedef std::basic_string< wchar_t, std::char_traits<wchar_t> > String;
typedef std::basic_ostringstream< wchar_t, std::char_traits<wchar_t> > StringStream;

const std::basic_string< wchar_t, std::char_traits<wchar_t> > EmptyString;

typedef std::shared_ptr<boost::interprocess::file_lock> filelockPtr;
typedef std::shared_ptr<boost::thread> threadPtr;

typedef std::shared_ptr<boost::filesystem::ofstream> ofstreamPtr;
typedef std::shared_ptr<boost::filesystem::ifstream> ifstreamPtr;
typedef std::shared_ptr<std::locale> localePtr;
}

#include "LuceneFactory.h"
#include "fnord-fts/util/LuceneException.h"
#include "Array.h"
#include "Collection.h"
#include "Map.h"
#include "Set.h"
#include "HashMap.h"
#include "HashSet.h"
#include "fnord-fts/util/Constants.h"

namespace Lucene {

typedef Array<uint8_t> ByteArray;
typedef Array<int32_t> IntArray;
typedef Array<int64_t> LongArray;
typedef Array<wchar_t> CharArray;
typedef Array<double> DoubleArray;

template <class TYPE>
struct luceneEquals {
    inline bool operator()(const TYPE& first, const TYPE& second) const {
        return first ? first->equals(second) : (!first && !second);
    }
};

template <class TYPE>
struct luceneEqualTo {
    luceneEqualTo(const TYPE& type) : equalType(type) {}
    inline bool operator()(const TYPE& other) const {
        return equalType->equals(other);
    }
    const TYPE& equalType;
};

template <class TYPE>
struct luceneWeakEquals {
    inline bool operator()(const TYPE& first, const TYPE& second) const {
        if (first.expired() || second.expired()) {
            return (first.expired() && second.expired());
        }
        return first.lock()->equals(second.lock());
    }
};

template <class TYPE>
struct luceneHash : std::unary_function<TYPE, std::size_t> {
    std::size_t operator()(const TYPE& type) const {
        return type ? type->hashCode() : 0;
    }
};

template <class TYPE>
struct luceneWeakHash : std::unary_function<TYPE, std::size_t> {
    std::size_t operator()(const TYPE& type) const {
        return type.expired() ? 0 : type.lock()->hashCode();
    }
};

template <class TYPE>
struct luceneCompare {
    inline bool operator()(const TYPE& first, const TYPE& second) const {
        if (!second) {
            return false;
        }
        if (!first) {
            return true;
        }
        return (first->compareTo(second) < 0);
    }
};

typedef boost::blank VariantNull;
typedef boost::variant<String, int32_t, int64_t, double, ReaderPtr, ByteArray, VariantNull> FieldsData;
typedef boost::variant<String, uint8_t, int32_t, int64_t, double, VariantNull> ComparableValue;
typedef boost::variant<int32_t, int64_t, double, VariantNull> NumericValue;
typedef boost::variant<String, VariantNull> StringValue;
typedef boost::variant<Collection<uint8_t>, Collection<int32_t>, Collection<double>, VariantNull> CollectionValue;

typedef HashSet< SegmentInfoPtr, luceneHash<SegmentInfoPtr>, luceneEquals<SegmentInfoPtr> > SetSegmentInfo;
typedef HashSet< MergeThreadPtr, luceneHash<MergeThreadPtr>, luceneEquals<MergeThreadPtr> > SetMergeThread;
typedef HashSet< OneMergePtr, luceneHash<OneMergePtr>, luceneEquals<OneMergePtr> > SetOneMerge;
typedef HashSet< QueryPtr, luceneHash<QueryPtr>, luceneEquals<QueryPtr> > SetQuery;
typedef HashSet< TermPtr, luceneHash<TermPtr>, luceneEquals<TermPtr> > SetTerm;
typedef HashSet< BooleanClausePtr, luceneHash<BooleanClausePtr>, luceneEquals<BooleanClausePtr> > SetBooleanClause;
typedef HashSet< ReaderFieldPtr, luceneHash<ReaderFieldPtr>, luceneEquals<ReaderFieldPtr> > SetReaderField;
typedef HashSet<ByteArray> SetByteArray;

typedef HashMap< String, String > MapStringString;
typedef HashMap< wchar_t, NormalizeCharMapPtr > MapCharNormalizeCharMap;
typedef HashMap< String, AnalyzerPtr > MapStringAnalyzer;
typedef HashMap< String, ByteArray > MapStringByteArray;
typedef HashMap< String, int32_t > MapStringInt;
typedef HashMap< String, FieldInfoPtr > MapStringFieldInfo;
typedef HashMap< String, Collection<TermVectorEntryPtr> > MapStringCollectionTermVectorEntry;
typedef HashMap< String, RefCountPtr > MapStringRefCount;
typedef HashMap< int32_t, TermVectorsPositionInfoPtr > MapIntTermVectorsPositionInfo;
typedef HashMap< String, MapIntTermVectorsPositionInfo > MapStringMapIntTermVectorsPositionInfo;
typedef HashMap< String, NormPtr > MapStringNorm;
typedef HashMap< String, TermVectorEntryPtr > MapStringTermVectorEntry;
typedef HashMap< String, RAMFilePtr > MapStringRAMFile;
typedef HashMap< int32_t, ByteArray > MapIntByteArray;
typedef HashMap< int32_t, FilterItemPtr > MapIntFilterItem;
typedef HashMap< int32_t, double > MapIntDouble;
typedef HashMap< int64_t, int32_t > MapLongInt;
typedef HashMap< String, double > MapStringDouble;
typedef HashMap< int32_t, CachePtr > MapStringCache;
typedef HashMap< String, LockPtr > MapStringLock;

typedef HashMap< SegmentInfoPtr, SegmentReaderPtr, luceneHash<SegmentInfoPtr>, luceneEquals<SegmentInfoPtr> > MapSegmentInfoSegmentReader;
typedef HashMap< SegmentInfoPtr, int32_t, luceneHash<SegmentInfoPtr>, luceneEquals<SegmentInfoPtr> > MapSegmentInfoInt;
typedef HashMap< DocFieldConsumerPerThreadPtr, Collection<DocFieldConsumerPerFieldPtr>, luceneHash<DocFieldConsumerPerThreadPtr>, luceneEquals<DocFieldConsumerPerThreadPtr> > MapDocFieldConsumerPerThreadCollectionDocFieldConsumerPerField;
typedef HashMap< InvertedDocConsumerPerThreadPtr, Collection<InvertedDocConsumerPerFieldPtr>, luceneHash<InvertedDocConsumerPerThreadPtr>, luceneEquals<InvertedDocConsumerPerThreadPtr> > MapInvertedDocConsumerPerThreadCollectionInvertedDocConsumerPerField;
typedef HashMap< InvertedDocEndConsumerPerThreadPtr, Collection<InvertedDocEndConsumerPerFieldPtr>, luceneHash<InvertedDocEndConsumerPerThreadPtr>, luceneEquals<InvertedDocEndConsumerPerThreadPtr> > MapInvertedDocEndConsumerPerThreadCollectionInvertedDocEndConsumerPerField;
typedef HashMap< TermsHashConsumerPerThreadPtr, Collection<TermsHashConsumerPerFieldPtr>, luceneHash<TermsHashConsumerPerThreadPtr>, luceneEquals<TermsHashConsumerPerThreadPtr> > MapTermsHashConsumerPerThreadCollectionTermsHashConsumerPerField;
typedef HashMap< FieldInfoPtr, Collection<NormsWriterPerFieldPtr>, luceneHash<FieldInfoPtr>, luceneEquals<FieldInfoPtr> > MapFieldInfoCollectionNormsWriterPerField;
typedef HashMap< IndexReaderPtr, HashSet<String>, luceneHash<IndexReaderPtr>, luceneEquals<IndexReaderPtr> > MapIndexReaderSetString;
typedef HashMap< TermPtr, int32_t, luceneHash<TermPtr>, luceneEquals<TermPtr> > MapTermInt;
typedef HashMap< QueryPtr, int32_t, luceneHash<QueryPtr>, luceneEquals<QueryPtr> > MapQueryInt;
typedef HashMap< EntryPtr, boost::any, luceneHash<EntryPtr>, luceneEquals<EntryPtr> > MapEntryAny;
typedef HashMap< PhrasePositionsPtr, LuceneObjectPtr, luceneHash<PhrasePositionsPtr>, luceneEquals<PhrasePositionsPtr> > MapPhrasePositionsLuceneObject;
typedef HashMap< ReaderFieldPtr, SetReaderField, luceneHash<ReaderFieldPtr>, luceneEquals<ReaderFieldPtr> > MapReaderFieldSetReaderField;

typedef WeakHashMap< LuceneObjectWeakPtr, LuceneObjectPtr, luceneWeakHash<LuceneObjectWeakPtr>, luceneWeakEquals<LuceneObjectWeakPtr> > WeakMapObjectObject;
typedef WeakHashMap< LuceneObjectWeakPtr, MapEntryAny, luceneWeakHash<LuceneObjectWeakPtr>, luceneWeakEquals<LuceneObjectWeakPtr> > WeakMapLuceneObjectMapEntryAny;

typedef Map< String, AttributePtr > MapStringAttribute;
typedef Map< int64_t, DocumentsWriterThreadStatePtr > MapThreadDocumentsWriterThreadState;
typedef Map< String, IndexReaderPtr > MapStringIndexReader;
typedef Map< TermPtr, NumPtr, luceneCompare<TermPtr> > MapTermNum;

typedef boost::function<bool (const TermVectorEntryPtr&, const TermVectorEntryPtr&)> TermVectorEntryComparator;

template < class KEY, class VALUE, class HASH = boost::hash<KEY>, class EQUAL = std::equal_to<KEY> > class SimpleLRUCache;
typedef SimpleLRUCache< TermPtr, TermInfoPtr, luceneHash<TermPtr>, luceneEquals<TermPtr> > TermInfoCache;
typedef std::shared_ptr<TermInfoCache> TermInfoCachePtr;
}

#include "fnord-fts/util/Synchronize.h"
#include "fnord-fts/util/CycleCheck.h"
#if defined(LPP_BUILDING_LIB) || defined(LPP_EXPOSE_INTERNAL)
#define INTERNAL public
#else
#define INTERNAL protected
#endif

#endif
