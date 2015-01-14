/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef DOCVALUES_H
#define DOCVALUES_H

#include "fnord-fts/util/LuceneObject.h"

namespace fnord {
namespace fts {

/// Represents field values as different types.  Normally created via a {@link ValueSuorce} for a
/// particular field and reader.
///
/// DocValues is distinct from ValueSource because there needs to be an object created at query
/// evaluation time that is not referenced by the query itself because:
///  - Query objects should be MT safe
///  - For caching, Query objects are often used as keys... you don't want the Query carrying around
///    big objects
class DocValues : public LuceneObject {
public:
    DocValues();
    virtual ~DocValues();

    LUCENE_CLASS(DocValues);

protected:
    double minVal;
    double maxVal;
    double avgVal;
    bool computed;

public:
    using LuceneObject::toString;

    /// Return doc value as a double.
    /// Mandatory: every DocValues implementation must implement at least this method.
    /// @param doc document whose double value is requested.
    virtual double doubleVal(int32_t doc) = 0;

    /// Return doc value as an int.
    /// Optional: DocValues implementation can (but don't have to) override this method.
    /// @param doc document whose int value is requested.
    virtual int32_t intVal(int32_t doc);

    /// Return doc value as a long.
    /// Optional: DocValues implementation can (but don't have to) override this method.
    /// @param doc document whose long value is requested.
    virtual int64_t longVal(int32_t doc);

    /// Return doc value as a string.
    /// Optional: DocValues implementation can (but don't have to) override this method.
    /// @param doc document whose string value is requested.
    virtual String strVal(int32_t doc);

    /// Return a string representation of a doc value, as required for Explanations.
    virtual String toString(int32_t doc) = 0;

    /// Explain the scoring value for the input doc.
    virtual ExplanationPtr explain(int32_t doc);

    /// For test purposes only, return the inner array of values, or null if not applicable.
    ///
    /// Allows tests to verify that loaded values are:
    /// <ol>
    /// <li>indeed cached/reused.
    /// <li>stored in the expected size/type (byte/short/int/float).
    /// </ol>
    ///
    /// Note: implementations of DocValues must override this method for these test elements to be tested,
    /// Otherwise the test would not fail, just print a warning.
    virtual CollectionValue getInnerArray();

    /// Returns the minimum of all values or NaN if this DocValues instance does not contain any value.
    /// This operation is optional
    /// @return the minimum of all values or NaN if this DocValues instance does not contain any value.
    virtual double getMinValue();

    /// Returns the maximum of all values or NaN if this DocValues instance does not contain any value.
    /// This operation is optional
    /// @return the maximum of all values or NaN if this DocValues instance does not contain any value.
    virtual double getMaxValue();

    /// Returns the average of all values or NaN if this DocValues instance does not contain any value.
    /// This operation is optional
    /// @return the average of all values or NaN if this DocValues instance does not contain any value.
    virtual double getAverageValue();

protected:
    /// Compute optional values
    void compute();
};

}

}
#endif
