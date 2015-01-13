/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef _FIELDCACHERANGEFILTER_H
#define _FIELDCACHERANGEFILTER_H

#include "Filter.h"
#include "DocIdSet.h"
#include "DocIdSetIterator.h"
#include "MiscUtils.h"
#include "StringUtils.h"

namespace Lucene {

class FieldCacheRangeFilterString : public FieldCacheRangeFilter {
public:
    FieldCacheRangeFilterString(const String& field, const ParserPtr& parser, const String& lowerVal, const String& upperVal, bool includeLower, bool includeUpper);
    virtual ~FieldCacheRangeFilterString();

    LUCENE_CLASS(FieldCacheRangeFilterString);

public:
    String lowerVal;
    String upperVal;

public:
    virtual DocIdSetPtr getDocIdSet(const IndexReaderPtr& reader);

    virtual String toString();
    virtual bool equals(const LuceneObjectPtr& other);
    virtual int32_t hashCode();
};

class FieldCacheDocIdSet : public DocIdSet {
public:
    FieldCacheDocIdSet(const IndexReaderPtr& reader, bool mayUseTermDocs);
    virtual ~FieldCacheDocIdSet();

    LUCENE_CLASS(FieldCacheDocIdSet);

protected:
    IndexReaderPtr reader;
    bool mayUseTermDocs;

public:
    /// This method checks, if a doc is a hit, should throw ArrayIndexOutOfBounds, when position invalid
    virtual bool matchDoc(int32_t doc) = 0;

    /// This DocIdSet is cacheable, if it works solely with FieldCache and no TermDocs.
    virtual bool isCacheable();

    virtual DocIdSetIteratorPtr iterator();
};

template <typename TYPE>
class FieldCacheDocIdSetNumeric : public FieldCacheDocIdSet {
public:
    FieldCacheDocIdSetNumeric(const IndexReaderPtr& reader, bool mayUseTermDocs, Collection<TYPE> values, TYPE inclusiveLowerPoint, TYPE inclusiveUpperPoint) : FieldCacheDocIdSet(reader, mayUseTermDocs) {
        this->values = values;
        this->inclusiveLowerPoint = inclusiveLowerPoint;
        this->inclusiveUpperPoint = inclusiveUpperPoint;
    }

    virtual ~FieldCacheDocIdSetNumeric() {
    }

protected:
    Collection<TYPE> values;
    TYPE inclusiveLowerPoint;
    TYPE inclusiveUpperPoint;

public:
    virtual bool matchDoc(int32_t doc) {
        if (doc < 0 || doc >= values.size()) {
            boost::throw_exception(IndexOutOfBoundsException());
        }
        return (values[doc] >= inclusiveLowerPoint && values[doc] <= inclusiveUpperPoint);
    }
};

template <typename TYPE>
class FieldCacheRangeFilterNumeric : public FieldCacheRangeFilter {
public:
    FieldCacheRangeFilterNumeric(const String& field, const ParserPtr& parser, TYPE lowerVal, TYPE upperVal, TYPE maxVal, bool includeLower, bool includeUpper) : FieldCacheRangeFilter(field, parser, includeLower, includeUpper) {
        this->lowerVal = lowerVal;
        this->upperVal = upperVal;
        this->maxVal = maxVal;
    }

    virtual ~FieldCacheRangeFilterNumeric() {
    }

public:
    TYPE lowerVal;
    TYPE upperVal;
    TYPE maxVal;

public:
    virtual DocIdSetPtr getDocIdSet(const IndexReaderPtr& reader) {
        if (!includeLower && lowerVal == maxVal) {
            return DocIdSet::EMPTY_DOCIDSET();
        }
        int64_t inclusiveLowerPoint = (int64_t)(includeLower ? lowerVal : (lowerVal + 1));

        if (!includeUpper && upperVal == 0) {
            return DocIdSet::EMPTY_DOCIDSET();
        }
        int64_t inclusiveUpperPoint = (int64_t)(includeUpper ? upperVal : (upperVal - 1));

        if (inclusiveLowerPoint > inclusiveUpperPoint) {
            return DocIdSet::EMPTY_DOCIDSET();
        }

        // we only request the usage of termDocs, if the range contains 0
        return newLucene< FieldCacheDocIdSetNumeric<TYPE> >(reader, (inclusiveLowerPoint <= 0 && inclusiveUpperPoint >= 0), getValues(reader), inclusiveLowerPoint, inclusiveUpperPoint);
    }

    virtual Collection<TYPE> getValues(const IndexReaderPtr& reader) = 0;

    virtual String toString() {
        StringStream buffer;
        buffer << field << L":" << (includeLower ? L"[" : L"{");
        buffer << lowerVal << L" TO " << lowerVal;
        buffer << (includeLower ? L"]" : L"}");
        return buffer.str();
    }

