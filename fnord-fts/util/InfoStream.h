/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef INFOSTREAM_H
#define INFOSTREAM_H

#include "fnord-fts/util/LuceneObject.h"
#include <boost/filesystem/fstream.hpp>

namespace fnord {
namespace fts {

/// Utility class to support streaming info messages.
class InfoStream : public LuceneObject {
protected:
    InfoStream();

public:
    virtual ~InfoStream();
    LUCENE_CLASS(InfoStream);

public:
    virtual InfoStream& operator<< (const String& t) = 0;
};

/// Stream override to write messages to a file.
class InfoStreamFile : public InfoStream {
public:
    InfoStreamFile(const String& path);
    virtual ~InfoStreamFile();

    LUCENE_CLASS(InfoStreamFile);

protected:
    boost::filesystem::wofstream file;

public:
    virtual InfoStreamFile& operator<< (const String& t);
};

/// Stream override to write messages to a std::cout.
class InfoStreamOut : public InfoStream {
public:
    virtual ~InfoStreamOut();
    LUCENE_CLASS(InfoStreamOut);

public:
    virtual InfoStreamOut& operator<< (const String& t);
};

/// Null stream override to eat messages.
class InfoStreamNull : public InfoStream {
public:
    virtual ~InfoStreamNull();
    LUCENE_CLASS(InfoStreamNull);

public:
    virtual InfoStreamNull& operator<< (const String& t);
};

}

}
#endif
