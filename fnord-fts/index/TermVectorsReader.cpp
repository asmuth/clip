/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "TermVectorsReader.h"
#include "BufferedIndexInput.h"
#include "IndexFileNames.h"
#include "Directory.h"
#include "FieldInfos.h"
#include "SegmentTermPositionVector.h"
#include "TermVectorOffsetInfo.h"
#include "MiscUtils.h"
#include "StringUtils.h"

namespace Lucene {

/// NOTE: if you make a new format, it must be larger than the current format
const int32_t TermVectorsReader::FORMAT_VERSION = 2;

/// Changes to speed up bulk merging of term vectors
const int32_t TermVectorsReader::FORMAT_VERSION2 = 3;

/// Changed strings to UTF8 with length-in-bytes not length-in-chars
const int32_t TermVectorsReader::FORMAT_UTF8_LENGTH_IN_BYTES = 4;

/// NOTE: always change this if you switch to a new format
const int32_t TermVectorsReader::FORMAT_CURRENT = TermVectorsReader::FORMAT_UTF8_LENGTH_IN_BYTES;

/// The size in bytes that the FORMAT_VERSION will take up at the beginning of each file
const int32_t TermVectorsReader::FORMAT_SIZE = 4;

const uint8_t TermVectorsReader::STORE_POSITIONS_WITH_TERMVECTOR = 0x1;
const uint8_t TermVectorsReader::STORE_OFFSET_WITH_TERMVECTOR = 0x2;

TermVectorsReader::TermVectorsReader() {
    this->_size = 0;
    this->numTotalDocs = 0;
    this->docStoreOffset = 0;
    this->format = 0;
}

TermVectorsReader::TermVectorsReader(const DirectoryPtr& d, const String& segment, const FieldInfosPtr& fieldInfos) {
    ConstructReader(d, segment, fieldInfos, BufferedIndexInput::BUFFER_SIZE, -1, 0);
}

TermVectorsReader::TermVectorsReader(const DirectoryPtr& d, const String& segment, const FieldInfosPtr& fieldInfos, int32_t readBufferSize, int32_t docStoreOffset, int32_t size) {
    ConstructReader(d, segment, fieldInfos, readBufferSize, docStoreOffset, size);
}

TermVectorsReader::~TermVectorsReader() {
}

void TermVectorsReader::ConstructReader(const DirectoryPtr& d, const String& segment, const FieldInfosPtr& fieldInfos, int32_t readBufferSize, int32_t docStoreOffset, int32_t size) {
    this->_size = 0;
    this->numTotalDocs = 0;
    this->docStoreOffset = 0;
    this->format = 0;

    bool success = false;
    LuceneException finally;
    try {
        if (d->fileExists(segment + L"." + IndexFileNames::VECTORS_INDEX_EXTENSION())) {
            tvx = d->openInput(segment + L"." + IndexFileNames::VECTORS_INDEX_EXTENSION(), readBufferSize);
            format = checkValidFormat(tvx);
            tvd = d->openInput(segment + L"." + IndexFileNames::VECTORS_DOCUMENTS_EXTENSION(), readBufferSize);
            int32_t tvdFormat = checkValidFormat(tvd);
            tvf = d->openInput(segment + L"." + IndexFileNames::VECTORS_FIELDS_EXTENSION(), readBufferSize);
            int32_t tvfFormat = checkValidFormat(tvf);

            BOOST_ASSERT(format == tvdFormat);
            BOOST_ASSERT(format == tvfFormat);

            if (format >= FORMAT_VERSION2) {
                BOOST_ASSERT((tvx->length() - FORMAT_SIZE) % 16 == 0);
                numTotalDocs = (int32_t)(tvx->length() >> 4);
            } else {
                BOOST_ASSERT((tvx->length() - FORMAT_SIZE) % 8 == 0);
                numTotalDocs = (int32_t)(tvx->length() >> 3);
            }

            if (docStoreOffset == -1) {
                this->docStoreOffset = 0;
                this->_size = numTotalDocs;
                BOOST_ASSERT(size == 0 || numTotalDocs == size);
            } else {
                this->docStoreOffset = docStoreOffset;
                this->_size = size;
                // Verify the file is long enough to hold all of our docs
                BOOST_ASSERT(numTotalDocs >= size + docStoreOffset);
            }
        } else {
            // If all documents flushed in a segment had hit non-aborting exceptions, it's possible that
            // FieldInfos.hasVectors returns true yet the term vector files don't exist.
            format = 0;
        }

        this->fieldInfos = fieldInfos;
        success = true;
    } catch (LuceneException& e) {
        finally = e;
    }

    // With lock-less commits, it's entirely possible (and fine) to hit a FileNotFound exception
    // above. In this case, we want to explicitly close any subset of things that were opened.
    if (!success) {
        close();
    }
    finally.throwException();
}

IndexInputPtr TermVectorsReader::getTvdStream() {
    return tvd;
}

IndexInputPtr TermVectorsReader::getTvfStream() {
    return tvf;
}

void TermVectorsReader::seekTvx(int32_t docNum) {
    if (format < FORMAT_VERSION2) {
        tvx->seek((docNum + docStoreOffset) * 8 + FORMAT_SIZE);
    } else {
        tvx->seek((docNum + docStoreOffset) * 16 + FORMAT_SIZE);
    }
}

bool TermVectorsReader::canReadRawDocs() {
    return (format >= FORMAT_UTF8_LENGTH_IN_BYTES);
}

void TermVectorsReader::rawDocs(Collection<int32_t> tvdLengths, Collection<int32_t> tvfLengths, int32_t startDocID, int32_t numDocs) {
    if (!tvx) {
        MiscUtils::arrayFill(tvdLengths.begin(), 0, tvdLengths.size(), 0);
        MiscUtils::arrayFill(tvfLengths.begin(), 0, tvfLengths.size(), 0);
        return;
    }

    // SegmentMerger calls canReadRawDocs() first and should not call us if that returns false.
    if (format < FORMAT_VERSION2) {
        boost::throw_exception(IllegalStateException(L"cannot read raw docs with older term vector formats"));
    }

    seekTvx(startDocID);

    int64_t tvdPosition = tvx->readLong();
    tvd->seek(tvdPosition);

    int64_t tvfPosition = tvx->readLong();
    tvf->seek(tvfPosition);

    int64_t lastTvdPosition = tvdPosition;
    int64_t lastTvfPosition = tvfPosition;

    int32_t count = 0;
    while (count < numDocs) {
        int32_t docID = docStoreOffset + startDocID + count + 1;
        BOOST_ASSERT(docID <= numTotalDocs);
        if (docID < numTotalDocs) {
            tvdPosition = tvx->readLong();
            tvfPosition = tvx->readLong();
        } else {
            tvdPosition = tvd->length();
            tvfPosition = tvf->length();
            BOOST_ASSERT(count == numDocs - 1);
        }
        tvdLengths[count] = (int32_t)(tvdPosition - lastTvdPosition);
        tvfLengths[count] = (int32_t)(tvfPosition - lastTvfPosition);
        ++count;
        lastTvdPosition = tvdPosition;
        lastTvfPosition = tvfPosition;
    }
}

int32_t TermVectorsReader::checkValidFormat(const IndexInputPtr& in) {
    int32_t format = in->readInt();
    if (format > FORMAT_CURRENT) {
        boost::throw_exception(CorruptIndexException(L"Incompatible format version: " +
                               StringUtils::toString(format) + L" expected " +
                               StringUtils::toString(FORMAT_CURRENT) + L" or less"));
    }
    return format;
}

void TermVectorsReader::close() {
    // make all effort to close up. Keep the first exception and throw it as a new one.
    LuceneException keep;
    if (tvx) {
        try {
            tvx->close();
        } catch (LuceneException& e) {
            if (keep.isNull()) {
                keep = e;
            }
        }
    }
    if (tvd) {
        try {
            tvd->close();
        } catch (LuceneException& e) {
            if (keep.isNull()) {
                keep = e;
            }
        }
    }
    if (tvf) {
        try {
            tvf->close();
        } catch (LuceneException& e) {
            if (keep.isNull()) {
                keep = e;
            }
        }
    }
    keep.throwException();
}

int32_t TermVectorsReader::size() {
    return _size;
}

void TermVectorsReader::get(int32_t docNum, const String& field, const TermVectorMapperPtr& mapper) {
    if (tvx) {
        int32_t fieldNumber = fieldInfos->fieldNumber(field);

        // We need to account for the FORMAT_SIZE at when seeking in the tvx.  We don't need to do
        // this in other seeks because we already have the file pointer that was written in another file
        seekTvx(docNum);
        int64_t tvdPosition = tvx->readLong();

        tvd->seek(tvdPosition);
        int32_t fieldCount = tvd->readVInt();

        // There are only a few fields per document. We opt for a full scan rather then requiring that they
        // be ordered. We need to read through all of the fields anyway to get to the tvf pointers.
        int32_t number = 0;
        int32_t found = -1;
        for (int32_t i = 0; i < fieldCount; ++i) {
            if (format >= FORMAT_VERSION) {
                number = tvd->readVInt();
            } else {
                number += tvd->readVInt();
            }

            if (number == fieldNumber) {
                found = i;
            }
        }

        // This field, although valid in the segment, was not found in this document
        if (found != -1) {
            // Compute position in the tvf file
            int64_t position;
            if (format >= FORMAT_VERSION2) {
                position = tvx->readLong();
            } else {
                position = tvd->readVLong();
            }
            for (int32_t i = 1; i <= found; ++i) {
                position += tvd->readVLong();
            }

            mapper->setDocumentNumber(docNum);
            readTermVector(field, position, mapper);
        }
    }
}

TermFreqVectorPtr TermVectorsReader::get(int32_t docNum, const String& field) {
    // Check if no term vectors are available for this segment at all
    ParallelArrayTermVectorMapperPtr mapper(newLucene<ParallelArrayTermVectorMapper>());
    get(docNum, field, mapper);
    return mapper->materializeVector();
}

Collection<String> TermVectorsReader::readFields(int32_t fieldCount) {
    int32_t number = 0;
    Collection<String> fields(Collection<String>::newInstance(fieldCount));

    for (int32_t i = 0; i < fieldCount; ++i) {
        if (format >= FORMAT_VERSION) {
            number = tvd->readVInt();
        } else {
            number += tvd->readVInt();
        }
        fields[i] = fieldInfos->fieldName(number);
    }

    return fields;
}

Collection<int64_t> TermVectorsReader::readTvfPointers(int32_t fieldCount) {
    // Compute position in the tvf file
    int64_t position;
    if (format >= FORMAT_VERSION2) {
        position = tvx->readLong();
    } else {
        position = tvd->readVLong();
    }

    Collection<int64_t> tvfPointers(Collection<int64_t>::newInstance(fieldCount));
    tvfPointers[0] = position;

    for (int32_t i = 1; i < fieldCount; ++i) {
        position += tvd->readVLong();
        tvfPointers[i] = position;
    }

    return tvfPointers;
}

Collection<TermFreqVectorPtr> TermVectorsReader::get(int32_t docNum) {
    Collection<TermFreqVectorPtr> result;
    if (tvx) {
        // We need to offset by
        seekTvx(docNum);
        int64_t tvdPosition = tvx->readLong();

        tvd->seek(tvdPosition);
        int32_t fieldCount = tvd->readVInt();

        // No fields are vectorized for this document
        if (fieldCount != 0) {
            Collection<String> fields(readFields(fieldCount));
            Collection<int64_t> tvfPointers(readTvfPointers(fieldCount));
            result = readTermVectors(docNum, fields, tvfPointers);
        }
    }
    return result;
}

void TermVectorsReader::get(int32_t docNumber, const TermVectorMapperPtr& mapper) {
    // Check if no term vectors are available for this segment at all
    if (tvx) {
        // We need to offset by
        seekTvx(docNumber);
        int64_t tvdPosition = tvx->readLong();

        tvd->seek(tvdPosition);
        int32_t fieldCount = tvd->readVInt();

        // No fields are vectorized for this document
        if (fieldCount != 0) {
            Collection<String> fields(readFields(fieldCount));
            Collection<int64_t> tvfPointers(readTvfPointers(fieldCount));
            mapper->setDocumentNumber(docNumber);
            readTermVectors(fields, tvfPointers, mapper);
        }
    }
}

Collection<TermFreqVectorPtr> TermVectorsReader::readTermVectors(int32_t docNum, Collection<String> fields, Collection<int64_t> tvfPointers) {
    Collection<TermFreqVectorPtr> res(Collection<TermFreqVectorPtr>::newInstance(fields.size()));
    for (int32_t i = 0; i < fields.size(); ++i) {
        ParallelArrayTermVectorMapperPtr mapper(newLucene<ParallelArrayTermVectorMapper>());
        mapper->setDocumentNumber(docNum);
        readTermVector(fields[i], tvfPointers[i], mapper);
        res[i] = mapper->materializeVector();
    }
    return res;
}

void TermVectorsReader::readTermVectors(Collection<String> fields, Collection<int64_t> tvfPointers, const TermVectorMapperPtr& mapper) {
    for (int32_t i = 0; i < fields.size(); ++i) {
        readTermVector(fields[i], tvfPointers[i], mapper);
    }
}

void TermVectorsReader::readTermVector(const String& field, int64_t tvfPointer, const TermVectorMapperPtr& mapper) {
    // Now read the data from specified position.  We don't need to offset by the FORMAT here since
    // the pointer already includes the offset
    tvf->seek(tvfPointer);

    int32_t numTerms = tvf->readVInt();

    // If no terms - return a constant empty termvector. However, this should never occur!
    if (numTerms == 0) {
        return;
    }

    bool storePositions;
    bool storeOffsets;

    if (format >= FORMAT_VERSION) {
        uint8_t bits = tvf->readByte();
        storePositions = ((bits & STORE_POSITIONS_WITH_TERMVECTOR) != 0);
        storeOffsets = ((bits & STORE_OFFSET_WITH_TERMVECTOR) != 0);
    } else {
        tvf->readVInt();
        storePositions = false;
        storeOffsets = false;
    }

    mapper->setExpectations(field, numTerms, storeOffsets, storePositions);
    int32_t start = 0;
    int32_t deltaLength = 0;
    int32_t totalLength = 0;
    ByteArray byteBuffer;
    CharArray charBuffer;
    bool preUTF8 = (format < FORMAT_UTF8_LENGTH_IN_BYTES);

    // init the buffers
    if (preUTF8) {
        charBuffer = CharArray::newInstance(10);
        byteBuffer.reset();
    } else {
        charBuffer.reset();
        byteBuffer = ByteArray::newInstance(20);
    }

    for (int32_t i = 0; i < numTerms; ++i) {
        start = tvf->readVInt();
        deltaLength = tvf->readVInt();
        totalLength = start + deltaLength;

        String term;

        if (preUTF8) {
            // Term stored as "java chars"
            if (charBuffer.size() < totalLength) {
                charBuffer.resize((int32_t)(1.5 * (double)totalLength));
            }
            totalLength = start + tvf->readChars(charBuffer.get(), start, deltaLength);
            term.append(charBuffer.get(), totalLength);
        } else {
            // Term stored as utf8 bytes
            if (byteBuffer.size() < totalLength) {
                byteBuffer.resize((int32_t)(1.5 * (double)totalLength));
            }
            tvf->readBytes(byteBuffer.get(), start, deltaLength);
            term = StringUtils::toUnicode(byteBuffer.get(), totalLength);
        }
        int32_t freq = tvf->readVInt();
        Collection<int32_t> positions;
        if (storePositions) { // read in the positions
            // does the mapper even care about positions?
            if (!mapper->isIgnoringPositions()) {
                positions = Collection<int32_t>::newInstance(freq);
                int32_t prevPosition = 0;
                for (Collection<int32_t>::iterator position = positions.begin(); position != positions.end(); ++position) {
                    *position = prevPosition + tvf->readVInt();
                    prevPosition = *position;
                }
            } else {
                // we need to skip over the positions.  Since these are VInts, I don't believe there
                // is anyway to know for sure how far to skip
                for (int32_t j = 0; j < freq; ++j) {
                    tvf->readVInt();
                }
            }
        }

        Collection<TermVectorOffsetInfoPtr> offsets;
        if (storeOffsets) {
            // does the mapper even care about offsets?
            if (!mapper->isIgnoringOffsets()) {
                offsets = Collection<TermVectorOffsetInfoPtr>::newInstance(freq);
                int32_t prevOffset = 0;
                for (Collection<TermVectorOffsetInfoPtr>::iterator offset = offsets.begin(); offset != offsets.end(); ++offset) {
                    int32_t startOffset = prevOffset + tvf->readVInt();
                    int32_t endOffset = startOffset + tvf->readVInt();
                    *offset = newLucene<TermVectorOffsetInfo>(startOffset, endOffset);
                    prevOffset = endOffset;
                }
            } else {
                for (int32_t j = 0; j < freq; ++j) {
                    tvf->readVInt();
                    tvf->readVInt();
                }
            }
        }
        mapper->map(term, freq, offsets, positions);
    }
}

LuceneObjectPtr TermVectorsReader::clone(const LuceneObjectPtr& other) {
    LuceneObjectPtr clone = other ? other : newLucene<TermVectorsReader>();
    TermVectorsReaderPtr cloneReader(boost::dynamic_pointer_cast<TermVectorsReader>(LuceneObject::clone(clone)));
    cloneReader->fieldInfos = fieldInfos;
    cloneReader->_size = _size;
    cloneReader->numTotalDocs = numTotalDocs;
    cloneReader->docStoreOffset = docStoreOffset;
    cloneReader->format = format;

    // These are null when a TermVectorsReader was created on a segment that did not have term vectors saved
    if (tvx && tvd && tvf) {
        cloneReader->tvx = boost::dynamic_pointer_cast<IndexInput>(tvx->clone());
        cloneReader->tvd = boost::dynamic_pointer_cast<IndexInput>(tvd->clone());
        cloneReader->tvf = boost::dynamic_pointer_cast<IndexInput>(tvf->clone());
    }

    return cloneReader;
}

ParallelArrayTermVectorMapper::ParallelArrayTermVectorMapper() {
    currentPosition = 0;
    storingOffsets = false;
    storingPositions = false;
}

ParallelArrayTermVectorMapper::~ParallelArrayTermVectorMapper() {
}

void ParallelArrayTermVectorMapper::setExpectations(const String& field, int32_t numTerms, bool storeOffsets, bool storePositions) {
    this->field = field;
    terms = Collection<String>::newInstance(numTerms);
    termFreqs = Collection<int32_t>::newInstance(numTerms);
    this->storingOffsets = storeOffsets;
    this->storingPositions = storePositions;

    if (storePositions) {
        this->positions = Collection< Collection<int32_t> >::newInstance(numTerms);
    }
    if (storeOffsets) {
        this->offsets = Collection< Collection<TermVectorOffsetInfoPtr> >::newInstance(numTerms);
    }
}

void ParallelArrayTermVectorMapper::map(const String& term, int32_t frequency, Collection<TermVectorOffsetInfoPtr> offsets, Collection<int32_t> positions) {
    terms[currentPosition] = term;
    termFreqs[currentPosition] = frequency;
    if (storingOffsets) {
        this->offsets[currentPosition] = offsets;
    }
    if (storingPositions) {
        this->positions[currentPosition] = positions;
    }
    ++currentPosition;
}

TermFreqVectorPtr ParallelArrayTermVectorMapper::materializeVector() {
    SegmentTermVectorPtr tv;
    if (!field.empty() && terms) {
        if (storingPositions || storingOffsets) {
            tv = newLucene<SegmentTermPositionVector>(field, terms, termFreqs, positions, offsets);
        } else {
            tv = newLucene<SegmentTermVector>(field, terms, termFreqs);
        }
    }
    return tv;
}

}
