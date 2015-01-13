/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef INDEXFILENAMES_H
#define INDEXFILENAMES_H

#include "LuceneObject.h"

namespace Lucene {

/// Constants representing filenames and extensions used by Lucene.
class IndexFileNames : public LuceneObject {
public:
    virtual ~IndexFileNames();
    LUCENE_CLASS(IndexFileNames);

public:
    /// Name of the index segment file.
    static const String& SEGMENTS();

    /// Name of the generation reference file name.
    static const String& SEGMENTS_GEN();

    /// Name of the index deletable file (only used in pre-lockless indices).
    static const String& DELETABLE();

    /// Extension of norms file.
    static const String& NORMS_EXTENSION();

    /// Extension of freq postings file.
    static const String& FREQ_EXTENSION();

    /// Extension of prox postings file.
    static const String& PROX_EXTENSION();

    /// Extension of terms file.
    static const String& TERMS_EXTENSION();

    /// Extension of terms index file.
    static const String& TERMS_INDEX_EXTENSION();

    /// Extension of stored fields index file.
    static const String& FIELDS_INDEX_EXTENSION();

    /// Extension of stored fields file.
    static const String& FIELDS_EXTENSION();

    /// Extension of vectors fields file.
    static const String& VECTORS_FIELDS_EXTENSION();

    /// Extension of vectors documents file.
    static const String& VECTORS_DOCUMENTS_EXTENSION();

    /// Extension of vectors index file.
    static const String& VECTORS_INDEX_EXTENSION();

    /// Extension of compound file.
    static const String& COMPOUND_FILE_EXTENSION();

    /// Extension of compound file for doc store files.
    static const String& COMPOUND_FILE_STORE_EXTENSION();

    /// Extension of deletes.
    static const String& DELETES_EXTENSION();

    /// Extension of field infos.
    static const String& FIELD_INFOS_EXTENSION();

    /// Extension of plain norms.
    static const String& PLAIN_NORMS_EXTENSION();

    /// Extension of separate norms.
    static const String& SEPARATE_NORMS_EXTENSION();

    /// Extension of gen file.
    static const String& GEN_EXTENSION();

    /// This array contains all filename extensions used by Lucene's index
    /// files, with two exceptions, namely the extension made up from
    /// ".f" + number and from ".s" + number.  Also note that Lucene's
    /// "segments_N" files do not have any filename extension.
    static const HashSet<String> INDEX_EXTENSIONS();

    /// File extensions that are added to a compound file (same as
    /// {@link #INDEX_EXTENSIONS}, minus "del", "gen", "cfs").
    static const HashSet<String> INDEX_EXTENSIONS_IN_COMPOUND_FILE();

    static const HashSet<String> STORE_INDEX_EXTENSIONS();
    static const HashSet<String> NON_STORE_INDEX_EXTENSIONS();

    /// File extensions of old-style index files.
    static const HashSet<String> COMPOUND_EXTENSIONS();

    /// File extensions for term vector support.
    static const HashSet<String> VECTOR_EXTENSIONS();

    /// Computes the full file name from base, extension and generation.
    /// If the generation is {@link SegmentInfo#NO}, the file name is null.
    /// If it's {@link SegmentInfo#WITHOUT_GEN} the file name is base+extension.
    /// If it's > 0, the file name is base_generation+extension.
    static String fileNameFromGeneration(const String& base, const String& extension, int64_t gen);

    /// Returns true if the provided filename is one of the doc store files
    /// (ends with an extension in STORE_INDEX_EXTENSIONS).
    static bool isDocStoreFile(const String& fileName);

    /// Return segment file name.
    static String segmentFileName(const String& segmentName, const String& ext);
};

}

#endif
