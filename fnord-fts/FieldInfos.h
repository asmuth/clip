/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FIELDINFOS_H
#define FIELDINFOS_H

#include "LuceneObject.h"

namespace Lucene {

/// Access to the Fieldable Info file that describes document fields and whether or not they are indexed.
/// Each segment has a separate Fieldable Info file. Objects of this class are thread-safe for multiple
/// readers, but only one thread can be adding documents at a time, with no other reader or writer threads
/// accessing this object.
class FieldInfos : public LuceneObject {
public:
    FieldInfos();

    /// Construct a FieldInfos object using the directory and the name of the file IndexInput
    /// @param d The directory to open the IndexInput from
    /// @param name The name of the file to open the IndexInput from in the Directory
    FieldInfos(const DirectoryPtr& d, const String& name);

    virtual ~FieldInfos();

    LUCENE_CLASS(FieldInfos);

public:
    // Used internally (ie not written to *.fnm files) for pre-2.9 files
    static const int32_t FORMAT_PRE;

    // First used in 2.9; prior to 2.9 there was no format header
    static const int32_t FORMAT_START;

    static const int32_t CURRENT_FORMAT;

    static const uint8_t IS_INDEXED;
    static const uint8_t STORE_TERMVECTOR;
    static const uint8_t STORE_POSITIONS_WITH_TERMVECTOR;
    static const uint8_t STORE_OFFSET_WITH_TERMVECTOR;
    static const uint8_t OMIT_NORMS;
    static const uint8_t STORE_PAYLOADS;
    static const uint8_t OMIT_TERM_FREQ_AND_POSITIONS;

protected:
    Collection<FieldInfoPtr> byNumber;
    MapStringFieldInfo byName;
    int32_t format;

public:
    /// Returns a deep clone of this FieldInfos instance.
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());

    /// Adds field info for a Document.
    void add(const DocumentPtr& doc);

    /// Returns true if any fields do not omitTermFreqAndPositions
    bool hasProx();

    /// Add fields that are indexed. Whether they have termvectors has to be specified.
    /// @param names The names of the fields
    /// @param storeTermVectors Whether the fields store term vectors or not
    /// @param storePositionWithTermVector true if positions should be stored.
    /// @param storeOffsetWithTermVector true if offsets should be stored
    void addIndexed(HashSet<String> names, bool storeTermVectors, bool storePositionWithTermVector, bool storeOffsetWithTermVector);

    /// Assumes the fields are not storing term vectors.
    /// @param names The names of the fields
    /// @param isIndexed Whether the fields are indexed or not
    /// @see #add(const String&, bool)
    void add(HashSet<String> names, bool isIndexed);

    /// Calls 5 parameter add with false for all TermVector parameters.
    /// @param name The name of the Fieldable
    /// @param isIndexed true if the field is indexed
    /// @see #add(const String&, bool, bool, bool, bool)
    void add(const String& name, bool isIndexed);

    /// Calls 5 parameter add with false for term vector positions and offsets.
    /// @param name The name of the field
    /// @param isIndexed  true if the field is indexed
    /// @param storeTermVector true if the term vector should be stored
    void add(const String& name, bool isIndexed, bool storeTermVector);

    /// If the field is not yet known, adds it. If it is known, checks to make sure that the isIndexed flag
    /// is the same as was given previously for this field. If not - marks it as being indexed.  Same goes
    /// for the TermVector parameters.
    /// @param name The name of the field
    /// @param isIndexed true if the field is indexed
    /// @param storeTermVector true if the term vector should be stored
    /// @param storePositionWithTermVector true if the term vector with positions should be stored
    /// @param storeOffsetWithTermVector true if the term vector with offsets should be stored
    void add(const String& name, bool isIndexed, bool storeTermVector, bool storePositionWithTermVector, bool storeOffsetWithTermVector);

    /// If the field is not yet known, adds it. If it is known, checks to make sure that the isIndexed flag
    /// is the same as was given previously for this field. If not - marks it as being indexed.  Same goes
    /// for the TermVector parameters.
    /// @param name The name of the field
    /// @param isIndexed true if the field is indexed
    /// @param storeTermVector true if the term vector should be stored
    /// @param storePositionWithTermVector true if the term vector with positions should be stored
    /// @param storeOffsetWithTermVector true if the term vector with offsets should be stored
    /// @param omitNorms true if the norms for the indexed field should be omitted
    void add(const String& name, bool isIndexed, bool storeTermVector, bool storePositionWithTermVector,
             bool storeOffsetWithTermVector, bool omitNorms);

    /// If the field is not yet known, adds it. If it is known, checks to make sure that the isIndexed
    /// flag is the same as was given previously for this field. If not - marks it as being indexed.
    /// Same goes for the TermVector parameters.
    /// @param name The name of the field
    /// @param isIndexed true if the field is indexed
    /// @param storeTermVector true if the term vector should be stored
    /// @param storePositionWithTermVector true if the term vector with positions should be stored
    /// @param storeOffsetWithTermVector true if the term vector with offsets should be stored
    /// @param omitNorms true if the norms for the indexed field should be omitted
    /// @param storePayloads true if payloads should be stored for this field
    /// @param omitTermFreqAndPositions true if term freqs should be omitted for this field
    FieldInfoPtr add(const String& name, bool isIndexed, bool storeTermVector, bool storePositionWithTermVector,
                     bool storeOffsetWithTermVector, bool omitNorms, bool storePayloads, bool omitTermFreqAndPositions);

    int32_t fieldNumber(const String& fieldName);
    FieldInfoPtr fieldInfo(const String& fieldName);

    /// Return the fieldName identified by its number.
    /// @return the fieldName or an empty string when the field with the given number doesn't exist.
    String fieldName(int32_t fieldNumber);

    /// Return the fieldinfo object referenced by the fieldNumber.
    /// @return the FieldInfo object or null when the given fieldNumber doesn't exist.
    FieldInfoPtr fieldInfo(int32_t fieldNumber);

    int32_t size();

    bool hasVectors();

    void write(const DirectoryPtr& d, const String& name);
    void write(const IndexOutputPtr& output);

protected:
    FieldInfoPtr addInternal(const String& name, bool isIndexed, bool storeTermVector, bool storePositionWithTermVector,
                             bool storeOffsetWithTermVector, bool omitNorms, bool storePayloads, bool omitTermFreqAndPositions);

    void read(const IndexInputPtr& input, const String& fileName);
};

}

#endif
