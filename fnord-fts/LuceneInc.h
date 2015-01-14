/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>

#endif

/**
 * Fix for Boost Bug #2792
 *   HP aCC rejects string_parse_tree <> in string_parse_tree.hpp
 *   https://svn.boost.org/trac/boost/ticket/2792
 */
#include "fnord-fts/util/boost_string_parse_tree_fix.h"

#include "fnord-fts/Lucene.h"
