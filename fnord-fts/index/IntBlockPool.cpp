/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "IntBlockPool.h"
#include "DocumentsWriter.h"

namespace Lucene {

IntBlockPool::IntBlockPool(const DocumentsWriterPtr& docWriter, bool trackAllocations) {
    this->buffers = Collection<IntArray>::newInstance(10);
    this->bufferUpto = -1;
    this->intUpto = DocumentsWriter::INT_BLOCK_SIZE;
    this->intOffset = -DocumentsWriter::INT_BLOCK_SIZE;
    this->_docWriter = docWriter;
    this->trackAllocations = trackAllocations;
}

IntBlockPool::~IntBlockPool() {
}

void IntBlockPool::reset() {
    if (bufferUpto != -1) {
        if (bufferUpto > 0) {
            // Recycle all but the first buffer
            DocumentsWriterPtr(_docWriter)->recycleIntBlocks(buffers, 1, 1 + bufferUpto);
        }

        // Reuse first buffer
        bufferUpto = 0;
        intUpto = 0;
        intOffset = 0;
        buffer = buffers[0];
    }
}

void IntBlockPool::nextBuffer() {
    if (bufferUpto + 1 == buffers.size()) {
        buffers.resize((int32_t)((double)buffers.size() * 1.5));
    }
    buffer = DocumentsWriterPtr(_docWriter)->getIntBlock(trackAllocations);
    buffers[1 + bufferUpto] = buffer;
    ++bufferUpto;

    intUpto = 0;
    intOffset += DocumentsWriter::INT_BLOCK_SIZE;
}

}
