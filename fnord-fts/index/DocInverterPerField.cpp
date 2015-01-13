/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "DocInverterPerField.h"
#include "DocInverterPerThread.h"
#include "InvertedDocConsumerPerThread.h"
#include "InvertedDocEndConsumerPerThread.h"
#include "InvertedDocConsumerPerField.h"
#include "InvertedDocEndConsumerPerField.h"
#include "Fieldable.h"
#include "FieldInfo.h"
#include "FieldInvertState.h"
#include "DocumentsWriter.h"
#include "Document.h"
#include "Analyzer.h"
#include "ReusableStringReader.h"
#include "TokenStream.h"
#include "PositionIncrementAttribute.h"
#include "OffsetAttribute.h"
#include "AttributeSource.h"
#include "InfoStream.h"
#include "StringUtils.h"

namespace Lucene {

DocInverterPerField::DocInverterPerField(const DocInverterPerThreadPtr& perThread, const FieldInfoPtr& fieldInfo) {
    this->_perThread = perThread;
    this->fieldInfo = fieldInfo;
    docState = perThread->docState;
    fieldState = perThread->fieldState;
}

DocInverterPerField::~DocInverterPerField() {
}

void DocInverterPerField::initialize() {
    DocInverterPerThreadPtr perThread(_perThread);
    consumer = perThread->consumer->addField(shared_from_this(), fieldInfo);
    endConsumer = perThread->endConsumer->addField(shared_from_this(), fieldInfo);
}

void DocInverterPerField::abort() {
    consumer->abort();
    endConsumer->abort();
}

void DocInverterPerField::processFields(Collection<FieldablePtr> fields, int32_t count) {
    fieldState->reset(docState->doc->getBoost());

    int32_t maxFieldLength = docState->maxFieldLength;
    bool doInvert = consumer->start(fields, count);
    DocumentsWriterPtr docWriter(docState->_docWriter);
    DocInverterPerThreadPtr perThread(_perThread);

    for (int32_t i = 0; i < count; ++i) {
        FieldablePtr field = fields[i];
        if (field->isIndexed() && doInvert) {
            bool anyToken;

            if (fieldState->length > 0) {
                fieldState->position += docState->analyzer->getPositionIncrementGap(fieldInfo->name);
            }

            if (!field->isTokenized()) {
                // un-tokenized field
                String stringValue(field->stringValue());
                int32_t valueLength = (int32_t)stringValue.length();
                perThread->singleToken->reinit(stringValue, 0, valueLength);
                fieldState->attributeSource = perThread->singleToken;
                consumer->start(field);

                bool success = false;
                LuceneException finally;
                try {
                    consumer->add();
                    success = true;
                } catch (LuceneException& e) {
                    finally = e;
                }
                if (!success) {
                    docWriter->setAborting();
                }
                finally.throwException();
                fieldState->offset += valueLength;
                ++fieldState->length;
                ++fieldState->position;
                anyToken = (valueLength > 0);
            } else {
                // tokenized field
                TokenStreamPtr stream;
                TokenStreamPtr streamValue(field->tokenStreamValue());

                if (streamValue) {
                    stream = streamValue;
                } else {
                    // the field does not have a TokenStream, so we have to obtain one from the analyzer
                    ReaderPtr reader; // find or make Reader
                    ReaderPtr readerValue(field->readerValue());

                    if (readerValue) {
                        reader = readerValue;
                    } else {
                        String stringValue(field->stringValue());
                        perThread->stringReader->init(stringValue);
                        reader = perThread->stringReader;
                    }

                    // Tokenize field and add to postingTable
                    stream = docState->analyzer->reusableTokenStream(fieldInfo->name, reader);
                }

                // reset the TokenStream to the first token
                stream->reset();

                int32_t startLength = fieldState->length;

                LuceneException finally;
                try {
                    int32_t offsetEnd = fieldState->offset - 1;

                    bool hasMoreTokens = stream->incrementToken();

                    fieldState->attributeSource = stream;

                    OffsetAttributePtr offsetAttribute(fieldState->attributeSource->addAttribute<OffsetAttribute>());
                    PositionIncrementAttributePtr posIncrAttribute(fieldState->attributeSource->addAttribute<PositionIncrementAttribute>());

                    consumer->start(field);

                    while (true) {
                        // If we hit an exception in stream.next below (which is fairly common, eg if analyzer
                        // chokes on a given document), then it's non-aborting and (above) this one document
                        // will be marked as deleted, but still consume a docID
                        if (!hasMoreTokens) {
                            break;
                        }

                        int32_t posIncr = posIncrAttribute->getPositionIncrement();
                        fieldState->position += posIncr;
                        if (fieldState->position > 0) {
                            --fieldState->position;
                        }

                        if (posIncr == 0) {
                            ++fieldState->numOverlap;
                        }

                        bool success = false;
                        try {
                            // If we hit an exception in here, we abort all buffered documents since the last
                            // flush, on the likelihood that the internal state of the consumer is now corrupt
                            // and should not be flushed to a new segment
                            consumer->add();
                            success = true;
                        } catch (LuceneException& e) {
                            finally = e;
                        }
                        if (!success) {
                            docWriter->setAborting();
                        }
                        finally.throwException();
                        ++fieldState->position;
                        offsetEnd = fieldState->offset + offsetAttribute->endOffset();
                        if (++fieldState->length >= maxFieldLength) {
                            if (docState->infoStream) {
                                *docState->infoStream << L"maxFieldLength " << StringUtils::toString(maxFieldLength) << L" reached for field " << fieldInfo->name << L", ignoring following tokens\n";
                            }
                            break;
                        }

                        hasMoreTokens = stream->incrementToken();
                    }

                    // trigger streams to perform end-of-stream operations
                    stream->end();

                    fieldState->offset += offsetAttribute->endOffset();
                    anyToken = (fieldState->length > startLength);
                } catch (LuceneException& e) {
                    finally = e;
                }
                stream->close();
                finally.throwException();
            }

            if (anyToken) {
                fieldState->offset += docState->analyzer->getOffsetGap(field);
            }
            fieldState->boost *= field->getBoost();
        }

        // don't hang onto the field
        fields[i].reset();
    }

    consumer->finish();
    endConsumer->finish();
}

}
