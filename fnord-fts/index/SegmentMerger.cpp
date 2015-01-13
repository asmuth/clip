/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "SegmentMerger.h"
#include "MergePolicy.h"
#include "IndexWriter.h"
#include "IndexOutput.h"
#include "FieldInfos.h"
#include "FieldInfo.h"
#include "FieldsReader.h"
#include "FieldsWriter.h"
#include "IndexFileNames.h"
#include "CompoundFileWriter.h"
#include "SegmentReader.h"
#include "_SegmentReader.h"
#include "Directory.h"
#include "TermPositions.h"
#include "TermVectorsReader.h"
#include "TermVectorsWriter.h"
#include "FormatPostingsDocsConsumer.h"
#include "FormatPostingsFieldsWriter.h"
#include "FormatPostingsPositionsConsumer.h"
#include "FormatPostingsTermsConsumer.h"
#include "SegmentMergeInfo.h"
#include "SegmentMergeQueue.h"
#include "SegmentWriteState.h"
#include "TestPoint.h"
#include "MiscUtils.h"
#include "StringUtils.h"

namespace Lucene {

/// Maximum number of contiguous documents to bulk-copy when merging stored fields
const int32_t SegmentMerger::MAX_RAW_MERGE_DOCS = 4192;

/// norms header placeholder
const uint8_t SegmentMerger::NORMS_HEADER[] = {'N', 'R', 'M', static_cast<uint8_t>(-1) };
const int32_t SegmentMerger::NORMS_HEADER_LENGTH = 4;

SegmentMerger::SegmentMerger(const DirectoryPtr& dir, const String& name) {
    readers = Collection<IndexReaderPtr>::newInstance();
    termIndexInterval = IndexWriter::DEFAULT_TERM_INDEX_INTERVAL;
    mergedDocs = 0;
    mergeDocStores = false;
    omitTermFreqAndPositions = false;

    directory = dir;
    segment = name;
    checkAbort = newLucene<CheckAbortNull>();
}

SegmentMerger::SegmentMerger(const IndexWriterPtr& writer, const String& name, const OneMergePtr& merge) {
    readers = Collection<IndexReaderPtr>::newInstance();
    mergedDocs = 0;
    mergeDocStores = false;
    omitTermFreqAndPositions = false;

    directory = writer->getDirectory();
    segment = name;

    if (merge) {
        checkAbort = newLucene<CheckAbort>(merge, directory);
    } else {
        checkAbort = newLucene<CheckAbortNull>();
    }
    termIndexInterval = writer->getTermIndexInterval();
}

SegmentMerger::~SegmentMerger() {
}

bool SegmentMerger::hasProx() {
    return fieldInfos->hasProx();
}

void SegmentMerger::add(const IndexReaderPtr& reader) {
    readers.add(reader);
}

IndexReaderPtr SegmentMerger::segmentReader(int32_t i) {
    return readers[i];
}

int32_t SegmentMerger::merge() {
    return merge(true);
}

int32_t SegmentMerger::merge(bool mergeDocStores) {
    this->mergeDocStores = mergeDocStores;

    // NOTE: it's important to add calls to checkAbort.work(...) if you make any changes to this method that will spend a lot of time.
    // The frequency of this check impacts how long IndexWriter.close(false) takes to actually stop the threads.

    mergedDocs = mergeFields();
    mergeTerms();
    mergeNorms();

    if (mergeDocStores && fieldInfos->hasVectors()) {
        mergeVectors();
    }

    return mergedDocs;
}

void SegmentMerger::closeReaders() {
    for (Collection<IndexReaderPtr>::iterator reader = readers.begin(); reader != readers.end(); ++reader) {
        (*reader)->close();
    }
}

HashSet<String> SegmentMerger::getMergedFiles() {
    HashSet<String> fileSet(HashSet<String>::newInstance());

    // Basic files
    for (HashSet<String>::iterator ext = IndexFileNames::COMPOUND_EXTENSIONS().begin(); ext != IndexFileNames::COMPOUND_EXTENSIONS().end(); ++ext) {
        if (*ext == IndexFileNames::PROX_EXTENSION() && !hasProx()) {
            continue;
        }

        if (mergeDocStores || (*ext != IndexFileNames::FIELDS_EXTENSION() && *ext != IndexFileNames::FIELDS_INDEX_EXTENSION())) {
            fileSet.add(segment + L"." + *ext);
        }
    }

    // Fieldable norm files
    for (int32_t i = 0; i < fieldInfos->size(); ++i) {
        FieldInfoPtr fi(fieldInfos->fieldInfo(i));
        if (fi->isIndexed && !fi->omitNorms) {
            fileSet.add(segment + L"." + IndexFileNames::NORMS_EXTENSION());
            break;
        }
    }

    // Vector files
    if (fieldInfos->hasVectors() && mergeDocStores) {
        for (HashSet<String>::iterator ext = IndexFileNames::VECTOR_EXTENSIONS().begin(); ext != IndexFileNames::VECTOR_EXTENSIONS().end(); ++ext) {
            fileSet.add(segment + L"." + *ext);
        }
    }

    return fileSet;
}

HashSet<String> SegmentMerger::createCompoundFile(const String& fileName) {
    HashSet<String> files(getMergedFiles());
    CompoundFileWriterPtr cfsWriter(newLucene<CompoundFileWriter>(directory, fileName, checkAbort));

    // Now merge all added files
    for (HashSet<String>::iterator file = files.begin(); file != files.end(); ++file) {
        cfsWriter->addFile(*file);
    }

    // Perform the merge
    cfsWriter->close();

    return files;
}

void SegmentMerger::addIndexed(const IndexReaderPtr& reader, const FieldInfosPtr& fInfos, HashSet<String> names,
                               bool storeTermVectors, bool storePositionWithTermVector,
                               bool storeOffsetWithTermVector, bool storePayloads, bool omitTFAndPositions) {
    for (HashSet<String>::iterator field = names.begin(); field != names.end(); ++field) {
        fInfos->add(*field, true, storeTermVectors, storePositionWithTermVector, storeOffsetWithTermVector,
                    !reader->hasNorms(*field), storePayloads, omitTFAndPositions);
    }
}

void SegmentMerger::setMatchingSegmentReaders() {
    // If the i'th reader is a SegmentReader and has identical fieldName -> number mapping, then
    // this array will be non-null at position i
    int32_t numReaders = readers.size();
    matchingSegmentReaders = Collection<SegmentReaderPtr>::newInstance(numReaders);

    // If this reader is a SegmentReader, and all of its field name -> number mappings match the
    // "merged" FieldInfos, then we can do a bulk copy of the stored fields
    for (int32_t i = 0; i < numReaders; ++i) {
        IndexReaderPtr reader(readers[i]);
        SegmentReaderPtr segmentReader(boost::dynamic_pointer_cast<SegmentReader>(reader));
        if (segmentReader) {
            bool same = true;
            FieldInfosPtr segmentFieldInfos(segmentReader->fieldInfos());
            int32_t numFieldInfos = segmentFieldInfos->size();
            for (int32_t j = 0; same && j < numFieldInfos; ++j) {
                same = (fieldInfos->fieldName(j) == segmentFieldInfos->fieldName(j));
            }
            if (same) {
                matchingSegmentReaders[i] = segmentReader;
            }
        }
    }

    // Used for bulk-reading raw bytes for stored fields
    rawDocLengths = Collection<int32_t>::newInstance(MAX_RAW_MERGE_DOCS);
    rawDocLengths2 = Collection<int32_t>::newInstance(MAX_RAW_MERGE_DOCS);
}

int32_t SegmentMerger::mergeFields() {
    if (!mergeDocStores) {
        // When we are not merging by doc stores, their field name -> number mapping are the same.
        // So, we start with the fieldInfos of the last segment in this case, to keep that numbering
        fieldInfos = boost::dynamic_pointer_cast<FieldInfos>(boost::dynamic_pointer_cast<SegmentReader>(readers[readers.size() - 1])->core->fieldInfos->clone());
    } else {
        fieldInfos = newLucene<FieldInfos>();    // merge field names
    }

    for (Collection<IndexReaderPtr>::iterator reader = readers.begin(); reader != readers.end(); ++reader) {
        SegmentReaderPtr segmentReader(boost::dynamic_pointer_cast<SegmentReader>(*reader));
        if (segmentReader) {
            FieldInfosPtr readerFieldInfos(segmentReader->fieldInfos());
            int32_t numReaderFieldInfos = readerFieldInfos->size();
            for (int32_t j = 0; j < numReaderFieldInfos; ++j) {
                FieldInfoPtr fi(readerFieldInfos->fieldInfo(j));
                fieldInfos->add(fi->name, fi->isIndexed, fi->storeTermVector, fi->storePositionWithTermVector,
                                fi->storeOffsetWithTermVector, !(*reader)->hasNorms(fi->name), fi->storePayloads,
                                fi->omitTermFreqAndPositions);
            }
        } else {
            addIndexed(*reader, fieldInfos, (*reader)->getFieldNames(IndexReader::FIELD_OPTION_TERMVECTOR_WITH_POSITION_OFFSET), true, true, true, false, false);
            addIndexed(*reader, fieldInfos, (*reader)->getFieldNames(IndexReader::FIELD_OPTION_TERMVECTOR_WITH_POSITION), true, true, false, false, false);
            addIndexed(*reader, fieldInfos, (*reader)->getFieldNames(IndexReader::FIELD_OPTION_TERMVECTOR_WITH_OFFSET), true, false, true, false, false);
            addIndexed(*reader, fieldInfos, (*reader)->getFieldNames(IndexReader::FIELD_OPTION_TERMVECTOR), true, false, false, false, false);
            addIndexed(*reader, fieldInfos, (*reader)->getFieldNames(IndexReader::FIELD_OPTION_OMIT_TERM_FREQ_AND_POSITIONS), false, false, false, false, true);
            addIndexed(*reader, fieldInfos, (*reader)->getFieldNames(IndexReader::FIELD_OPTION_STORES_PAYLOADS), false, false, false, true, false);
            addIndexed(*reader, fieldInfos, (*reader)->getFieldNames(IndexReader::FIELD_OPTION_INDEXED), false, false, false, false, false);
            fieldInfos->add((*reader)->getFieldNames(IndexReader::FIELD_OPTION_UNINDEXED), false);
        }
    }
    fieldInfos->write(directory, segment + L".fnm");

    int32_t docCount = 0;

    setMatchingSegmentReaders();

    if (mergeDocStores) {
        // merge field values
        FieldsWriterPtr fieldsWriter(newLucene<FieldsWriter>(directory, segment, fieldInfos));

        LuceneException finally;
        try {
            int32_t idx = 0;
            for (Collection<IndexReaderPtr>::iterator reader = readers.begin(); reader != readers.end(); ++reader) {
                SegmentReaderPtr matchingSegmentReader(matchingSegmentReaders[idx++]);
                FieldsReaderPtr matchingFieldsReader;
                if (matchingSegmentReader) {
                    FieldsReaderPtr fieldsReader(matchingSegmentReader->getFieldsReader());
                    if (fieldsReader && fieldsReader->canReadRawDocs()) {
                        matchingFieldsReader = fieldsReader;
                    }
                }
                if ((*reader)->hasDeletions()) {
                    docCount += copyFieldsWithDeletions(fieldsWriter, *reader, matchingFieldsReader);
                } else {
                    docCount += copyFieldsNoDeletions(fieldsWriter, *reader, matchingFieldsReader);
                }
            }
        } catch (LuceneException& e) {
            finally = e;
        }
        fieldsWriter->close();
        finally.throwException();

        String fileName(segment + L"." + IndexFileNames::FIELDS_INDEX_EXTENSION());
        int64_t fdxFileLength = directory->fileLength(fileName);

        if (4 + ((int64_t)docCount) * 8 != fdxFileLength) {
            boost::throw_exception(RuntimeException(L"mergeFields produced an invalid result: docCount is " +
                                                    StringUtils::toString(docCount) + L" but fdx file size is " +
                                                    StringUtils::toString(fdxFileLength) + L" file=" + fileName +
                                                    L" file exists?=" + StringUtils::toString(directory->fileExists(fileName)) +
                                                    L"; now aborting this merge to prevent index corruption"));
        }
    } else {
        // If we are skipping the doc stores, that means there are no deletions in any of these segments,
        // so we just sum numDocs() of each segment to get total docCount
        for (Collection<IndexReaderPtr>::iterator reader = readers.begin(); reader != readers.end(); ++reader) {
            docCount += (*reader)->numDocs();
        }
    }

    return docCount;
}

int32_t SegmentMerger::copyFieldsWithDeletions(const FieldsWriterPtr& fieldsWriter, const IndexReaderPtr& reader, const FieldsReaderPtr& matchingFieldsReader) {
    int32_t docCount = 0;
    int32_t maxDoc = reader->maxDoc();
    if (matchingFieldsReader) {
        // We can bulk-copy because the fieldInfos are "congruent"
        for (int32_t j = 0; j < maxDoc;) {
            if (reader->isDeleted(j)) {
                // skip deleted docs
                ++j;
                continue;
            }
            // We can optimize this case (doing a bulk byte copy) since the field numbers are identical
            int32_t start = j;
            int32_t numDocs = 0;
            do {
                ++j;
                ++numDocs;
                if (j >= maxDoc) {
                    break;
                }
                if (reader->isDeleted(j)) {
                    ++j;
                    break;
                }
            } while (numDocs < MAX_RAW_MERGE_DOCS);

            IndexInputPtr stream(matchingFieldsReader->rawDocs(rawDocLengths, start, numDocs));
            fieldsWriter->addRawDocuments(stream, rawDocLengths, numDocs);
            docCount += numDocs;
            checkAbort->work(300 * numDocs);
        }
    } else {
        for (int32_t j = 0; j < maxDoc; ++j) {
            if (reader->isDeleted(j)) {
                // skip deleted docs
                continue;
            }

            // NOTE: it's very important to first assign to doc then pass it to termVectorsWriter.addAllDocVectors
            fieldsWriter->addDocument(reader->document(j));
            ++docCount;
            checkAbort->work(300);
        }
    }
    return docCount;
}

int32_t SegmentMerger::copyFieldsNoDeletions(const FieldsWriterPtr& fieldsWriter, const IndexReaderPtr& reader, const FieldsReaderPtr& matchingFieldsReader) {
    int32_t docCount = 0;
    int32_t maxDoc = reader->maxDoc();
    if (matchingFieldsReader) {
        // We can bulk-copy because the fieldInfos are "congruent"
        while (docCount < maxDoc) {
            int32_t len = std::min(MAX_RAW_MERGE_DOCS, maxDoc - docCount);
            IndexInputPtr stream(matchingFieldsReader->rawDocs(rawDocLengths, docCount, len));
            fieldsWriter->addRawDocuments(stream, rawDocLengths, len);
            docCount += len;
            checkAbort->work(300 * len);
        }
    } else {
        for (; docCount < maxDoc; ++docCount) {
            // NOTE: it's very important to first assign to doc then pass it to termVectorsWriter.addAllDocVectors
            fieldsWriter->addDocument(reader->document(docCount));
            checkAbort->work(300);
        }
    }
    return docCount;
}

void SegmentMerger::mergeVectors() {
    TermVectorsWriterPtr termVectorsWriter(newLucene<TermVectorsWriter>(directory, segment, fieldInfos));

    LuceneException finally;
    try {
        int32_t idx = 0;
        for (Collection<IndexReaderPtr>::iterator reader = readers.begin(); reader != readers.end(); ++reader) {
            SegmentReaderPtr matchingSegmentReader(matchingSegmentReaders[idx++]);
            TermVectorsReaderPtr matchingVectorsReader;
            if (matchingSegmentReader) {
                TermVectorsReaderPtr vectorsReader(matchingSegmentReader->getTermVectorsReaderOrig());

                // If the TV* files are an older format then they cannot read raw docs
                if (vectorsReader && vectorsReader->canReadRawDocs()) {
                    matchingVectorsReader = vectorsReader;
                }
            }
            if ((*reader)->hasDeletions()) {
                copyVectorsWithDeletions(termVectorsWriter, matchingVectorsReader, *reader);
            } else {
                copyVectorsNoDeletions(termVectorsWriter, matchingVectorsReader, *reader);
            }
        }
    } catch (LuceneException& e) {
        finally = e;
    }
    termVectorsWriter->close();
    finally.throwException();

    String fileName(segment + L"." + IndexFileNames::VECTORS_INDEX_EXTENSION());
    int64_t tvxSize = directory->fileLength(fileName);

    if (4 + ((int64_t)mergedDocs) * 16 != tvxSize) {
        boost::throw_exception(RuntimeException(L"mergeVectors produced an invalid result: mergedDocs is " +
                                                StringUtils::toString(mergedDocs) + L" but tvx size is " +
                                                StringUtils::toString(tvxSize) + L" file=" + fileName +
                                                L" file exists?=" + StringUtils::toString(directory->fileExists(fileName)) +
                                                L"; now aborting this merge to prevent index corruption"));
    }
}

void SegmentMerger::copyVectorsWithDeletions(const TermVectorsWriterPtr& termVectorsWriter, const TermVectorsReaderPtr& matchingVectorsReader, const IndexReaderPtr& reader) {
    int32_t maxDoc = reader->maxDoc();
    if (matchingVectorsReader) {
        // We can bulk-copy because the fieldInfos are "congruent"
        for (int32_t docNum = 0; docNum < maxDoc;) {
            if (reader->isDeleted(docNum)) {
                // skip deleted docs
                ++docNum;
                continue;
            }
            // We can optimize this case (doing a bulk byte copy) since the field numbers are identical
            int32_t start = docNum;
            int32_t numDocs = 0;
            do {
                ++docNum;
                ++numDocs;
                if (docNum >= maxDoc) {
                    break;
                }
                if (reader->isDeleted(docNum)) {
                    ++docNum;
                    break;
                }
            } while (numDocs < MAX_RAW_MERGE_DOCS);

            matchingVectorsReader->rawDocs(rawDocLengths, rawDocLengths2, start, numDocs);
            termVectorsWriter->addRawDocuments(matchingVectorsReader, rawDocLengths, rawDocLengths2, numDocs);
            checkAbort->work(300 * numDocs);
        }
    } else {
        for (int32_t docNum = 0; docNum < maxDoc; ++docNum) {
            if (reader->isDeleted(docNum)) {
                // skip deleted docs
                continue;
            }

            // NOTE: it's very important to first assign to vectors then pass it to termVectorsWriter.addAllDocVectors
            termVectorsWriter->addAllDocVectors(reader->getTermFreqVectors(docNum));
            checkAbort->work(300);
        }
    }
}

void SegmentMerger::copyVectorsNoDeletions(const TermVectorsWriterPtr& termVectorsWriter, const TermVectorsReaderPtr& matchingVectorsReader, const IndexReaderPtr& reader) {
    int32_t maxDoc = reader->maxDoc();
    if (matchingVectorsReader) {
        // We can bulk-copy because the fieldInfos are "congruent"
        int32_t docCount = 0;
        while (docCount < maxDoc) {
            int32_t len = std::min(MAX_RAW_MERGE_DOCS, maxDoc - docCount);
            matchingVectorsReader->rawDocs(rawDocLengths, rawDocLengths2, docCount, len);
            termVectorsWriter->addRawDocuments(matchingVectorsReader, rawDocLengths, rawDocLengths2, len);
            docCount += len;
            checkAbort->work(300 * len);
        }
    } else {
        for (int32_t docNum = 0; docNum < maxDoc; ++docNum) {
            // NOTE: it's very important to first assign to vectors then pass it to termVectorsWriter.addAllDocVectors
            termVectorsWriter->addAllDocVectors(reader->getTermFreqVectors(docNum));
            checkAbort->work(300);
        }
    }
}

void SegmentMerger::mergeTerms() {
    TestScope testScope(L"SegmentMerger", L"mergeTerms");

    SegmentWriteStatePtr state(newLucene<SegmentWriteState>(DocumentsWriterPtr(), directory, segment, L"", mergedDocs, 0, termIndexInterval));

    FormatPostingsFieldsConsumerPtr consumer(newLucene<FormatPostingsFieldsWriter>(state, fieldInfos));

    LuceneException finally;
    try {
        queue = newLucene<SegmentMergeQueue>(readers.size());
        mergeTermInfos(consumer);
    } catch (LuceneException& e) {
        finally = e;
    }
    consumer->finish();
    if (queue) {
        queue->close();
    }
    finally.throwException();
}

void SegmentMerger::mergeTermInfos(const FormatPostingsFieldsConsumerPtr& consumer) {
    int32_t base = 0;
    int32_t readerCount = readers.size();
    for (int32_t i = 0; i < readerCount; ++i) {
        IndexReaderPtr reader(readers[i]);
        TermEnumPtr termEnum(reader->terms());
        SegmentMergeInfoPtr smi(newLucene<SegmentMergeInfo>(base, termEnum, reader));
        Collection<int32_t> docMap(smi->getDocMap());
        if (docMap) {
            if (!docMaps) {
                docMaps = Collection< Collection<int32_t> >::newInstance(readerCount);
                delCounts = Collection<int32_t>::newInstance(readerCount);
            }
            docMaps[i] = docMap;
            IndexReaderPtr segmentMergeReader(smi->_reader);
            delCounts[i] = segmentMergeReader->maxDoc() - segmentMergeReader->numDocs();
        }

        base += reader->numDocs();

        BOOST_ASSERT(reader->numDocs() == reader->maxDoc() - smi->delCount);

        if (smi->next()) {
            queue->add(smi);    // initialize queue
        } else {
            smi->close();
        }
    }

    Collection<SegmentMergeInfoPtr> match(Collection<SegmentMergeInfoPtr>::newInstance(readers.size()));

    String currentField;
    FormatPostingsTermsConsumerPtr termsConsumer;

    while (!queue->empty()) {
        int32_t matchSize = 0; // pop matching terms
        match[matchSize++] = queue->pop();
        TermPtr term(match[0]->term);
        SegmentMergeInfoPtr top(queue->empty() ? SegmentMergeInfoPtr() : queue->top());

        while (top && term->compareTo(top->term) == 0) {
            match[matchSize++] = queue->pop();
            top = queue->top();
        }

        if (currentField != term->_field) {
            currentField = term->_field;
            if (termsConsumer) {
                termsConsumer->finish();
            }
            FieldInfoPtr fieldInfo(fieldInfos->fieldInfo(currentField));
            termsConsumer = consumer->addField(fieldInfo);
            omitTermFreqAndPositions = fieldInfo->omitTermFreqAndPositions;
        }

        int32_t df = appendPostings(termsConsumer, match, matchSize); // add new TermInfo

        checkAbort->work(df / 3.0);

        while (matchSize > 0) {
            SegmentMergeInfoPtr smi(match[--matchSize]);
            if (smi->next()) {
                queue->add(smi);    // restore queue
            } else {
                smi->close();    // done with a segment
            }
        }
    }
}

Collection< Collection<int32_t> > SegmentMerger::getDocMaps() {
    return docMaps;
}

Collection<int32_t> SegmentMerger::getDelCounts() {
    return delCounts;
}

int32_t SegmentMerger::appendPostings(const FormatPostingsTermsConsumerPtr& termsConsumer, Collection<SegmentMergeInfoPtr> smis, int32_t n) {
    FormatPostingsDocsConsumerPtr docConsumer(termsConsumer->addTerm(smis[0]->term->_text));
    int32_t df = 0;
    for (int32_t i = 0; i < n; ++i) {
        SegmentMergeInfoPtr smi(smis[i]);
        TermPositionsPtr postings(smi->getPositions());
        BOOST_ASSERT(postings);
        int32_t base = smi->base;
        Collection<int32_t> docMap(smi->getDocMap());
        postings->seek(smi->termEnum);

        while (postings->next()) {
            ++df;
            int32_t doc = postings->doc();
            if (docMap) {
                doc = docMap[doc];    // map around deletions
            }
            doc += base; // convert to merged space

            int32_t freq = postings->freq();
            FormatPostingsPositionsConsumerPtr posConsumer(docConsumer->addDoc(doc, freq));

            if (!omitTermFreqAndPositions) {
                for (int32_t j = 0; j < freq; ++j) {
                    int32_t position = postings->nextPosition();
                    int32_t payloadLength = postings->getPayloadLength();
                    if (payloadLength > 0) {
                        if (!payloadBuffer) {
                            payloadBuffer = ByteArray::newInstance(payloadLength);
                        }
                        if (payloadBuffer.size() < payloadLength) {
                            payloadBuffer.resize(payloadLength);
                        }
                        postings->getPayload(payloadBuffer, 0);
                    }
                    posConsumer->addPosition(position, payloadBuffer, 0, payloadLength);
                }
                posConsumer->finish();
            }
        }
    }
    docConsumer->finish();

    return df;
}

void SegmentMerger::mergeNorms() {
    ByteArray normBuffer;
    IndexOutputPtr output;
    LuceneException finally;
    try {
        int32_t numFieldInfos = fieldInfos->size();
        for (int32_t i = 0; i < numFieldInfos; ++i) {
            FieldInfoPtr fi(fieldInfos->fieldInfo(i));
            if (fi->isIndexed && !fi->omitNorms) {
                if (!output) {
                    output = directory->createOutput(segment + L"." + IndexFileNames::NORMS_EXTENSION());
                    output->writeBytes(NORMS_HEADER, SIZEOF_ARRAY(NORMS_HEADER));
                }
                for (Collection<IndexReaderPtr>::iterator reader = readers.begin(); reader != readers.end(); ++reader) {
                    int32_t maxDoc = (*reader)->maxDoc();

                    if (!normBuffer) {
                        normBuffer = ByteArray::newInstance(maxDoc);
                    }
                    if (normBuffer.size() < maxDoc) { // the buffer is too small for the current segment
                        normBuffer.resize(maxDoc);
                    }
                    MiscUtils::arrayFill(normBuffer.get(), 0, normBuffer.size(), 0);
                    (*reader)->norms(fi->name, normBuffer, 0);
                    if (!(*reader)->hasDeletions()) {
                        // optimized case for segments without deleted docs
                        output->writeBytes(normBuffer.get(), maxDoc);
                    } else {
                        // this segment has deleted docs, so we have to check for every doc if it is deleted or not
                        for (int32_t k = 0; k < maxDoc; ++k) {
                            if (!(*reader)->isDeleted(k)) {
                                output->writeByte(normBuffer[k]);
                            }
                        }
                    }
                    checkAbort->work(maxDoc);
                }
            }
        }
    } catch (LuceneException& e) {
        finally = e;
    }
    if (output) {
        output->close();
    }
    finally.throwException();
}

CheckAbort::CheckAbort(const OneMergePtr& merge, const DirectoryPtr& dir) {
    workCount = 0;
    this->merge = merge;
    this->_dir = dir;
}

CheckAbort::~CheckAbort() {
}

void CheckAbort::work(double units) {
    workCount += units;
    if (workCount >= 10000.0) {
        merge->checkAborted(DirectoryPtr(_dir));
        workCount = 0;
    }
}

CheckAbortNull::CheckAbortNull() : CheckAbort(OneMergePtr(), DirectoryPtr()) {
}

CheckAbortNull::~CheckAbortNull() {
}

void CheckAbortNull::work(double units) {
    // do nothing
}

}
