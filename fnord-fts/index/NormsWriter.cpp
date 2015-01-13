/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "NormsWriter.h"
#include "NormsWriterPerThread.h"
#include "NormsWriterPerField.h"
#include "Similarity.h"
#include "IndexFileNames.h"
#include "IndexOutput.h"
#include "SegmentMerger.h"
#include "SegmentWriteState.h"
#include "InvertedDocEndConsumerPerField.h"
#include "FieldInfos.h"
#include "FieldInfo.h"
#include "Directory.h"

namespace Lucene {

NormsWriter::NormsWriter() {
}

NormsWriter::~NormsWriter() {
}

uint8_t NormsWriter::getDefaultNorm() {
    static uint8_t defaultNorm = 0;
    if (defaultNorm == 0) {
        defaultNorm = Similarity::encodeNorm(1.0);
    }
    return defaultNorm;
}

InvertedDocEndConsumerPerThreadPtr NormsWriter::addThread(const DocInverterPerThreadPtr& docInverterPerThread) {
    return newLucene<NormsWriterPerThread>(docInverterPerThread, shared_from_this());
}

void NormsWriter::abort() {
}

void NormsWriter::files(HashSet<String> files) {
}

void NormsWriter::setFieldInfos(const FieldInfosPtr& fieldInfos) {
    this->fieldInfos = fieldInfos;
}

void NormsWriter::flush(MapInvertedDocEndConsumerPerThreadCollectionInvertedDocEndConsumerPerField threadsAndFields, const SegmentWriteStatePtr& state) {
    MapFieldInfoCollectionNormsWriterPerField byField(MapFieldInfoCollectionNormsWriterPerField::newInstance());

    // Typically, each thread will have encountered the same field.  So first we collate by field, ie all
    // per-thread field instances that correspond to the same FieldInfo
    for (MapInvertedDocEndConsumerPerThreadCollectionInvertedDocEndConsumerPerField::iterator entry = threadsAndFields.begin(); entry != threadsAndFields.end(); ++entry) {
        for (Collection<InvertedDocEndConsumerPerFieldPtr>::iterator perField = entry->second.begin(); perField != entry->second.end();) {
            NormsWriterPerFieldPtr normsPerField(boost::static_pointer_cast<NormsWriterPerField>(*perField));
            if (normsPerField->upto > 0) {
                // It has some norms
                Collection<NormsWriterPerFieldPtr> l = byField.get(normsPerField->fieldInfo);
                if (!l) {
                    l = Collection<NormsWriterPerFieldPtr>::newInstance();
                    byField.put(normsPerField->fieldInfo, l);
                }
                l.add(normsPerField);
                ++perField;
            } else {
                // Remove this field since we haven't seen it since the previous flush
                perField = entry->second.remove(perField);
            }
        }
    }

    String normsFileName(state->segmentName + L"." + IndexFileNames::NORMS_EXTENSION());
    state->flushedFiles.add(normsFileName);
    IndexOutputPtr normsOut(state->directory->createOutput(normsFileName));

    LuceneException finally;
    try {
        normsOut->writeBytes(SegmentMerger::NORMS_HEADER, 0, SegmentMerger::NORMS_HEADER_LENGTH);

        int32_t numField = fieldInfos->size();

        int32_t normCount = 0;

        for (int32_t fieldNumber = 0; fieldNumber < numField; ++fieldNumber) {
            FieldInfoPtr fieldInfo(fieldInfos->fieldInfo(fieldNumber));

            Collection<NormsWriterPerFieldPtr> toMerge = byField.get(fieldInfo);
            int32_t upto = 0;

            if (toMerge) {
                int32_t numFields = toMerge.size();

                ++normCount;

                Collection<NormsWriterPerFieldPtr> fields(Collection<NormsWriterPerFieldPtr>::newInstance(numFields));
                Collection<int32_t> uptos(Collection<int32_t>::newInstance(numFields));

                for (int32_t j = 0; j < numFields; ++j) {
                    fields[j] = toMerge[j];
                }

                int32_t numLeft = numFields;

                while (numLeft > 0) {
                    BOOST_ASSERT(uptos[0] < fields[0]->docIDs.size());

                    int32_t minLoc = 0;
                    int32_t minDocID = fields[0]->docIDs[uptos[0]];

                    for (int32_t j = 1; j < numLeft; ++j) {
                        int32_t docID = fields[j]->docIDs[uptos[j]];
                        if (docID < minDocID) {
                            minDocID = docID;
                            minLoc = j;
                        }
                    }

                    BOOST_ASSERT(minDocID < state->numDocs);

                    // Fill hole
                    for (; upto < minDocID; ++upto) {
                        normsOut->writeByte(getDefaultNorm());
                    }

                    normsOut->writeByte(fields[minLoc]->norms[uptos[minLoc]]);
                    ++(uptos[minLoc]);
                    ++upto;

                    if (uptos[minLoc] == fields[minLoc]->upto) {
                        fields[minLoc]->reset();
                        if (minLoc != numLeft - 1) {
                            fields[minLoc] = fields[numLeft - 1];
                            uptos[minLoc] = uptos[numLeft - 1];
                        }
                        --numLeft;
                    }
                }

                // Fill final hole with defaultNorm
                for (; upto < state->numDocs; ++upto) {
                    normsOut->writeByte(getDefaultNorm());
                }
            } else if (fieldInfo->isIndexed && !fieldInfo->omitNorms) {
                ++normCount;
                // Fill entire field with default norm
                for (; upto < state->numDocs; ++upto) {
                    normsOut->writeByte(getDefaultNorm());
                }
            }

            BOOST_ASSERT(4 + normCount * state->numDocs == normsOut->getFilePointer()); // .nrm file size mismatch?
        }
    } catch (LuceneException& e) {
        finally = e;
    }

    normsOut->close();

    finally.throwException();
}

void NormsWriter::closeDocStore(const SegmentWriteStatePtr& state) {
}

}
