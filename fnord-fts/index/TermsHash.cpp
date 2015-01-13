/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "TermsHash.h"
#include "DocumentsWriter.h"
#include "TermsHashConsumer.h"
#include "TermsHashPerThread.h"
#include "TermsHashPerField.h"
#include "TermsHashConsumerPerThread.h"
#include "DocInverterPerThread.h"
#include "TermsHashConsumerPerField.h"
#include "IndexWriter.h"
#include "MiscUtils.h"

namespace Lucene {

TermsHash::TermsHash(const DocumentsWriterPtr& docWriter, bool trackAllocations, const TermsHashConsumerPtr& consumer, const TermsHashPtr& nextTermsHash) {
    this->postingsFreeCount = 0;
    this->postingsAllocCount = 0;
    this->trackAllocations = false;
    this->postingsFreeList = Collection<RawPostingListPtr>::newInstance(1);

    this->_docWriter = docWriter;
    this->consumer = consumer;
    this->nextTermsHash = nextTermsHash;
    this->trackAllocations = trackAllocations;

    bytesPerPosting = consumer->bytesPerPosting() + 4 * DocumentsWriter::POINTER_NUM_BYTE;
    postingsFreeChunk = (int32_t)((double)DocumentsWriter::BYTE_BLOCK_SIZE / (double)bytesPerPosting);
}

TermsHash::~TermsHash() {
}

InvertedDocConsumerPerThreadPtr TermsHash::addThread(const DocInverterPerThreadPtr& docInverterPerThread) {
    return newLucene<TermsHashPerThread>(docInverterPerThread, shared_from_this(), nextTermsHash, TermsHashPerThreadPtr());
}

TermsHashPerThreadPtr TermsHash::addThread(const DocInverterPerThreadPtr& docInverterPerThread, const TermsHashPerThreadPtr& primaryPerThread) {
    return newLucene<TermsHashPerThread>(docInverterPerThread, shared_from_this(), nextTermsHash, primaryPerThread);
}

void TermsHash::setFieldInfos(const FieldInfosPtr& fieldInfos) {
    this->fieldInfos = fieldInfos;
    consumer->setFieldInfos(fieldInfos);
}

void TermsHash::abort() {
    consumer->abort();
    if (nextTermsHash) {
        nextTermsHash->abort();
    }
}

void TermsHash::shrinkFreePostings(MapInvertedDocConsumerPerThreadCollectionInvertedDocConsumerPerField threadsAndFields, const SegmentWriteStatePtr& state) {
    BOOST_ASSERT(postingsFreeCount == postingsAllocCount);

    int32_t newSize = 1;
    if (newSize != postingsFreeList.size()) {
        if (postingsFreeCount > newSize) {
            if (trackAllocations) {
                DocumentsWriterPtr(_docWriter)->bytesAllocated(-(postingsFreeCount - newSize) * bytesPerPosting);
            }
            postingsFreeCount = newSize;
            postingsAllocCount = newSize;
        }
        postingsFreeList.resize(newSize);
    }
}

void TermsHash::closeDocStore(const SegmentWriteStatePtr& state) {
    SyncLock syncLock(this);
    consumer->closeDocStore(state);
    if (nextTermsHash) {
        nextTermsHash->closeDocStore(state);
    }
}

void TermsHash::flush(MapInvertedDocConsumerPerThreadCollectionInvertedDocConsumerPerField threadsAndFields, const SegmentWriteStatePtr& state) {
    SyncLock syncLock(this);
    MapTermsHashConsumerPerThreadCollectionTermsHashConsumerPerField childThreadsAndFields(MapTermsHashConsumerPerThreadCollectionTermsHashConsumerPerField::newInstance());
    MapInvertedDocConsumerPerThreadCollectionInvertedDocConsumerPerField nextThreadsAndFields;
    if (nextTermsHash) {
        nextThreadsAndFields = MapInvertedDocConsumerPerThreadCollectionInvertedDocConsumerPerField::newInstance();
    }

    for (MapInvertedDocConsumerPerThreadCollectionInvertedDocConsumerPerField::iterator entry = threadsAndFields.begin(); entry != threadsAndFields.end(); ++entry) {
        Collection<TermsHashConsumerPerFieldPtr> childFields(Collection<TermsHashConsumerPerFieldPtr>::newInstance());
        Collection<InvertedDocConsumerPerFieldPtr> nextChildFields;
        if (nextTermsHash) {
            nextChildFields = Collection<InvertedDocConsumerPerFieldPtr>::newInstance();
        }

        for (Collection<InvertedDocConsumerPerFieldPtr>::iterator perField = entry->second.begin(); perField != entry->second.end(); ++perField) {
            childFields.add(boost::static_pointer_cast<TermsHashPerField>(*perField)->consumer);
            if (nextTermsHash) {
                nextChildFields.add(boost::static_pointer_cast<TermsHashPerField>(*perField)->nextPerField);
            }
        }

        childThreadsAndFields.put(boost::static_pointer_cast<TermsHashPerThread>(entry->first)->consumer, childFields);
        if (nextTermsHash) {
            nextThreadsAndFields.put(boost::static_pointer_cast<TermsHashPerThread>(entry->first)->nextPerThread, nextChildFields);
        }
    }

    consumer->flush(childThreadsAndFields, state);

    shrinkFreePostings(threadsAndFields, state);

    if (nextTermsHash) {
        nextTermsHash->flush(nextThreadsAndFields, state);
    }
}

bool TermsHash::freeRAM() {
    if (!trackAllocations) {
        return false;
    }

    bool any = false;
    int64_t bytesFreed = 0;
    {
        SyncLock syncLock(this);
        int32_t numToFree = postingsFreeCount >= postingsFreeChunk ? postingsFreeChunk : postingsFreeCount;
        any = (numToFree > 0);
        if (any) {
            MiscUtils::arrayFill(postingsFreeList.begin(), postingsFreeCount - numToFree, postingsFreeCount, RawPostingListPtr());
            postingsFreeCount -= numToFree;
            postingsAllocCount -= numToFree;
            bytesFreed = -numToFree * bytesPerPosting;
            any = true;
        }
    }

    if (any) {
        DocumentsWriterPtr(_docWriter)->bytesAllocated(bytesFreed);
    }

    if (nextTermsHash && nextTermsHash->freeRAM()) {
        any = true;
    }

    return any;
}

void TermsHash::recyclePostings(Collection<RawPostingListPtr> postings, int32_t numPostings) {
    SyncLock syncLock(this);
    BOOST_ASSERT(postings.size() >= numPostings);

    // Move all Postings from this ThreadState back to our free list.  We pre-allocated this array while we
    // were creating Postings to make sure it's large enough
    BOOST_ASSERT(postingsFreeCount + numPostings <= postingsFreeList.size());
    MiscUtils::arrayCopy(postings.begin(), 0, postingsFreeList.begin(), postingsFreeCount, numPostings);
    postingsFreeCount += numPostings;
}

void TermsHash::getPostings(Collection<RawPostingListPtr> postings) {
    SyncLock syncLock(this);
    DocumentsWriterPtr docWriter(_docWriter);
    IndexWriterPtr writer(docWriter->_writer);

    BOOST_ASSERT(writer->testPoint(L"TermsHash.getPostings start"));

    BOOST_ASSERT(postingsFreeCount <= postingsFreeList.size());
    BOOST_ASSERT(postingsFreeCount <= postingsAllocCount);

    int32_t numToCopy = postingsFreeCount < postings.size() ? postingsFreeCount : postings.size();
    int32_t start = postingsFreeCount - numToCopy;
    BOOST_ASSERT(start >= 0);
    BOOST_ASSERT(start + numToCopy <= postingsFreeList.size());
    BOOST_ASSERT(numToCopy <= postings.size());
    MiscUtils::arrayCopy(postingsFreeList.begin(), start, postings.begin(), 0, numToCopy);

    // Directly allocate the remainder if any
    if (numToCopy != postings.size()) {
        int32_t extra = postings.size() - numToCopy;
        int32_t newPostingsAllocCount = postingsAllocCount + extra;

        consumer->createPostings(postings, numToCopy, extra);
        BOOST_ASSERT(writer->testPoint(L"TermsHash.getPostings after create"));
        postingsAllocCount += extra;

        if (trackAllocations) {
            docWriter->bytesAllocated(extra * bytesPerPosting);
        }

        if (newPostingsAllocCount > postingsFreeList.size()) {
            // Pre-allocate the postingsFreeList so it's large enough to hold all postings we've given out
            postingsFreeList = Collection<RawPostingListPtr>::newInstance(MiscUtils::getNextSize(newPostingsAllocCount));
        }
    }

    postingsFreeCount -= numToCopy;

    if (trackAllocations) {
        docWriter->bytesUsed(postings.size() * bytesPerPosting);
    }
}

}
