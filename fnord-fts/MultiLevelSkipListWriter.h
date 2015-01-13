/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef MULTILEVELSKIPLISTWRITER_H
#define MULTILEVELSKIPLISTWRITER_H

#include "LuceneObject.h"

namespace Lucene {

/// This abstract class writes skip lists with multiple levels.
///
/// Example for skipInterval = 3:
///
///                                                     c            (skip level 2)
///                 c                 c                 c            (skip level 1)
///     x     x     x     x     x     x     x     x     x     x      (skip level 0)
/// d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d d  (posting list)
///     3     6     9     12    15    18    21    24    27    30     (df)
///
/// d - document
/// x - skip data
/// c - skip data with child pointer
///
/// Skip level i contains every skipInterval-th entry from skip level i-1.
/// Therefore the number of entries on level i is: floor(df / ((skipInterval ^ (i + 1))).
///
/// Each skip entry on a level i>0 contains a pointer to the corresponding skip entry in list i-1.
/// This guarantees a logarithmic amount of skips to find the target document.
///
/// While this class takes care of writing the different skip levels, subclasses must define the
/// actual format of the skip data.
class MultiLevelSkipListWriter : public LuceneObject {
public:
    MultiLevelSkipListWriter(int32_t skipInterval, int32_t maxSkipLevels, int32_t df);
    virtual ~MultiLevelSkipListWriter();

    LUCENE_CLASS(MultiLevelSkipListWriter);

protected:
    /// number of levels in this skip list
    int32_t numberOfSkipLevels;

    /// the skip interval in the list with level = 0
    int32_t skipInterval;

    /// for every skip level a different buffer is used
    Collection<RAMOutputStreamPtr> skipBuffer;

public:
    /// Writes the current skip data to the buffers. The current document frequency determines
    /// the max level is skip data is to be written to.
    /// @param df the current document frequency
    void bufferSkip(int32_t df);

    /// Writes the buffered skip lists to the given output.
    /// @param output the IndexOutput the skip lists shall be written to
    /// @return the pointer the skip list starts
    int64_t writeSkip(const IndexOutputPtr& output);

protected:
    void init();
    virtual void resetSkip();

    /// Subclasses must implement the actual skip data encoding in this method.
    /// @param level the level skip data shall be writing for
    /// @param skipBuffer the skip buffer to write to
    virtual void writeSkipData(int32_t level, const IndexOutputPtr& skipBuffer) = 0;
};

}

#endif
