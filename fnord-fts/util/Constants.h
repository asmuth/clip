/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "fnord-fts/Lucene.h"

namespace Lucene {

/// Some useful Lucene constants.
class Constants {
private:
    Constants();

public:
    virtual ~Constants();

public:
    static String OS_NAME;
    static String LUCENE_MAIN_VERSION;
    static String LUCENE_VERSION;
};

/// Use by certain classes to match version compatibility across releases of Lucene.
///
/// WARNING: When changing the version parameter that you supply to components in Lucene, do not simply
/// change the version at search-time, but instead also adjust your indexing code to match, and re-index.
class LuceneVersion {
private:
    LuceneVersion();

public:
    virtual ~LuceneVersion();

public:
    enum Version {
        /// Match settings and bugs in Lucene's 2.0 release.
        LUCENE_20 = 0,

        /// Match settings and bugs in Lucene's 2.1 release.
        LUCENE_21,

        /// Match settings and bugs in Lucene's 2.2 release.
        LUCENE_22,

        /// Match settings and bugs in Lucene's 2.3 release.
        LUCENE_23,

        /// Match settings and bugs in Lucene's 2.4 release.
        LUCENE_24,

        /// Match settings and bugs in Lucene's 2.9 release.
        LUCENE_29,

        /// Match settings and bugs in Lucene's 3.0 release.
        ///
        /// Use this to get the latest &amp; greatest settings, bug fixes, etc, for Lucene.
        LUCENE_30,

        /// Add new constants for later versions **here** to respect order!

        /// Warning: If you use this setting, and then upgrade to a newer release of Lucene,
        /// sizable changes may happen.  If backwards compatibility is important then you
        /// should instead explicitly specify an actual version.
        ///
        /// If you use this constant then you  may need to re-index all of your documents
        /// when upgrading Lucene, as the way text is indexed may have changed.  Additionally,
        /// you may need to re-test your entire application to ensure it behaves as
        /// expected, as some defaults may have changed and may break functionality in your
        /// application.
        ///
        /// Deprecated: Use an actual version instead.
        LUCENE_CURRENT
    };

public:
    static bool onOrAfter(LuceneVersion::Version first, LuceneVersion::Version second);
};

}

#endif
