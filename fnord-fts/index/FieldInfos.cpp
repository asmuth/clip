/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "FieldInfos.h"
#include "FieldInfo.h"
#include "IndexInput.h"
#include "IndexOutput.h"
#include "Directory.h"
#include "Document.h"
#include "Fieldable.h"
#include "StringUtils.h"

namespace Lucene {

// Used internally (ie not written to *.fnm files) for pre-2.9 files
const int32_t FieldInfos::FORMAT_PRE = -1;

// First used in 2.9; prior to 2.9 there was no format header
const int32_t FieldInfos::FORMAT_START = -2;

const int32_t FieldInfos::CURRENT_FORMAT = FieldInfos::FORMAT_START;

const uint8_t FieldInfos::IS_INDEXED = 0x1;
const uint8_t FieldInfos::STORE_TERMVECTOR = 0x2;
const uint8_t FieldInfos::STORE_POSITIONS_WITH_TERMVECTOR = 0x4;
const uint8_t FieldInfos::STORE_OFFSET_WITH_TERMVECTOR = 0x8;
const uint8_t FieldInfos::OMIT_NORMS = 0x10;
const uint8_t FieldInfos::STORE_PAYLOADS = 0x20;
const uint8_t FieldInfos::OMIT_TERM_FREQ_AND_POSITIONS = 0x40;

FieldInfos::FieldInfos() {
    format = 0;
    byNumber = Collection<FieldInfoPtr>::newInstance();
    byName = MapStringFieldInfo::newInstance();
}

FieldInfos::FieldInfos(const DirectoryPtr& d, const String& name) {
    format = 0;
    byNumber = Collection<FieldInfoPtr>::newInstance();
    byName = MapStringFieldInfo::newInstance();
    IndexInputPtr input(d->openInput(name));
    LuceneException finally;
    try {
        try {
            read(input, name);
        } catch (IOException& e) {
            if (format == FORMAT_PRE) {
                input->seek(0);
                input->setModifiedUTF8StringsMode();
                byNumber.clear();
                byName.clear();
                try {
                    read(input, name);
                } catch (...) {
                    // Ignore any new exception & throw original IOE
                    finally = e;
                }
            } else {
                finally = e;
            }
        }
    } catch (LuceneException& e) {
        finally = e;
    }
    input->close();
    finally.throwException();
}

FieldInfos::~FieldInfos() {
}

LuceneObjectPtr FieldInfos::clone(const LuceneObjectPtr& other) {
    SyncLock syncLock(this);
    FieldInfosPtr fis(newLucene<FieldInfos>());
    for (Collection<FieldInfoPtr>::iterator field = byNumber.begin(); field != byNumber.end(); ++field) {
        FieldInfoPtr fi(boost::dynamic_pointer_cast<FieldInfo>((*field)->clone()));
        fis->byNumber.add(fi);
        fis->byName.put(fi->name, fi);
    }
    return fis;
}

void FieldInfos::add(const DocumentPtr& doc) {
    SyncLock syncLock(this);
    Collection<FieldablePtr> fields(doc->getFields());
    for (Collection<FieldablePtr>::iterator field = fields.begin(); field != fields.end(); ++field) {
        add((*field)->name(), (*field)->isIndexed(), (*field)->isTermVectorStored(),
            (*field)->isStorePositionWithTermVector(), (*field)->isStoreOffsetWithTermVector(),
            (*field)->getOmitNorms(), false, (*field)->getOmitTermFreqAndPositions());
    }
}

bool FieldInfos::hasProx() {
    for (Collection<FieldInfoPtr>::iterator fi = byNumber.begin(); fi != byNumber.end(); ++fi) {
        if ((*fi)->isIndexed && !(*fi)->omitTermFreqAndPositions) {
            return true;
        }
    }
    return false;
}

void FieldInfos::addIndexed(HashSet<String> names, bool storeTermVectors, bool storePositionWithTermVector, bool storeOffsetWithTermVector) {
    SyncLock syncLock(this);
    for (HashSet<String>::iterator name = names.begin(); name != names.end(); ++name) {
        add(*name, true, storeTermVectors, storePositionWithTermVector, storeOffsetWithTermVector);
    }
}

void FieldInfos::add(HashSet<String> names, bool isIndexed) {
    SyncLock syncLock(this);
    for (HashSet<String>::iterator name = names.begin(); name != names.end(); ++name) {
        add(*name, isIndexed);
    }
}

void FieldInfos::add(const String& name, bool isIndexed) {
    add(name, isIndexed, false, false, false, false);
}

void FieldInfos::add(const String& name, bool isIndexed, bool storeTermVector) {
    add(name, isIndexed, storeTermVector, false, false, false);
}

void FieldInfos::add(const String& name, bool isIndexed, bool storeTermVector, bool storePositionWithTermVector, bool storeOffsetWithTermVector) {
    add(name, isIndexed, storeTermVector, storePositionWithTermVector, storeOffsetWithTermVector, false);
}

void FieldInfos::add(const String& name, bool isIndexed, bool storeTermVector, bool storePositionWithTermVector,
                     bool storeOffsetWithTermVector, bool omitNorms) {
    add(name, isIndexed, storeTermVector, storePositionWithTermVector, storeOffsetWithTermVector, omitNorms, false, false);
}

FieldInfoPtr FieldInfos::add(const String& name, bool isIndexed, bool storeTermVector, bool storePositionWithTermVector,
                             bool storeOffsetWithTermVector, bool omitNorms, bool storePayloads, bool omitTermFreqAndPositions) {
    SyncLock syncLock(this);
    FieldInfoPtr fi(fieldInfo(name));
    if (!fi) {
        return addInternal(name, isIndexed, storeTermVector, storePositionWithTermVector, storeOffsetWithTermVector, omitNorms, storePayloads, omitTermFreqAndPositions);
    } else {
        fi->update(isIndexed, storeTermVector, storePositionWithTermVector, storeOffsetWithTermVector, omitNorms, storePayloads, omitTermFreqAndPositions);
    }
    return fi;
}

FieldInfoPtr FieldInfos::addInternal(const String& name, bool isIndexed, bool storeTermVector, bool storePositionWithTermVector,
                                     bool storeOffsetWithTermVector, bool omitNorms, bool storePayloads, bool omitTermFreqAndPositions) {
    FieldInfoPtr fi(newLucene<FieldInfo>(name, isIndexed, byNumber.size(), storeTermVector,
                                         storePositionWithTermVector, storeOffsetWithTermVector,
                                         omitNorms, storePayloads, omitTermFreqAndPositions));
    byNumber.add(fi);
    byName.put(name, fi);
    return fi;
}

int32_t FieldInfos::fieldNumber(const String& fieldName) {
    FieldInfoPtr fi(fieldInfo(fieldName));
    return fi ? fi->number : -1;
}

FieldInfoPtr FieldInfos::fieldInfo(const String& fieldName) {
    return byName.get(fieldName);
}

String FieldInfos::fieldName(int32_t fieldNumber) {
    FieldInfoPtr fi(fieldInfo(fieldNumber));
    return fi ? fi->name : L"";
}

FieldInfoPtr FieldInfos::fieldInfo(int32_t fieldNumber) {
    return (fieldNumber >= 0 && fieldNumber < byNumber.size()) ? byNumber[fieldNumber] : FieldInfoPtr();
}

int32_t FieldInfos::size() {
    return byNumber.size();
}

bool FieldInfos::hasVectors() {
    for (Collection<FieldInfoPtr>::iterator fi = byNumber.begin(); fi != byNumber.end(); ++fi) {
        if ((*fi)->storeTermVector) {
            return true;
        }
    }
    return false;
}

void FieldInfos::write(const DirectoryPtr& d, const String& name) {
    IndexOutputPtr output(d->createOutput(name));
    LuceneException finally;
    try {
        write(output);
    } catch (LuceneException& e) {
        finally = e;
    }
    output->close();
    finally.throwException();
}

void FieldInfos::write(const IndexOutputPtr& output) {
    output->writeVInt(CURRENT_FORMAT);
    output->writeVInt(size());
    for (Collection<FieldInfoPtr>::iterator fi = byNumber.begin(); fi != byNumber.end(); ++fi) {
        uint8_t bits = 0x0;
        if ((*fi)->isIndexed) {
            bits |= IS_INDEXED;
        }
        if ((*fi)->storeTermVector) {
            bits |= STORE_TERMVECTOR;
        }
        if ((*fi)->storePositionWithTermVector) {
            bits |= STORE_POSITIONS_WITH_TERMVECTOR;
        }
        if ((*fi)->storeOffsetWithTermVector) {
            bits |= STORE_OFFSET_WITH_TERMVECTOR;
        }
        if ((*fi)->omitNorms) {
            bits |= OMIT_NORMS;
        }
        if ((*fi)->storePayloads) {
            bits |= STORE_PAYLOADS;
        }
        if ((*fi)->omitTermFreqAndPositions) {
            bits |= OMIT_TERM_FREQ_AND_POSITIONS;
        }

        output->writeString((*fi)->name);
        output->writeByte(bits);
    }
}

void FieldInfos::read(const IndexInputPtr& input, const String& fileName) {
    int32_t firstInt = input->readVInt();
    format = firstInt < 0 ? firstInt : FORMAT_PRE; // This is a real format?

    if (format != FORMAT_PRE && format != FORMAT_START) {
        boost::throw_exception(CorruptIndexException(L"unrecognized format " + StringUtils::toString(format) + L" in file \"" + fileName + L"\""));
    }

    int32_t size = format == FORMAT_PRE ? firstInt : input->readVInt(); // read in the size if required
    for (int32_t i = 0; i < size; ++i) {
        String name(input->readString());
        uint8_t bits = input->readByte();

        addInternal(name, (bits & IS_INDEXED) != 0, (bits & STORE_TERMVECTOR) != 0, (bits & STORE_POSITIONS_WITH_TERMVECTOR) != 0,
                    (bits & STORE_OFFSET_WITH_TERMVECTOR) != 0, (bits & OMIT_NORMS) != 0, (bits & STORE_PAYLOADS) != 0,
                    (bits & OMIT_TERM_FREQ_AND_POSITIONS) != 0);
    }

    if (input->getFilePointer() != input->length()) {
        boost::throw_exception(CorruptIndexException(L"did not read all bytes from file \"" + fileName + L"\": read " +
                               StringUtils::toString(input->getFilePointer()) + L" vs size " +
                               StringUtils::toString(input->length())));
    }
}

}