    virtual bool equals(const LuceneObjectPtr& other) {
        if (Filter::equals(other)) {
            return true;
        }
        boost::shared_ptr< FieldCacheRangeFilterNumeric<TYPE> > otherFilter(boost::dynamic_pointer_cast< FieldCacheRangeFilterNumeric<TYPE> >(other));
        if (!otherFilter) {
            return false;
        }
        if (field != otherFilter->field || includeLower != otherFilter->includeLower || includeUpper != otherFilter->includeUpper) {
            return false;
        }
        if (lowerVal != otherFilter->lowerVal || upperVal != otherFilter->upperVal) {
            return false;
        }
        if (parser.get() != NULL ? !parser->equals(otherFilter->parser) : otherFilter->parser.get() != NULL) {
            return false;
        }
        return true;
    }

    int32_t hashCode() {
        int32_t code = StringUtils::hashCode(field);
        code ^= lowerVal == 0 ? 550356204 : (int32_t)lowerVal;
        code = (code << 1) | MiscUtils::unsignedShift(code, 31); // rotate to distinguish lower from upper
        code ^= upperVal == 0 ? -1674416163 : (int32_t)upperVal;
        code ^= parser ? parser->hashCode() : -1572457324;
        code ^= (includeLower ? 1549299360 : -365038026) ^ (includeUpper ? 1721088258 : 1948649653);
        return code;
    }
};

class FieldCacheRangeFilterByte : public FieldCacheRangeFilterNumeric<uint8_t> {
public:
    FieldCacheRangeFilterByte(const String& field, const ParserPtr& parser, uint8_t lowerVal, uint8_t upperVal, bool includeLower, bool includeUpper);
    virtual ~FieldCacheRangeFilterByte();

    LUCENE_CLASS(FieldCacheRangeFilterByte);

public:
    virtual Collection<uint8_t> getValues(const IndexReaderPtr& reader);
};

class FieldCacheRangeFilterInt : public FieldCacheRangeFilterNumeric<int32_t> {
public:
    FieldCacheRangeFilterInt(const String& field, const ParserPtr& parser, int32_t lowerVal, int32_t upperVal, bool includeLower, bool includeUpper);
    virtual ~FieldCacheRangeFilterInt();

    LUCENE_CLASS(FieldCacheRangeFilterInt);

public:
    virtual Collection<int32_t> getValues(const IndexReaderPtr& reader);
};

class FieldCacheRangeFilterLong : public FieldCacheRangeFilterNumeric<int64_t> {
public:
    FieldCacheRangeFilterLong(const String& field, const ParserPtr& parser, int64_t lowerVal, int64_t upperVal, bool includeLower, bool includeUpper);
    virtual ~FieldCacheRangeFilterLong();

    LUCENE_CLASS(FieldCacheRangeFilterLong);

public:
    virtual Collection<int64_t> getValues(const IndexReaderPtr& reader);
};

class FieldCacheRangeFilterDouble : public FieldCacheRangeFilterNumeric<double> {
public:
    FieldCacheRangeFilterDouble(const String& field, const ParserPtr& parser, double lowerVal, double upperVal, bool includeLower, bool includeUpper);
    virtual ~FieldCacheRangeFilterDouble();

    LUCENE_CLASS(FieldCacheRangeFilterDouble);

public:
    virtual DocIdSetPtr getDocIdSet(const IndexReaderPtr& reader);
    virtual Collection<double> getValues(const IndexReaderPtr& reader);
};

class FieldCacheDocIdSetString : public FieldCacheDocIdSet {
public:
    FieldCacheDocIdSetString(const IndexReaderPtr& reader, bool mayUseTermDocs, const StringIndexPtr& fcsi, int32_t inclusiveLowerPoint, int32_t inclusiveUpperPoint);
    virtual ~FieldCacheDocIdSetString();

    LUCENE_CLASS(FieldCacheDocIdSetString);

protected:
    StringIndexPtr fcsi;
    int32_t inclusiveLowerPoint;
    int32_t inclusiveUpperPoint;

public:
    virtual bool matchDoc(int32_t doc);
};

/// A DocIdSetIterator using TermDocs to iterate valid docIds
class FieldDocIdSetIteratorTermDocs : public DocIdSetIterator {
public:
    FieldDocIdSetIteratorTermDocs(const FieldCacheDocIdSetPtr& cacheDocIdSet, const TermDocsPtr& termDocs);
    virtual ~FieldDocIdSetIteratorTermDocs();

    LUCENE_CLASS(FieldDocIdSetIteratorTermDocs);

protected:
    FieldCacheDocIdSetWeakPtr _cacheDocIdSet;
    TermDocsPtr termDocs;
    int32_t doc;

public:
    virtual int32_t docID();
    virtual int32_t nextDoc();
    virtual int32_t advance(int32_t target);
};

/// A DocIdSetIterator generating docIds by incrementing a variable - this one can be used if there
/// are no deletions are on the index.
class FieldDocIdSetIteratorIncrement : public DocIdSetIterator {
public:
    FieldDocIdSetIteratorIncrement(const FieldCacheDocIdSetPtr& cacheDocIdSet);
    virtual ~FieldDocIdSetIteratorIncrement();

    LUCENE_CLASS(FieldDocIdSetIteratorIncrement);

protected:
    FieldCacheDocIdSetWeakPtr _cacheDocIdSet;
    int32_t doc;

public:
    virtual int32_t docID();
    virtual int32_t nextDoc();
    virtual int32_t advance(int32_t target);
};

}

#endif
