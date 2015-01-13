/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "NormsWriterPerField.h"
#include "NormsWriterPerThread.h"
#include "Similarity.h"
#include "DocInverterPerField.h"
#include "DocumentsWriter.h"
#include "FieldInfo.h"
#include "MiscUtils.h"

namespace Lucene {

NormsWriterPerField::NormsWriterPerField(const DocInverterPerFieldPtr& docInverterPerField, const NormsWriterPerThreadPtr& perThread, const FieldInfoPtr& fieldInfo) {
    docIDs = Collection<int32_t>::newInstance(1);
    norms = ByteArray::newInstance(1);
    upto = 0;

    this->_perThread = perThread;
    this->fieldInfo = fieldInfo;
    docState = perThread->docState;
    fieldState = docInverterPerField->fieldState;
}

NormsWriterPerField::~NormsWriterPerField() {
}

void NormsWriterPerField::reset() {
    // Shrink back if we are over allocated now
    docIDs.resize(MiscUtils::getShrinkSize(docIDs.size(), upto));
    norms.resize(MiscUtils::getShrinkSize(norms.size(), upto));
    upto = 0;
}

void NormsWriterPerField::abort() {
    upto = 0;
}

int32_t NormsWriterPerField::compareTo(const LuceneObjectPtr& other) {
    return fieldInfo->name.compare(boost::static_pointer_cast<NormsWriterPerField>(other)->fieldInfo->name);
}

void NormsWriterPerField::finish() {
    BOOST_ASSERT(docIDs.size() == norms.size());
    if (fieldInfo->isIndexed && !fieldInfo->omitNorms) {
        if (docIDs.size() <= upto) {
            BOOST_ASSERT(docIDs.size() == upto);
            docIDs.resize(MiscUtils::getNextSize(1 + upto));
            norms.resize(MiscUtils::getNextSize(1 + upto));
        }
        double norm = docState->similarity->computeNorm(fieldInfo->name, fieldState);
        norms[upto] = Similarity::encodeNorm(norm);
        docIDs[upto] = docState->docID;
        ++upto;
    }
}

}
