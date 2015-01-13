/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "DocInverter.h"
#include "InvertedDocConsumer.h"
#include "InvertedDocEndConsumer.h"
#include "InvertedDocConsumerPerThread.h"
#include "InvertedDocEndConsumerPerThread.h"
#include "DocFieldConsumerPerThread.h"
#include "DocFieldConsumerPerField.h"
#include "DocInverterPerField.h"
#include "DocInverterPerThread.h"

namespace Lucene {

DocInverter::DocInverter(const InvertedDocConsumerPtr& consumer, const InvertedDocEndConsumerPtr& endConsumer) {
    this->consumer = consumer;
    this->endConsumer = endConsumer;
}

DocInverter::~DocInverter() {
}

void DocInverter::setFieldInfos(const FieldInfosPtr& fieldInfos) {
    DocFieldConsumer::setFieldInfos(fieldInfos);
    consumer->setFieldInfos(fieldInfos);
    endConsumer->setFieldInfos(fieldInfos);
}

void DocInverter::flush(MapDocFieldConsumerPerThreadCollectionDocFieldConsumerPerField threadsAndFields, const SegmentWriteStatePtr& state) {
    MapInvertedDocConsumerPerThreadCollectionInvertedDocConsumerPerField childThreadsAndFields(MapInvertedDocConsumerPerThreadCollectionInvertedDocConsumerPerField::newInstance());
    MapInvertedDocEndConsumerPerThreadCollectionInvertedDocEndConsumerPerField endChildThreadsAndFields(MapInvertedDocEndConsumerPerThreadCollectionInvertedDocEndConsumerPerField::newInstance());

    for (MapDocFieldConsumerPerThreadCollectionDocFieldConsumerPerField::iterator entry = threadsAndFields.begin(); entry != threadsAndFields.end(); ++entry) {
        Collection<InvertedDocConsumerPerFieldPtr> childFields(Collection<InvertedDocConsumerPerFieldPtr>::newInstance());
        Collection<InvertedDocEndConsumerPerFieldPtr> endChildFields(Collection<InvertedDocEndConsumerPerFieldPtr>::newInstance());

        for (Collection<DocFieldConsumerPerFieldPtr>::iterator perField = entry->second.begin(); perField != entry->second.end(); ++perField) {
            childFields.add(boost::static_pointer_cast<DocInverterPerField>(*perField)->consumer);
            endChildFields.add(boost::static_pointer_cast<DocInverterPerField>(*perField)->endConsumer);
        }

        childThreadsAndFields.put(boost::static_pointer_cast<DocInverterPerThread>(entry->first)->consumer, childFields);
        endChildThreadsAndFields.put(boost::static_pointer_cast<DocInverterPerThread>(entry->first)->endConsumer, endChildFields);
    }

    consumer->flush(childThreadsAndFields, state);
    endConsumer->flush(endChildThreadsAndFields, state);
}

void DocInverter::closeDocStore(const SegmentWriteStatePtr& state) {
    consumer->closeDocStore(state);
    endConsumer->closeDocStore(state);
}

void DocInverter::abort() {
    consumer->abort();
    endConsumer->abort();
}

bool DocInverter::freeRAM() {
    return consumer->freeRAM();
}

DocFieldConsumerPerThreadPtr DocInverter::addThread(const DocFieldProcessorPerThreadPtr& docFieldProcessorPerThread) {
    return newLucene<DocInverterPerThread>(docFieldProcessorPerThread, shared_from_this());
}

}
