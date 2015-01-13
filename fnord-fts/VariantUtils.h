/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef VARIANTUTILS_H
#define VARIANTUTILS_H

#include <boost/any.hpp>
#include "fnord-fts/Lucene.h"
#include "fnord-fts/MiscUtils.h"

namespace Lucene {

class VariantUtils {
public:
    template <typename TYPE>
    static TYPE get(const boost::any& var) {
        return var.type() == typeid(TYPE) ? boost::any_cast<TYPE>(var) : TYPE();
    }

    template <typename TYPE, typename VAR>
    static TYPE get(VAR var) {
        return var.type() == typeid(TYPE) ? boost::get<TYPE>(var) : TYPE();
    }

    template <typename TYPE, typename VAR>
    static bool typeOf(VAR var) {
        return (var.type() == typeid(TYPE));
    }

    static VariantNull null() {
        return VariantNull();
    }

    static bool isNull(const boost::any& var) {
        return var.empty();
    }

    template <typename VAR>
    static bool isNull(VAR var) {
        return typeOf<VariantNull>(var);
    }

    template <typename VAR>
    static int32_t hashCode(VAR var) {
        if (typeOf<String>(var)) {
            return StringUtils::hashCode(get<String>(var));
        }
        if (typeOf<int32_t>(var)) {
            return get<int32_t>(var);
        }
        if (typeOf<int64_t>(var)) {
            return (int32_t)get<int64_t>(var);
        }
        if (typeOf<double>(var)) {
            int64_t longBits = MiscUtils::doubleToLongBits(get<double>(var));
            return (int32_t)(longBits ^ (longBits >> 32));
        }
        if (typeOf< Collection<uint8_t> >(var)) {
            return get< Collection<uint8_t> >(var).hashCode();
        }
        if (typeOf< Collection<int32_t> >(var)) {
            return get< Collection<int32_t> >(var).hashCode();
        }
        if (typeOf< Collection<int64_t> >(var)) {
            return get< Collection<int64_t> >(var).hashCode();
        }
        if (typeOf< Collection<double> >(var)) {
            return get< Collection<double> >(var).hashCode();
        }
        if (typeOf< Collection<String> >(var)) {
            return get< Collection<String> >(var).hashCode();
        }
        if (typeOf<LuceneObjectPtr>(var)) {
            return get<LuceneObjectPtr>(var)->hashCode();
        }
        return 0;
    }

    template <typename FIRST, typename SECOND>
    static bool equalsType(FIRST first, SECOND second) {
        return (first.type() == second.type());
    }

    template <typename FIRST, typename SECOND>
    static bool equals(FIRST first, SECOND second) {
        return first.type() == second.type() ? (first == second) : false;
    }

    template <typename VAR>
    static int32_t compareTo(VAR first, VAR second) {
        return first < second ? -1 : (first == second ? 0 : 1);
    }
};

}

#endif
