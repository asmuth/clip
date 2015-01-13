/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/LuceneInc.h"
#include "fnord-fts/util/Constants.h"

namespace Lucene {

#if defined(linux) || defined(__linux) || defined(__linux__)
String Constants::OS_NAME = L"Linux";
#elif defined(sun) || defined(__sun)
String Constants::OS_NAME = L"Sun";
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(_WIN64) || defined(__WIN64__) || defined(WIN64)
String Constants::OS_NAME = L"Windows";
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
String Constants::OS_NAME = L"Mac";
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__) || defined(__FreeBSD_kernel__)
String Constants::OS_NAME = L"BSD";
#elif defined(__GNU__)
String Constants::OS_NAME = L"HURD";
#else
String Constants::OS_NAME = L"UNKNOWN";
#endif

String Constants::LUCENE_MAIN_VERSION = L"3.0.6";
String Constants::LUCENE_VERSION = L"3.0.6";

Constants::Constants() {
    // private
}

Constants::~Constants() {
}

LuceneVersion::LuceneVersion() {
    // private
}

LuceneVersion::~LuceneVersion() {
}

bool LuceneVersion::onOrAfter(LuceneVersion::Version first, LuceneVersion::Version second) {
    return (first >= second);
}

}
