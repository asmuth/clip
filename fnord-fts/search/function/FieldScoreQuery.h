/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FIELDSCOREQUERY_H
#define FIELDSCOREQUERY_H

#include "fnord-fts/search/function/ValueSourceQuery.h"

namespace fnord {
namespace fts {

/// A query that scores each document as the value of the numeric input field.
///
/// The query matches all documents, and scores each document according to the numeric value of that field.
///
/// It is assumed, and expected, that:
/// <ul>
/// <li>The field used here is indexed, and has exactly one token in every scored document.
/// <li>Best if this field is un_tokenized.
/// <li>That token is parseable to the selected type.
/// </ul>
///
/// Combining this query in a FunctionQuery allows much freedom in affecting document scores.  Note, that
/// with this freedom comes responsibility: it is more than likely that the default Lucene scoring is superior
/// in quality to scoring modified as explained here.  However, in some cases, and certainly for research
/// experiments, this capability may turn useful.
///
/// When constructing this query, select the appropriate type. That type should match the data stored in the
/// field.  So in fact the "right" type should be selected before indexing. Type selection has effect on the
/// RAM usage:
/// <ul>
/// <li>Byte consumes 1 * maxDocs bytes.
/// <li>Int consumes 4 * maxDocs bytes.
/// <li>Double consumes 8 * maxDocs bytes.
/// </ul>
///
/// Caching:  Values for the numeric field are loaded once and cached in memory for further use with the same
/// IndexReader.  To take advantage of this, it is extremely important to reuse index-readers or index-
/// searchers, otherwise, for instance if for each query a new index reader is opened, large penalties would
/// be paid for loading the field values into memory over and over again.
class FieldScoreQuery : public ValueSourceQuery {
public:
    /// Type of score field, indicating how field values are interpreted/parsed.
    enum Type {
        /// Field values are interpreted as numeric byte values.
        BYTE,

        /// Field values are interpreted as numeric integer values.
        INT,

        /// Field values are interpreted as numeric double values.
        DOUBLE
    };

    /// Create a FieldScoreQuery - a query that scores each document as the value of the numeric input field.
    /// The type param tells how to parse the field string values into a numeric score value.
    /// @param field the numeric field to be used.
    /// @param type the type of the field.
    FieldScoreQuery(const String& field, Type type);

    virtual ~FieldScoreQuery();

    LUCENE_CLASS(FieldScoreQuery);

public:
    /// Create the appropriate (cached) field value source.
    static ValueSourcePtr getValueSource(const String& field, Type type);
};

}

}
#endif
