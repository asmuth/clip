/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#ifndef WINVER                  // Specifies that the minimum required platform is Windows XP.
#define WINVER 0x0501           // Change this to the appropriate value to target other versions of Windows.
#endif
#ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows XP.
#define _WIN32_WINNT 0x0501     // Change this to the appropriate value to target other versions of Windows.
#endif
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#define BOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS
#define BOOST_FILESYSTEM_VERSION 3

/**
 * Fix for Boost Bug #2792
 *   HP aCC rejects string_parse_tree <> in string_parse_tree.hpp
 *   https://svn.boost.org/trac/boost/ticket/2792
 */
#include "fnord-fts/util/boost_string_parse_tree_fix.h"

#include "fnord-fts/Lucene.h"
