/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef TESTPOINT_H
#define TESTPOINT_H

#include "Lucene.h"

namespace Lucene {

/// Used for unit testing as a substitute for stack trace
class TestPoint {
public:
    virtual ~TestPoint();

protected:
    static MapStringInt testMethods;
    static bool enable;

public:
    static void enableTestPoints();
    static void clear();
    static void setTestPoint(const String& object, const String& method, bool point);
    static bool getTestPoint(const String& object, const String& method);
    static bool getTestPoint(const String& method);
};

class TestScope {
public:
    TestScope(const String& object, const String& method);
    virtual ~TestScope();

protected:
    String object;
    String method;
};

}

#endif
