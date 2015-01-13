/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef MULTILEVELSKIPLISTREADER_H
#define MULTILEVELSKIPLISTREADER_H

#include "IndexInput.h"

namespace Lucene {

/// This abstract class reads skip lists with multiple levels.
///
/// See {@link MultiLevelSkipListWriter} for the information about the encoding of the multi level skip lists.
///
/// Subclasses must implement the abstract method {@link #readSkipData(int, IndexInput)} which defines the
/// actual format of the skip data.
class MultiLevelSkipListReader : public LuceneObject {
public:
    MultiLevelSkipListReader(const IndexInputPtr& skipStream, int32_t maxSkipLevels, int32_t skipInterval);
    virtual ~MultiLevelSkipListReader();

    LUCENE_CLASS(MultiLevelSkipListReader);

protected:
    /// the maximum number of skip levels possible for this index
    int32_t maxNumberOfSkipLevels;

    /// number of levels in this skip list
    int32_t numberOfSkipLevels;

    /// Defines the number of top skip levels to buffer in memory.  Reducing this number results in less
    /// memory usage, but possibly slower performance due to more random I/Os.  Please notice that the space
    /// each level occupies is limited by the skipInterval. The top level can not contain more than
    /// skipLevel entries, the second top level can not contain more than skipLevel^2 entries and so forth.
    int32_t numberOfLevelsToBuffer;

    int32_t docCount;
    bool haveSkipped;

    Collection<IndexInputPtr> skipStream; // skipStream for each level
    Collection<int64_t> skipPointer; // the start pointer of each skip level
    Collection<int32_t> skipInterval; // skipInterval of each level
    Collection<int32_t> numSkipped; // number of docs skipped per level

    Collection<int32_t> skipDoc; // doc id of current skip entry per level
    int32_t lastDoc; // doc id of last read skip entry with docId <= target
    Collection<int64_t> childPointer; // child pointer of current skip entry per level
    int64_t lastChildPointer; // childPointer of last read skip entry with docId <= target

    bool inputIsBuffered;

public:
    /// Returns the id of the doc to which the last call of {@link #skipTo(int)} has skipped.
    virtual int32_t getDoc();

    /// Skips entries to the first beyond the current whose document number is greater than or equal to
    /// target. Returns the current doc count.
    virtual int32_t skipTo(int32_t target);

    virtual void close();

    /// Initializes the reader.
    virtual void init(int64_t skipPointer, int32_t df);

protected:
    virtual bool loadNextSkip(int32_t level);

    /// Seeks the skip entry on the given level
    virtual void seekChild(int32_t level);

    /// Loads the skip levels
    virtual void loadSkipLevels();

    /// Subclasses must implement the actual skip data encoding in this method.
    ///
    /// @param level the level skip data shall be read from
    /// @param skipStream the skip stream to read from
    virtual int32_t readSkipData(int32_t level, const IndexInputPtr& skipStream) = 0;

    /// Copies the values of the last read skip entry on this level
    virtual void setLastSkipData(int32_t level);
};

/// Used to buffer the top skip levels
class SkipBuffer : public IndexInput {
public:
    SkipBuffer(const IndexInputPtr& input, int32_t length);
    virtual ~SkipBuffer();

    LUCENE_CLASS(SkipBuffer);

protected:
    ByteArray data;
    int64_t pointer;
    int32_t pos;

public:
    /// Closes the stream to further operations.
    virtual void close();

    /// Returns the current position in this file, where the next read will occur.
    virtual int64_t getFilePointer();

    /// The number of bytes in the file.
    virtual int64_t length();

    /// Reads and returns a single byte.
    virtual uint8_t readByte();

    /// Reads a specified number of bytes into an array at the specified offset.
    virtual void readBytes(uint8_t* b, int32_t offset, int32_t length);

    /// Sets current position in this file, where the next read will occur.
    virtual void seek(int64_t pos);
};

}

#endif
