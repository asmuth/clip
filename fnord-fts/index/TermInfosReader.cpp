/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "TermInfosReader.h"
#include "SegmentTermEnum.h"
#include "Directory.h"
#include "IndexFileNames.h"
#include "Term.h"
#include "StringUtils.h"

namespace Lucene {

const int32_t TermInfosReader::DEFAULT_CACHE_SIZE = 1024;

TermInfosReader::TermInfosReader(const DirectoryPtr& dir, const String& seg, const FieldInfosPtr& fis, int32_t readBufferSize, int32_t indexDivisor) {
    bool success = false;

    if (indexDivisor < 1 && indexDivisor != -1) {
        boost::throw_exception(IllegalArgumentException(L"indexDivisor must be -1 (don't load terms index) or greater than 0: got " + StringUtils::toString(indexDivisor)));
    }

    LuceneException finally;
    try {
        directory = dir;
        segment = seg;
        fieldInfos = fis;

        origEnum = newLucene<SegmentTermEnum>(directory->openInput(segment + L"." + IndexFileNames::TERMS_EXTENSION(), readBufferSize), fieldInfos, false);
        _size = origEnum->size;

        if (indexDivisor != -1) {
            // Load terms index
            totalIndexInterval = origEnum->indexInterval * indexDivisor;
            SegmentTermEnumPtr indexEnum(newLucene<SegmentTermEnum>(directory->openInput(segment + L"." + IndexFileNames::TERMS_INDEX_EXTENSION(), readBufferSize), fieldInfos, true));

            try {
                int32_t indexSize = 1 + ((int32_t)indexEnum->size - 1) / indexDivisor; // otherwise read index

                indexTerms = Collection<TermPtr>::newInstance(indexSize);
                indexInfos = Collection<TermInfoPtr>::newInstance(indexSize);
                indexPointers = Collection<int64_t>::newInstance(indexSize);

                for (int32_t i = 0; indexEnum->next(); ++i) {
                    indexTerms[i] = indexEnum->term();
                    indexInfos[i] = indexEnum->termInfo();
                    indexPointers[i] = indexEnum->indexPointer;

                    for (int32_t j = 1; j < indexDivisor; ++j) {
                        if (!indexEnum->next()) {
                            break;
                        }
                    }
                }
            } catch (LuceneException& e) {
                finally = e;
            }
            indexEnum->close();
        } else {
            // Do not load terms index
            totalIndexInterval = -1;
        }
        success = true;
    } catch (LuceneException& e) {
        finally = e;
    }
    // With lock-less commits, it's entirely possible (and fine) to hit a FileNotFound exception above.
    // In this case, we want to explicitly close any subset of things that were opened.
    if (!success) {
        close();
    }
    finally.throwException();
}

TermInfosReader::~TermInfosReader() {
}

int32_t TermInfosReader::getMaxSkipLevels() {
    return origEnum->maxSkipLevels;
}

int32_t TermInfosReader::getSkipInterval() {
    return origEnum->skipInterval;
}

void TermInfosReader::close() {
    if (origEnum) {
        origEnum->close();
    }
    threadResources.close();
}

int64_t TermInfosReader::size() {
    return _size;
}

TermInfosReaderThreadResourcesPtr TermInfosReader::getThreadResources() {
    TermInfosReaderThreadResourcesPtr resources(threadResources.get());
    if (!resources) {
        resources = newLucene<TermInfosReaderThreadResources>();
        resources->termEnum = terms();

        // Cache does not have to be thread-safe, it is only used by one thread at the same time
        resources->termInfoCache = newInstance<TermInfoCache>(DEFAULT_CACHE_SIZE);
        threadResources.set(resources);
    }
    return resources;
}

int32_t TermInfosReader::getIndexOffset(const TermPtr& term) {
    // binary search indexTerms
    Collection<TermPtr>::iterator indexTerm = std::upper_bound(indexTerms.begin(), indexTerms.end(), term, luceneCompare<TermPtr>());
    return (std::distance(indexTerms.begin(), indexTerm) - 1);
}

void TermInfosReader::seekEnum(const SegmentTermEnumPtr& enumerator, int32_t indexOffset) {
    enumerator->seek(indexPointers[indexOffset], ((int64_t)indexOffset * (int64_t)totalIndexInterval) - 1, indexTerms[indexOffset], indexInfos[indexOffset]);
}

TermInfoPtr TermInfosReader::get(const TermPtr& term) {
    return get(term, true);
}

TermInfoPtr TermInfosReader::get(const TermPtr& term, bool useCache) {
    if (_size == 0) {
        return TermInfoPtr();
    }

    ensureIndexIsRead();

    TermInfoPtr ti;
    TermInfosReaderThreadResourcesPtr resources(getThreadResources());
    TermInfoCachePtr cache;

    if (useCache) {
        cache = resources->termInfoCache;
        // check the cache first if the term was recently looked up
        ti = cache->get(term);
        if (ti) {
            return ti;
        }
    }

    // optimize sequential access: first try scanning cached enum without seeking
    SegmentTermEnumPtr enumerator = resources->termEnum;

    if (enumerator->term() && // term is at or past current
            ((enumerator->prev() && term->compareTo(enumerator->prev()) > 0) ||
             term->compareTo(enumerator->term()) >= 0)) {
        int32_t enumOffset = (int32_t)(enumerator->position / totalIndexInterval ) + 1;
        if (indexTerms.size() == enumOffset || // but before end of block
                term->compareTo(indexTerms[enumOffset]) < 0) {
            // no need to seek
            int32_t numScans = enumerator->scanTo(term);
            if (enumerator->term() && term->compareTo(enumerator->term()) == 0) {
                ti = enumerator->termInfo();
                if (cache && numScans > 1) {
                    // we only want to put this TermInfo into the cache if scanEnum skipped more
                    // than one dictionary entry. This prevents RangeQueries or WildcardQueries to
                    // wipe out the cache when they iterate over a large numbers of terms in order.
                    cache->put(term, ti);
                }
            } else {
                ti.reset();
            }
            return ti;
        }
    }

    // random-access: must seek
    seekEnum(enumerator, getIndexOffset(term));
    enumerator->scanTo(term);
    if (enumerator->term() && term->compareTo(enumerator->term()) == 0) {
        ti = enumerator->termInfo();
        if (cache) {
            cache->put(term, ti);
        }
    } else {
        ti.reset();
    }
    return ti;
}

void TermInfosReader::ensureIndexIsRead() {
    if (!indexTerms) {
        boost::throw_exception(IllegalStateException(L"terms index was not loaded when this reader was created"));
    }
}

int64_t TermInfosReader::getPosition(const TermPtr& term) {
    if (_size == 0) {
        return -1;
    }

    ensureIndexIsRead();
    int32_t indexOffset = getIndexOffset(term);

    SegmentTermEnumPtr enumerator(getThreadResources()->termEnum);
    seekEnum(enumerator, indexOffset);

    while (term->compareTo(enumerator->term()) > 0 && enumerator->next()) {
    }

    return term->compareTo(enumerator->term()) == 0 ? enumerator->position : -1;
}

SegmentTermEnumPtr TermInfosReader::terms() {
    return boost::static_pointer_cast<SegmentTermEnum>(origEnum->clone());
}

SegmentTermEnumPtr TermInfosReader::terms(const TermPtr& term) {
    // don't use the cache in this call because we want to reposition the enumeration
    get(term, false);
    return boost::static_pointer_cast<SegmentTermEnum>(getThreadResources()->termEnum->clone());
}

TermInfosReaderThreadResources::~TermInfosReaderThreadResources() {
}

}
