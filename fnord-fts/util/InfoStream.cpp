/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/fts.h"
#include <iostream>
#include "fnord-fts/util/InfoStream.h"
#include "fnord-fts/util/StringUtils.h"

namespace fnord {
namespace fts {

InfoStream::InfoStream() {
}

InfoStream::~InfoStream() {
}

InfoStreamFile::InfoStreamFile(const String& path) : file(path) {
}

InfoStreamFile::~InfoStreamFile() {
}

InfoStreamFile& InfoStreamFile::operator<< (const String& t) {
    file << t;
    return *this;
}

InfoStreamOut::~InfoStreamOut() {
}

InfoStreamOut& InfoStreamOut::operator<< (const String& t) {
    std::wcout << t;
    return *this;
}

InfoStreamNull::~InfoStreamNull() {
}

InfoStreamNull& InfoStreamNull::operator<< (const String& t) {
    return *this;
}

}

}
