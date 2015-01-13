/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include <boost/algorithm/string.hpp>
#include "IndexFileNames.h"
#include "SegmentInfo.h"
#include "StringUtils.h"

namespace Lucene {

IndexFileNames::~IndexFileNames() {
}

const String& IndexFileNames::SEGMENTS() {
    static String _SEGMENTS(L"segments");
    return _SEGMENTS;
}

const String& IndexFileNames::SEGMENTS_GEN() {
    static String _SEGMENTS_GEN(L"segments.gen");
    return _SEGMENTS_GEN;
}

const String& IndexFileNames::DELETABLE() {
    static String _DELETABLE(L"deletable");
    return _DELETABLE;
}

const String& IndexFileNames::NORMS_EXTENSION() {
    static String _NORMS_EXTENSION(L"nrm");
    return _NORMS_EXTENSION;
}

const String& IndexFileNames::FREQ_EXTENSION() {
    static String _FREQ_EXTENSION(L"frq");
    return _FREQ_EXTENSION;
}

const String& IndexFileNames::PROX_EXTENSION() {
    static String _PROX_EXTENSION(L"prx");
    return _PROX_EXTENSION;
}

const String& IndexFileNames::TERMS_EXTENSION() {
    static String _TERMS_EXTENSION(L"tis");
    return _TERMS_EXTENSION;
}

const String& IndexFileNames::TERMS_INDEX_EXTENSION() {
    static String _TERMS_INDEX_EXTENSION(L"tii");
    return _TERMS_INDEX_EXTENSION;
}

const String& IndexFileNames::FIELDS_INDEX_EXTENSION() {
    static String _FIELDS_INDEX_EXTENSION(L"fdx");
    return _FIELDS_INDEX_EXTENSION;
}

const String& IndexFileNames::FIELDS_EXTENSION() {
    static String _FIELDS_EXTENSION(L"fdt");
    return _FIELDS_EXTENSION;
}

const String& IndexFileNames::VECTORS_FIELDS_EXTENSION() {
    static String _VECTORS_FIELDS_EXTENSION(L"tvf");
    return _VECTORS_FIELDS_EXTENSION;
}

const String& IndexFileNames::VECTORS_DOCUMENTS_EXTENSION() {
    static String _VECTORS_DOCUMENTS_EXTENSION(L"tvd");
    return _VECTORS_DOCUMENTS_EXTENSION;
}

const String& IndexFileNames::VECTORS_INDEX_EXTENSION() {
    static String _VECTORS_INDEX_EXTENSION(L"tvx");
    return _VECTORS_INDEX_EXTENSION;
}

const String& IndexFileNames::COMPOUND_FILE_EXTENSION() {
    static String _COMPOUND_FILE_EXTENSION(L"cfs");
    return _COMPOUND_FILE_EXTENSION;
}

const String& IndexFileNames::COMPOUND_FILE_STORE_EXTENSION() {
    static String _COMPOUND_FILE_STORE_EXTENSION(L"cfx");
    return _COMPOUND_FILE_STORE_EXTENSION;
}

const String& IndexFileNames::DELETES_EXTENSION() {
    static String _DELETES_EXTENSION(L"del");
    return _DELETES_EXTENSION;
}

const String& IndexFileNames::FIELD_INFOS_EXTENSION() {
    static String _FIELD_INFOS_EXTENSION(L"fnm");
    return _FIELD_INFOS_EXTENSION;
}

const String& IndexFileNames::PLAIN_NORMS_EXTENSION() {
    static String _PLAIN_NORMS_EXTENSION(L"f");
    return _PLAIN_NORMS_EXTENSION;
}

const String& IndexFileNames::SEPARATE_NORMS_EXTENSION() {
    static String _SEPARATE_NORMS_EXTENSION(L"s");
    return _SEPARATE_NORMS_EXTENSION;
}

const String& IndexFileNames::GEN_EXTENSION() {
    static String _GEN_EXTENSION(L"gen");
    return _GEN_EXTENSION;
}

const HashSet<String> IndexFileNames::INDEX_EXTENSIONS() {
    static HashSet<String> _INDEX_EXTENSIONS;
    if (!_INDEX_EXTENSIONS) {
        _INDEX_EXTENSIONS = HashSet<String>::newInstance();
        _INDEX_EXTENSIONS.add(COMPOUND_FILE_EXTENSION());
        _INDEX_EXTENSIONS.add(FIELD_INFOS_EXTENSION());
        _INDEX_EXTENSIONS.add(FIELDS_INDEX_EXTENSION());
        _INDEX_EXTENSIONS.add(FIELDS_EXTENSION());
        _INDEX_EXTENSIONS.add(TERMS_INDEX_EXTENSION());
        _INDEX_EXTENSIONS.add(TERMS_EXTENSION());
        _INDEX_EXTENSIONS.add(FREQ_EXTENSION());
        _INDEX_EXTENSIONS.add(PROX_EXTENSION());
        _INDEX_EXTENSIONS.add(DELETES_EXTENSION());
        _INDEX_EXTENSIONS.add(VECTORS_INDEX_EXTENSION());
        _INDEX_EXTENSIONS.add(VECTORS_DOCUMENTS_EXTENSION());
        _INDEX_EXTENSIONS.add(VECTORS_FIELDS_EXTENSION());
        _INDEX_EXTENSIONS.add(GEN_EXTENSION());
        _INDEX_EXTENSIONS.add(NORMS_EXTENSION());
        _INDEX_EXTENSIONS.add(COMPOUND_FILE_STORE_EXTENSION());
    }
    return _INDEX_EXTENSIONS;
};

const HashSet<String> IndexFileNames::INDEX_EXTENSIONS_IN_COMPOUND_FILE() {
    static HashSet<String> _INDEX_EXTENSIONS_IN_COMPOUND_FILE;
    if (!_INDEX_EXTENSIONS_IN_COMPOUND_FILE) {
        _INDEX_EXTENSIONS_IN_COMPOUND_FILE = HashSet<String>::newInstance();
        _INDEX_EXTENSIONS_IN_COMPOUND_FILE.add(FIELD_INFOS_EXTENSION());
        _INDEX_EXTENSIONS_IN_COMPOUND_FILE.add(FIELDS_INDEX_EXTENSION());
        _INDEX_EXTENSIONS_IN_COMPOUND_FILE.add(FIELDS_EXTENSION());
        _INDEX_EXTENSIONS_IN_COMPOUND_FILE.add(TERMS_INDEX_EXTENSION());
        _INDEX_EXTENSIONS_IN_COMPOUND_FILE.add(TERMS_EXTENSION());
        _INDEX_EXTENSIONS_IN_COMPOUND_FILE.add(FREQ_EXTENSION());
        _INDEX_EXTENSIONS_IN_COMPOUND_FILE.add(PROX_EXTENSION());
        _INDEX_EXTENSIONS_IN_COMPOUND_FILE.add(VECTORS_INDEX_EXTENSION());
        _INDEX_EXTENSIONS_IN_COMPOUND_FILE.add(VECTORS_DOCUMENTS_EXTENSION());
        _INDEX_EXTENSIONS_IN_COMPOUND_FILE.add(VECTORS_FIELDS_EXTENSION());
        _INDEX_EXTENSIONS_IN_COMPOUND_FILE.add(NORMS_EXTENSION());
    }
    return _INDEX_EXTENSIONS_IN_COMPOUND_FILE;
};

const HashSet<String> IndexFileNames::STORE_INDEX_EXTENSIONS() {
    static HashSet<String> _STORE_INDEX_EXTENSIONS;
    if (!_STORE_INDEX_EXTENSIONS) {
        _STORE_INDEX_EXTENSIONS = HashSet<String>::newInstance();
        _STORE_INDEX_EXTENSIONS.add(VECTORS_INDEX_EXTENSION());
        _STORE_INDEX_EXTENSIONS.add(VECTORS_FIELDS_EXTENSION());
        _STORE_INDEX_EXTENSIONS.add(VECTORS_DOCUMENTS_EXTENSION());
        _STORE_INDEX_EXTENSIONS.add(FIELDS_INDEX_EXTENSION());
        _STORE_INDEX_EXTENSIONS.add(FIELDS_EXTENSION());
    }
    return _STORE_INDEX_EXTENSIONS;
};

const HashSet<String> IndexFileNames::NON_STORE_INDEX_EXTENSIONS() {
    static HashSet<String> _NON_STORE_INDEX_EXTENSIONS;
    if (!_NON_STORE_INDEX_EXTENSIONS) {
        _NON_STORE_INDEX_EXTENSIONS = HashSet<String>::newInstance();
        _NON_STORE_INDEX_EXTENSIONS.add(FIELD_INFOS_EXTENSION());
        _NON_STORE_INDEX_EXTENSIONS.add(FREQ_EXTENSION());
        _NON_STORE_INDEX_EXTENSIONS.add(PROX_EXTENSION());
        _NON_STORE_INDEX_EXTENSIONS.add(TERMS_EXTENSION());
        _NON_STORE_INDEX_EXTENSIONS.add(TERMS_INDEX_EXTENSION());
        _NON_STORE_INDEX_EXTENSIONS.add(NORMS_EXTENSION());
    }
    return _NON_STORE_INDEX_EXTENSIONS;
};

const HashSet<String> IndexFileNames::COMPOUND_EXTENSIONS() {
    static HashSet<String> _COMPOUND_EXTENSIONS;
    if (!_COMPOUND_EXTENSIONS) {
        _COMPOUND_EXTENSIONS = HashSet<String>::newInstance();
        _COMPOUND_EXTENSIONS.add(FIELD_INFOS_EXTENSION());
        _COMPOUND_EXTENSIONS.add(FREQ_EXTENSION());
        _COMPOUND_EXTENSIONS.add(PROX_EXTENSION());
        _COMPOUND_EXTENSIONS.add(FIELDS_INDEX_EXTENSION());
        _COMPOUND_EXTENSIONS.add(FIELDS_EXTENSION());
        _COMPOUND_EXTENSIONS.add(TERMS_INDEX_EXTENSION());
        _COMPOUND_EXTENSIONS.add(TERMS_EXTENSION());
    }
    return _COMPOUND_EXTENSIONS;
};

const HashSet<String> IndexFileNames::VECTOR_EXTENSIONS() {
    static HashSet<String> _VECTOR_EXTENSIONS;
    if (!_VECTOR_EXTENSIONS) {
        _VECTOR_EXTENSIONS = HashSet<String>::newInstance();
        _VECTOR_EXTENSIONS.add(VECTORS_INDEX_EXTENSION());
        _VECTOR_EXTENSIONS.add(VECTORS_DOCUMENTS_EXTENSION());
        _VECTOR_EXTENSIONS.add(VECTORS_FIELDS_EXTENSION());
    }
    return _VECTOR_EXTENSIONS;
};

String IndexFileNames::fileNameFromGeneration(const String& base, const String& extension, int64_t gen) {
    if (gen == SegmentInfo::NO) {
        return L"";
    } else if (gen == SegmentInfo::WITHOUT_GEN) {
        return base + extension;
    } else {
        return base + L"_" + StringUtils::toString(gen, StringUtils::CHARACTER_MAX_RADIX) + extension;
    }
}

bool IndexFileNames::isDocStoreFile(const String& fileName) {
    if (boost::ends_with(fileName, COMPOUND_FILE_STORE_EXTENSION())) {
        return true;
    }
    for (HashSet<String>::iterator index = STORE_INDEX_EXTENSIONS().begin(); index != STORE_INDEX_EXTENSIONS().end(); ++index) {
        if (boost::ends_with(fileName, *index)) {
            return true;
        }
    }
    return false;
}

String IndexFileNames::segmentFileName(const String& segmentName, const String& ext) {
    return segmentName + L"." + ext;
}

}
