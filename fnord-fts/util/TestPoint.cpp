/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "TestPoint.h"

namespace Lucene {

MapStringInt TestPoint::testMethods = MapStringInt::newInstance();
bool TestPoint::enable = false;

TestPoint::~TestPoint() {
}

void TestPoint::enableTestPoints() {
    enable = true;
}

void TestPoint::clear() {
    SyncLock syncLock(&testMethods);
    testMethods.clear();
}

void TestPoint::setTestPoint(const String& object, const String& method, bool point) {
    if (enable) {
        SyncLock syncLock(&testMethods);
        testMethods.put(object + L":" + method, point);
        testMethods.put(method, point);
    }
}

bool TestPoint::getTestPoint(const String& object, const String& method) {
    SyncLock syncLock(&testMethods);
    MapStringInt::iterator testMethod = testMethods.find(object + L":" + method);
    return testMethod == testMethods.end() ? false : (testMethod->second != 0);
}

bool TestPoint::getTestPoint(const String& method) {
    SyncLock syncLock(&testMethods);
    MapStringInt::iterator testMethod = testMethods.find(method);
    return testMethod == testMethods.end() ? false : (testMethod->second != 0);
}

TestScope::TestScope(const String& object, const String& method) {
    this->object = object;
    this->method = method;
    TestPoint::setTestPoint(object, method, true);
}

TestScope::~TestScope() {
    TestPoint::setTestPoint(object, method, false);
}

}
