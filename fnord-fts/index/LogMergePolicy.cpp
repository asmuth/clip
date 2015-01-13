/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "LogMergePolicy.h"
#include "IndexWriter.h"
#include "SegmentInfo.h"
#include "StringUtils.h"

namespace Lucene {

/// Defines the allowed range of log(size) for each level.  A level is computed by taking the max segment
/// log size, minus LEVEL_LOG_SPAN, and finding all segments falling within that range.
const double LogMergePolicy::LEVEL_LOG_SPAN = 0.75;

/// Default merge factor, which is how many segments are merged at a time.
const int32_t LogMergePolicy::DEFAULT_MERGE_FACTOR = 10;

/// Default maximum segment size.  A segment of this size or larger will never be merged.
const int32_t LogMergePolicy::DEFAULT_MAX_MERGE_DOCS = INT_MAX;

/// Default noCFSRatio.  If a merge's size is >= 10% of the index, then we disable compound file for it.
const double LogMergePolicy::DEFAULT_NO_CFS_RATIO = 0.1;

LogMergePolicy::LogMergePolicy(const IndexWriterPtr& writer) : MergePolicy(writer) {
    mergeFactor = DEFAULT_MERGE_FACTOR;
    noCFSRatio = DEFAULT_NO_CFS_RATIO;
    minMergeSize = 0;
    maxMergeSize = 0;
    maxMergeDocs = DEFAULT_MAX_MERGE_DOCS;
    calibrateSizeByDeletes = false;
    _useCompoundFile = true;
    _useCompoundDocStore = true;
}

LogMergePolicy::~LogMergePolicy() {
}

double LogMergePolicy::getNoCFSRatio() {
    return noCFSRatio;
}

void LogMergePolicy::setNoCFSRatio(double noCFSRatio) {
    if (noCFSRatio < 0.0 || noCFSRatio > 1.0) {
        boost::throw_exception(IllegalArgumentException(L"noCFSRatio must be 0.0 to 1.0 inclusive; got " + StringUtils::toString(noCFSRatio)));
    }
    this->noCFSRatio = noCFSRatio;
}

bool LogMergePolicy::verbose() {
    return (!_writer.expired() && IndexWriterPtr(_writer)->verbose());
}

void LogMergePolicy::message(const String& message) {
    if (verbose()) {
        IndexWriterPtr(_writer)->message(L"LMP: " + message);
    }
}

int32_t LogMergePolicy::getMergeFactor() {
    return mergeFactor;
}

void LogMergePolicy::setMergeFactor(int32_t mergeFactor) {
    if (mergeFactor < 2) {
        boost::throw_exception(IllegalArgumentException(L"mergeFactor cannot be less than 2"));
    }
    this->mergeFactor = mergeFactor;
}

bool LogMergePolicy::getUseCompoundFile() {
    return _useCompoundFile;
}

void LogMergePolicy::setUseCompoundFile(bool useCompoundFile) {
    _useCompoundFile = useCompoundFile;
}

bool LogMergePolicy::useCompoundFile(const SegmentInfosPtr& segments, const SegmentInfoPtr& newSegment) {
    return _useCompoundFile;
}

bool LogMergePolicy::useCompoundDocStore(const SegmentInfosPtr& segments) {
    return _useCompoundDocStore;
}

void LogMergePolicy::setUseCompoundDocStore(bool useCompoundDocStore) {
    _useCompoundDocStore = useCompoundDocStore;
}

bool LogMergePolicy::getUseCompoundDocStore() {
    return _useCompoundDocStore;
}

void LogMergePolicy::setCalibrateSizeByDeletes(bool calibrateSizeByDeletes) {
    this->calibrateSizeByDeletes = calibrateSizeByDeletes;
}

bool LogMergePolicy::getCalibrateSizeByDeletes() {
    return calibrateSizeByDeletes;
}

void LogMergePolicy::close() {
}

int64_t LogMergePolicy::sizeDocs(const SegmentInfoPtr& info) {
    if (calibrateSizeByDeletes) {
        int32_t delCount = IndexWriterPtr(_writer)->numDeletedDocs(info);
        return (info->docCount - (int64_t)delCount);
    } else {
        return info->docCount;
    }
}

int64_t LogMergePolicy::sizeBytes(const SegmentInfoPtr& info) {
    int64_t byteSize = info->sizeInBytes();
    if (calibrateSizeByDeletes) {
        int32_t delCount = IndexWriterPtr(_writer)->numDeletedDocs(info);
        double delRatio = info->docCount <= 0 ? 0.0 : ((double)delCount / (double)info->docCount);
        return info->docCount <= 0 ? byteSize : (int64_t)(byteSize * (1.0 - delRatio));
    } else {
        return byteSize;
    }
}

bool LogMergePolicy::isOptimized(const SegmentInfosPtr& infos, int32_t maxNumSegments, SetSegmentInfo segmentsToOptimize) {
    int32_t numSegments = infos->size();
    int32_t numToOptimize = 0;
    SegmentInfoPtr optimizeInfo;
    for (int32_t i = 0; i < numSegments && numToOptimize <= maxNumSegments; ++i) {
        SegmentInfoPtr info(infos->info(i));
        if (segmentsToOptimize.contains(info)) {
            ++numToOptimize;
            optimizeInfo = info;
        }
    }
    return (numToOptimize <= maxNumSegments && (numToOptimize != 1 || isOptimized(optimizeInfo)));
}

bool LogMergePolicy::isOptimized(const SegmentInfoPtr& info) {
    IndexWriterPtr writer(_writer);
    bool hasDeletions = (writer->numDeletedDocs(info) > 0);
    return (!hasDeletions && !info->hasSeparateNorms() && info->dir == writer->getDirectory() && (info->getUseCompoundFile() == _useCompoundFile || noCFSRatio < 1.0));
}

MergeSpecificationPtr LogMergePolicy::findMergesForOptimize(const SegmentInfosPtr& segmentInfos, int32_t maxSegmentCount, SetSegmentInfo segmentsToOptimize) {
    MergeSpecificationPtr spec;

    BOOST_ASSERT(maxSegmentCount > 0);

    if (!isOptimized(segmentInfos, maxSegmentCount, segmentsToOptimize)) {
        // Find the newest (rightmost) segment that needs to be optimized (other segments may have been
        // flushed since optimize started)
        int32_t last = segmentInfos->size();
        while (last > 0) {
            if (segmentsToOptimize.contains(segmentInfos->info(--last))) {
                ++last;
                break;
            }
        }

        if (last > 0) {
            spec = newLucene<MergeSpecification>();

            // First, enroll all "full" merges (size mergeFactor) to potentially be run concurrently
            while (last - maxSegmentCount + 1 >= mergeFactor) {
                spec->add(makeOneMerge(segmentInfos, segmentInfos->range(last - mergeFactor, last)));
                last -= mergeFactor;
            }

            // Only if there are no full merges pending do we add a final partial (< mergeFactor segments) merge
            if (spec->merges.empty()) {
                if (maxSegmentCount == 1) {
                    // Since we must optimize down to 1 segment, the choice is simple
                    if (last > 1 || !isOptimized(segmentInfos->info(0))) {
                        spec->add(makeOneMerge(segmentInfos, segmentInfos->range(0, last)));
                    }
                } else if (last > maxSegmentCount) {
                    // Take care to pick a partial merge that is least cost, but does not make the index too
                    // lopsided.  If we always just picked the partial tail then we could produce a highly
                    // lopsided index over time

                    // We must merge this many segments to leave maxNumSegments in the index (from when
                    // optimize was first kicked off)
                    int32_t finalMergeSize = last - maxSegmentCount + 1;

                    // Consider all possible starting points
                    int64_t bestSize = 0;
                    int32_t bestStart = 0;

                    for (int32_t i = 0; i < last - finalMergeSize + 1; ++i) {
                        int64_t sumSize = 0;
                        for (int32_t j = 0; j < finalMergeSize; ++j) {
                            sumSize += size(segmentInfos->info(j + i));
                        }
                        if (i == 0 || (sumSize < 2 * size(segmentInfos->info(i - 1)) && sumSize < bestSize)) {
                            bestStart = i;
                            bestSize = sumSize;
                        }
                    }

                    spec->add(makeOneMerge(segmentInfos, segmentInfos->range(bestStart, bestStart + finalMergeSize)));
                }
            }
        } else {
            spec.reset();
        }
    } else {
        spec.reset();
    }

    return spec;
}

MergeSpecificationPtr LogMergePolicy::findMergesToExpungeDeletes(const SegmentInfosPtr& segmentInfos) {
    int32_t numSegments = segmentInfos->size();

    message(L"findMergesToExpungeDeletes: " + StringUtils::toString(numSegments) + L" segments");

    MergeSpecificationPtr spec(newLucene<MergeSpecification>());
    int32_t firstSegmentWithDeletions = -1;
    for (int32_t i = 0; i < numSegments; ++i) {
        SegmentInfoPtr info(segmentInfos->info(i));
        int32_t delCount = IndexWriterPtr(_writer)->numDeletedDocs(info);
        if (delCount > 0) {
            message(L"  segment " + info->name + L" has deletions");
            if (firstSegmentWithDeletions == -1) {
                firstSegmentWithDeletions = i;
            } else if (i - firstSegmentWithDeletions == mergeFactor) {
                // We've seen mergeFactor segments in a row with deletions, so force a merge now
                message(L"  add merge " + StringUtils::toString(firstSegmentWithDeletions) + L" to " + StringUtils::toString(i - 1) + L" inclusive");
                spec->add(makeOneMerge(segmentInfos, segmentInfos->range(firstSegmentWithDeletions, i)));
                firstSegmentWithDeletions = i;
            }
        } else if (firstSegmentWithDeletions != -1) {
            // End of a sequence of segments with deletions, so merge those past segments even if
            // it's fewer than mergeFactor segments
            message(L"  add merge " + StringUtils::toString(firstSegmentWithDeletions) + L" to " + StringUtils::toString(i - 1) + L" inclusive");
            spec->add(makeOneMerge(segmentInfos, segmentInfos->range(firstSegmentWithDeletions, i)));
            firstSegmentWithDeletions = -1;
        }
    }

    if (firstSegmentWithDeletions != -1) {
        message(L"  add merge " + StringUtils::toString(firstSegmentWithDeletions) + L" to " + StringUtils::toString(numSegments - 1) + L" inclusive");
        spec->add(makeOneMerge(segmentInfos, segmentInfos->range(firstSegmentWithDeletions, numSegments)));
    }

    return spec;
}

MergeSpecificationPtr LogMergePolicy::findMerges(const SegmentInfosPtr& segmentInfos) {
    int32_t numSegments = segmentInfos->size();
    message(L"findMerges: " + StringUtils::toString(numSegments) + L" segments");

    // Compute levels, which is just log (base mergeFactor) of the size of each segment
    Collection<double> levels(Collection<double>::newInstance(numSegments));
    double norm = std::log((double)mergeFactor);

    for (int32_t i = 0; i < numSegments; ++i) {
        SegmentInfoPtr info(segmentInfos->info(i));
        int64_t _size = size(info);

        // Floor tiny segments
        _size = std::max(_size, (int64_t)1);
        levels[i] = std::log((double)_size) / norm;
    }

    double levelFloor = minMergeSize <= 0 ? 0 : (std::log((double)minMergeSize) / norm);

    // Now, we quantize the log values into levels.  The first level is any segment whose log
    // size is within LEVEL_LOG_SPAN of the max size, or, who has such as segment "to the right".
    // Then, we find the max of all other segments and use that to define the next level segment, etc.

    MergeSpecificationPtr spec;

    int32_t start = 0;
    while (start < numSegments) {
        // Find max level of all segments not already quantized
        double maxLevel = levels[start];
        for (int32_t i = 1 + start; i < numSegments; ++i) {
            maxLevel = std::max(maxLevel, levels[i]);
        }

        // Now search backwards for the rightmost segment that falls into this level
        double levelBottom;
        if (maxLevel < levelFloor) {
            levelBottom = -1.0;
        } else {
            levelBottom = (double)(maxLevel - LEVEL_LOG_SPAN);

            // Force a boundary at the level floor
            if (levelBottom < levelFloor && maxLevel >= levelFloor) {
                levelBottom = levelFloor;
            }
        }

        int32_t upto = numSegments - 1;
        while (upto >= start) {
            if (levels[upto] >= levelBottom) {
                break;
            }
            --upto;
        }
        message(L"  level " + StringUtils::toString(levelBottom) + L" to " + StringUtils::toString(maxLevel) + L": " + StringUtils::toString(1 + upto - start) + L" segments");

        // Finally, record all merges that are viable at this level
        int32_t end = start + mergeFactor;
        while (end <= 1 + upto) {
            bool anyTooLarge = false;
            for (int32_t i = start; i < end; ++i) {
                SegmentInfoPtr info(segmentInfos->info(i));
                if (size(info) >= maxMergeSize || sizeDocs(info) >= maxMergeDocs) {
                    anyTooLarge = true;
                    break;
                }
            }

            if (!anyTooLarge) {
                if (!spec) {
                    spec = newLucene<MergeSpecification>();
                }
                message(L"    " + StringUtils::toString(start) + L" to " + StringUtils::toString(end) + L": add this merge");
                spec->add(makeOneMerge(segmentInfos, segmentInfos->range(start, end)));
            } else {
                message(L"    " + StringUtils::toString(start) + L" to " + StringUtils::toString(end) + L": contains segment over maxMergeSize or maxMergeDocs; skipping");
            }

            start = end;
            end = start + mergeFactor;
        }

        start = 1 + upto;
    }

    return spec;
}

OneMergePtr LogMergePolicy::makeOneMerge(const SegmentInfosPtr& infos, const SegmentInfosPtr& infosToMerge) {
    bool doCFS;
    if (!_useCompoundFile) {
        doCFS = false;
    } else if (noCFSRatio == 1.0) {
        doCFS = true;
    } else {
        int64_t totSize = 0;
        int32_t numInfos = infos->size();
        for (int32_t i = 0; i < numInfos; ++i) {
            SegmentInfoPtr info(infos->info(i));
            totSize += size(info);
        }
        int64_t mergeSize = 0;
        int32_t numMerges = infosToMerge->size();
        for (int32_t i = 0; i < numMerges; ++i) {
            SegmentInfoPtr info(infosToMerge->info(i));
            mergeSize += size(info);
        }
        doCFS = mergeSize <= noCFSRatio * totSize;
    }
    return newLucene<OneMerge>(infosToMerge, doCFS);
}

void LogMergePolicy::setMaxMergeDocs(int32_t maxMergeDocs) {
    this->maxMergeDocs = maxMergeDocs;
}

int32_t LogMergePolicy::getMaxMergeDocs() {
    return maxMergeDocs;
}

}
