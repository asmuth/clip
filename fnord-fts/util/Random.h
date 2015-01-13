/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef RANDOM_H
#define RANDOM_H

#include "fnord-fts/util/LuceneObject.h"

namespace Lucene {

/// Utility class to generate a stream of pseudorandom numbers.
class Random : public LuceneObject {
public:
    Random();
    Random(int64_t seed);

    virtual ~Random();

protected:
    int64_t seed;

public:
    void setSeed(int64_t seed);
    int32_t nextInt(int32_t limit = INT_MAX);
    double nextDouble();

protected:
    int32_t next(int32_t bits);
};

}

#endif
