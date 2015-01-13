/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "TermVectorsWriter.h"
#include "IndexFileNames.h"
#include "IndexOutput.h"
#include "TermVectorsReader.h"
#include "TermVectorOffsetInfo.h"
#include "TermPositionVector.h"
#include "Directory.h"
#include "FieldInfos.h"
#include "MiscUtils.h"
#include "UnicodeUtils.h"
#include "StringUtils.h"

namespace Lucene {

TermVectorsWriter::TermVectorsWriter(const DirectoryPtr& directory, const String& segment, const FieldInfosPtr& fieldInfos) {
    utf8Results = newCollection<UTF8ResultPtr>(newInstance<UTF8Result>(), newInstance<UTF8Result>());

    // Open files for TermVector storage
    tvx = directory->createOutput(segment + L"." + IndexFileNames::VECTORS_INDEX_EXTENSION());
    tvx->writeInt(TermVectorsReader::FORMAT_CURRENT);
    tvd = directory->createOutput(segment + L"." + IndexFileNames::VECTORS_DOCUMENTS_EXTENSION());
    tvd->writeInt(TermVectorsReader::FORMAT_CURRENT);
    tvf = directory->createOutput(segment + L"." + IndexFileNames::VECTORS_FIELDS_EXTENSION());
    tvf->writeInt(TermVectorsReader::FORMAT_CURRENT);

    this->fieldInfos = fieldInfos;
}

TermVectorsWriter::~TermVectorsWriter() {
}

void TermVectorsWriter::addAllDocVectors(Collection<TermFreqVectorPtr> vectors) {
    tvx->writeLong(tvd->getFilePointer());
    tvx->writeLong(tvf->getFilePointer());

    if (vectors) {
        int32_t numFields = vectors.size();
        tvd->writeVInt(numFields);

        Collection<int64_t> fieldPointers(Collection<int64_t>::newInstance(numFields));

        for (int32_t i = 0; i < numFields; ++i) {
            fieldPointers[i] = tvf->getFilePointer();

            int32_t fieldNumber = fieldInfos->fieldNumber(vectors[i]->getField());

            // 1st pass: write field numbers to tvd
            tvd->writeVInt(fieldNumber);

            int32_t numTerms = vectors[i]->size();
            tvf->writeVInt(numTerms);

            TermPositionVectorPtr tpVector(boost::dynamic_pointer_cast<TermPositionVector>(vectors[i]));

            uint8_t bits;
            bool storePositions;
            bool storeOffsets;

            if (tpVector) {
                // May have positions & offsets
                storePositions = (tpVector->size() > 0 && !tpVector->getTermPositions(0));
                storeOffsets = (tpVector->size() > 0 && tpVector->getOffsets(0));
                bits = (uint8_t)((storePositions ? TermVectorsReader::STORE_POSITIONS_WITH_TERMVECTOR : 0) +
                                 (storeOffsets ? TermVectorsReader::STORE_OFFSET_WITH_TERMVECTOR : 0));
            } else {
                bits = 0;
                storePositions = false;
                storeOffsets = false;
            }

            tvf->writeVInt(bits);
            Collection<String> terms(vectors[i]->getTerms());
            Collection<int32_t> freqs(vectors[i]->getTermFrequencies());

            int32_t utf8Upto = 0;
            utf8Results[1]->length = 0;

            for (int32_t j = 0; j < numTerms; ++j) {
                StringUtils::toUTF8(terms[j].c_str(), terms[j].length(), utf8Results[utf8Upto]);

                int32_t start = MiscUtils::bytesDifference(utf8Results[1 - utf8Upto]->result.get(), utf8Results[1 - utf8Upto]->length,
                                utf8Results[utf8Upto]->result.get(), utf8Results[utf8Upto]->length);
                int32_t length = utf8Results[utf8Upto]->length - start;
                tvf->writeVInt(start); // write shared prefix length
                tvf->writeVInt(length); // write delta length
                tvf->writeBytes(utf8Results[utf8Upto]->result.get(), start, length); // write delta bytes
                utf8Upto = 1 - utf8Upto;

                int32_t termFreq = freqs[j];
                tvf->writeVInt(termFreq);

                if (storePositions) {
                    Collection<int32_t> positions(tpVector->getTermPositions(j));
                    if (!positions) {
                        boost::throw_exception(IllegalStateException(L"Trying to write positions that are null!"));
                    }
                    BOOST_ASSERT(positions.size() == termFreq);

                    // use delta encoding for positions
                    int32_t lastPosition = 0;
                    for (int32_t k = 0; k < positions.size(); ++k) {
                        int32_t position = positions[k];
                        tvf->writeVInt(position - lastPosition);
                        lastPosition = position;
                    }
                }

                if (storeOffsets) {
                    Collection<TermVectorOffsetInfoPtr> offsets(tpVector->getOffsets(j));
                    if (!offsets) {
                        boost::throw_exception(IllegalStateException(L"Trying to write offsets that are null!"));
                    }
                    BOOST_ASSERT(offsets.size() == termFreq);

                    // use delta encoding for offsets
                    int32_t lastEndOffset = 0;
                    for (int32_t k = 0; k < offsets.size(); ++k) {
                        int32_t startOffset = offsets[k]->getStartOffset();
                        int32_t endOffset = offsets[k]->getEndOffset();
                        tvf->writeVInt(startOffset - lastEndOffset);
                        tvf->writeVInt(endOffset - startOffset);
                        lastEndOffset = endOffset;
                    }
                }
            }
        }

        // 2nd pass: write field pointers to tvd
        if (numFields > 1) {
            int64_t lastFieldPointer = fieldPointers[0];
            for (int32_t i = 1; i < numFields; ++i) {
                int64_t fieldPointer = fieldPointers[i];
                tvd->writeVLong(fieldPointer - lastFieldPointer);
                lastFieldPointer = fieldPointer;
            }
        }
    } else {
        tvd->writeVInt(0);
    }
}

void TermVectorsWriter::addRawDocuments(const TermVectorsReaderPtr& reader, Collection<int32_t> tvdLengths, Collection<int32_t> tvfLengths, int32_t numDocs) {
    int64_t tvdPosition = tvd->getFilePointer();
    int64_t tvfPosition = tvf->getFilePointer();
    int64_t tvdStart = tvdPosition;
    int64_t tvfStart = tvfPosition;
    for (int32_t i = 0; i < numDocs; ++i) {
        tvx->writeLong(tvdPosition);
        tvdPosition += tvdLengths[i];
        tvx->writeLong(tvfPosition);
        tvfPosition += tvfLengths[i];
    }
    tvd->copyBytes(reader->getTvdStream(), tvdPosition - tvdStart);
    tvf->copyBytes(reader->getTvfStream(), tvfPosition - tvfStart);
    BOOST_ASSERT(tvd->getFilePointer() == tvdPosition);
    BOOST_ASSERT(tvf->getFilePointer() == tvfPosition);
}

void TermVectorsWriter::close() {
    // make an effort to close all streams we can but remember and re-throw the first exception
    // encountered in this process
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

}
