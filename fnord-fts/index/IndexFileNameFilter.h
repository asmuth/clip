/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef INDEXFILENAMEFILTER_H
#define INDEXFILENAMEFILTER_H

#include "fnord-fts/util/LuceneObject.h"

namespace Lucene {

/// Filename filter that accept filenames and extensions only created by Lucene.
class IndexFileNameFilter : public LuceneObject {
public:
    /// Returns true if this is a file known to be a Lucene index file.
    static bool accept(const String& directory, const String& name);

    /// Returns true if this is a file that would be contained in a CFS file.
    /// This function should only be called on files that pass the
    /// {@link #accept} (ie, are already known to be a Lucene index file).
    static bool isCFSFile(const String& name);

    /// Return singleton IndexFileNameFilter
    static IndexFileNameFilterPtr getFilter();
};

}

#endif
