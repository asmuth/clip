/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "FreqProxTermsWriter.h"
#include "FreqProxTermsWriterPerThread.h"
#include "FreqProxTermsWriterPerField.h"
#include "FreqProxFieldMergeState.h"
#include "TermsHashConsumerPerThread.h"
#include "TermsHashConsumerPerField.h"
#include "TermsHashPerField.h"
#include "TermsHashPerThread.h"
#include "FormatPostingsDocsConsumer.h"
#include "FormatPostingsFieldsConsumer.h"
#include "FormatPostingsFieldsWriter.h"
#include "FormatPostingsTermsConsumer.h"
#include "FormatPostingsPositionsConsumer.h"
#include "FieldInfo.h"
#include "ByteSliceReader.h"
#include "RawPostingList.h"
#include "DocumentsWriter.h"
#include "UTF8Stream.h"
#include "TestPoint.h"

namespace Lucene {

FreqProxTermsWriter::~FreqProxTermsWriter() {
}

TermsHashConsumerPerThreadPtr FreqProxTermsWriter::addThread(const TermsHashPerThreadPtr& perThread) {
    return newLucene<FreqProxTermsWriterPerThread>(perThread);
}

void FreqProxTermsWriter::createPostings(Collection<RawPostingListPtr> postings, int32_t start, int32_t count) {
    int32_t end = start + count;
    for (int32_t i = start; i < end; ++i) {
        postings[i] = newLucene<FreqProxTermsWriterPostingList>();
    }
}

int32_t FreqProxTermsWriter::compareText(const wchar_t* text1, int32_t pos1, const wchar_t* text2, int32_t pos2) {
    while (true) {
        wchar_t c1 = text1[pos1++];
        wchar_t c2 = text2[pos2++];
        if (c1 != c2) {
            if (c2 == UTF8Base::UNICODE_TERMINATOR) {
                return 1;
            } else if (c1 == UTF8Base::UNICODE_TERMINATOR) {
                return -1;
            } else {
                return (c1 - c2);
            }
        } else if (c1 == UTF8Base::UNICODE_TERMINATOR) {
            return 0;
        }
    }
}

void FreqProxTermsWriter::closeDocStore(const SegmentWriteStatePtr& state) {
}

void FreqProxTermsWriter::abort() {
}

void FreqProxTermsWriter::flush(MapTermsHashConsumerPerThreadCollectionTermsHashConsumerPerField threadsAndFields, const SegmentWriteStatePtr& state) {
    // Gather all FieldData's that have postings, across all ThreadStates
    Collection<FreqProxTermsWriterPerFieldPtr> allFields(Collection<FreqProxTermsWriterPerFieldPtr>::newInstance());

    for (MapTermsHashConsumerPerThreadCollectionTermsHashConsumerPerField::iterator entry = threadsAndFields.begin(); entry != threadsAndFields.end(); ++entry) {
        for (Collection<TermsHashConsumerPerFieldPtr>::iterator perField = entry->second.begin(); perField != entry->second.end(); ++perField) {
            FreqProxTermsWriterPerFieldPtr freqProxPerField(boost::static_pointer_cast<FreqProxTermsWriterPerField>(*perField));
            if (TermsHashPerFieldPtr(freqProxPerField->_termsHashPerField)->numPostings > 0) {
                allFields.add(freqProxPerField);
            }
        }
    }

    // Sort by field name
    std::sort(allFields.begin(), allFields.end(), luceneCompare<FreqProxTermsWriterPerFieldPtr>());

    int32_t numAllFields = allFields.size();

    FormatPostingsFieldsConsumerPtr consumer(newLucene<FormatPostingsFieldsWriter>(state, fieldInfos));

    // Current writer chain:
    // FormatPostingsFieldsConsumer
    // -> IMPL: FormatPostingsFieldsWriter
    //  -> FormatPostingsTermsConsumer
    //   -> IMPL: FormatPostingsTermsWriter
    //       -> FormatPostingsDocConsumer
    //        -> IMPL: FormatPostingsDocWriter
    //          -> FormatPostingsPositionsConsumer
    //            -> IMPL: FormatPostingsPositionsWriter

    int32_t start = 0;
    while (start < numAllFields) {
        FieldInfoPtr fieldInfo(allFields[start]->fieldInfo);
        String fieldName(fieldInfo->name);

        int32_t end = start + 1;
        while (end < numAllFields && allFields[end]->fieldInfo->name == fieldName) {
            ++end;
        }

        Collection<FreqProxTermsWriterPerFieldPtr> fields(Collection<FreqProxTermsWriterPerFieldPtr>::newInstance(end - start));
        for (int32_t i = start; i < end; ++i) {
            fields[i - start] = allFields[i];

            // Aggregate the storePayload as seen by the same field across multiple threads
            if (fields[i - start]->hasPayloads) {
                fieldInfo->storePayloads = true;
            }
        }

        // If this field has postings then add them to the segment
        appendPostings(fields, consumer);

        for (int32_t i = 0; i < fields.size(); ++i) {
            TermsHashPerFieldPtr perField(fields[i]->_termsHashPerField);
            int32_t numPostings = perField->numPostings;
            perField->reset();
            perField->shrinkHash(numPostings);
            fields[i]->reset();
        }

        start = end;
    }

    for (MapTermsHashConsumerPerThreadCollectionTermsHashConsumerPerField::iterator entry = threadsAndFields.begin(); entry != threadsAndFields.end(); ++entry) {
        TermsHashPerThreadPtr(boost::static_pointer_cast<FreqProxTermsWriterPerThread>(entry->first)->_termsHashPerThread)->reset(true);
    }

    consumer->finish();
}

void FreqProxTermsWriter::appendPostings(Collection<FreqProxTermsWriterPerFieldPtr> fields, const FormatPostingsFieldsConsumerPtr& consumer) {
    TestScope testScope(L"FreqProxTermsWriter", L"appendPostings");
    int32_t numFields = fields.size();

    Collection<FreqProxFieldMergeStatePtr> mergeStates(Collection<FreqProxFieldMergeStatePtr>::newInstance(numFields));

    for (int32_t i = 0; i < numFields; ++i) {
        FreqProxFieldMergeStatePtr fms(newLucene<FreqProxFieldMergeState>(fields[i]));
        mergeStates[i] = fms;

        BOOST_ASSERT(fms->field->fieldInfo == fields[0]->fieldInfo);

        // Should always be true
        bool result = fms->nextTerm();
        BOOST_ASSERT(result);
    }

    FormatPostingsTermsConsumerPtr termsConsumer(consumer->addField(fields[0]->fieldInfo));

    Collection<FreqProxFieldMergeStatePtr> termStates(Collection<FreqProxFieldMergeStatePtr>::newInstance(numFields));

    bool currentFieldOmitTermFreqAndPositions = fields[0]->fieldInfo->omitTermFreqAndPositions;

    while (numFields > 0) {
        // Get the next term to merge
        termStates[0] = mergeStates[0];
        int32_t numToMerge = 1;

        for (int32_t i = 1; i < numFields; ++i) {
            CharArray text = mergeStates[i]->text;
            int32_t textOffset = mergeStates[i]->textOffset;
            int32_t cmp = compareText(text.get(), textOffset, termStates[0]->text.get(), termStates[0]->textOffset);

            if (cmp < 0) {
                termStates[0] = mergeStates[i];
                numToMerge = 1;
            } else if (cmp == 0) {
                termStates[numToMerge++] = mergeStates[i];
            }
        }

        FormatPostingsDocsConsumerPtr docConsumer(termsConsumer->addTerm(termStates[0]->text, termStates[0]->textOffset));

        // Now termStates has numToMerge FieldMergeStates which all share the same term.  Now we must
        // interleave the docID streams.
        while (numToMerge > 0) {
            FreqProxFieldMergeStatePtr minState(termStates[0]);
            for (int32_t i = 1; i < numToMerge; ++i) {
                if (termStates[i]->docID < minState->docID) {
                    minState = termStates[i];
                }
            }

            int32_t termDocFreq = minState->termFreq;

            FormatPostingsPositionsConsumerPtr posConsumer(docConsumer->addDoc(minState->docID, termDocFreq));

            ByteSliceReaderPtr prox(minState->prox);

            // Carefully copy over the prox + payload info, changing the format to match Lucene's segment format.
            if (!currentFieldOmitTermFreqAndPositions) {
                // omitTermFreqAndPositions == false so we do write positions & payload
                int32_t position = 0;
                for (int32_t j = 0; j < termDocFreq; ++j) {
                    int32_t code = prox->readVInt();
                    position += (code >> 1);

                    int32_t payloadLength;
                    if ((code & 1) != 0) {
                        // This position has a payload
                        payloadLength = prox->readVInt();

                        if (!payloadBuffer) {
                            payloadBuffer = ByteArray::newInstance(payloadLength);
                        }
                        if (payloadBuffer.size() < payloadLength) {
                            payloadBuffer.resize(payloadLength);
                        }

                        prox->readBytes(payloadBuffer.get(), 0, payloadLength);
                    } else {
                        payloadLength = 0;
                    }

                    posConsumer->addPosition(position, payloadBuffer, 0, payloadLength);
                }

                posConsumer->finish();
            }

            if (!minState->nextDoc()) {
                // Remove from termStates
                int32_t upto = 0;
                for (int32_t i = 0; i < numToMerge; ++i) {
                    if (termStates[i] != minState) {
                        termStates[upto++] = termStates[i];
                    }
                }
                --numToMerge;
                BOOST_ASSERT(upto == numToMerge);

                // Advance this state to the next term

                if (!minState->nextTerm()) {
                    // OK, no more terms, so remove from mergeStates as well
                    upto = 0;
                    for (int32_t i = 0; i < numFields; ++i) {
                        if (mergeStates[i] != minState) {
                            mergeStates[upto++] = mergeStates[i];
                        }
                    }
                    --numFields;
                    BOOST_ASSERT(upto == numFields);
                }
            }
        }

        docConsumer->finish();
    }

    termsConsumer->finish();
}

int32_t FreqProxTermsWriter::bytesPerPosting() {
    return RawPostingList::BYTES_SIZE + 4 * DocumentsWriter::INT_NUM_BYTE;
}

FreqProxTermsWriterPostingList::FreqProxTermsWriterPostingList() {
    docFreq = 0;
    lastDocID = 0;
    lastDocCode = 0;
    lastPosition = 0;
}

FreqProxTermsWriterPostingList::~FreqProxTermsWriterPostingList() {
}

}
