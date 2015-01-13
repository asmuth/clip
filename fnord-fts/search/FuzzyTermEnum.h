/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef FUZZYTERMENUM_H
#define FUZZYTERMENUM_H

#include "FilteredTermEnum.h"

namespace Lucene {

/// Subclass of FilteredTermEnum for enumerating all terms that are similar to the specified filter term.
///
/// Term enumerations are always ordered by Term.compareTo().  Each term in the enumeration is greater
/// than all that precede it.
class FuzzyTermEnum : public FilteredTermEnum {
public:
    /// Constructor for enumeration of all terms from specified reader which share a prefix of length
    /// prefixLength with term and which have a fuzzy similarity > minSimilarity.
    ///
    /// After calling the constructor the enumeration is already pointing to the first valid term if
    /// such a term exists.
    /// @param reader Delivers terms.
    /// @param term Pattern term.
    /// @param minSimilarity Minimum required similarity for terms from the reader. Default value is 0.5.
    /// @param prefixLength Length of required common prefix. Default value is 0.
    FuzzyTermEnum(const IndexReaderPtr& reader, const TermPtr& term, double minSimilarity, int32_t prefixLength);
    FuzzyTermEnum(const IndexReaderPtr& reader, const TermPtr& term, double minSimilarity);
    FuzzyTermEnum(const IndexReaderPtr& reader, const TermPtr& term);

    virtual ~FuzzyTermEnum();

    LUCENE_CLASS(FuzzyTermEnum);

protected:
    /// Allows us save time required to create a new array every time similarity is called.
    Collection<int32_t> p;
    Collection<int32_t> d;

    double _similarity;
    bool _endEnum;

    TermPtr searchTerm;
    String field;
    String text;
    String prefix;

    double minimumSimilarity;
    double scale_factor;

public:
    virtual double difference();
    virtual bool endEnum();
    virtual void close();

protected:
    void ConstructTermEnum(const IndexReaderPtr& reader, const TermPtr& term, double minSimilarity, int32_t prefixLength);

    /// The termCompare method in FuzzyTermEnum uses Levenshtein distance to calculate the distance between
    /// the given term and the comparing term.
    virtual bool termCompare(const TermPtr& term);

    ///
    /// Compute Levenshtein distance
    ///
    /// Similarity returns a number that is 1.0f or less (including negative numbers) based on how similar the
    /// Term is compared to a target term.  It returns exactly 0.0 when
    /// <pre>
    /// editDistance > maximumEditDistance
    /// </pre>
    ///
    /// Otherwise it returns:
    /// <pre>
    /// 1 - (editDistance / length)
    /// </pre>
    /// where length is the length of the shortest term (text or target) including a prefix that are identical
    /// and editDistance is the Levenshtein distance for the two words.
    ///
    /// Embedded within this algorithm is a fail-fast Levenshtein distance algorithm.  The fail-fast algorithm
    /// differs from the standard Levenshtein distance algorithm in that it is aborted if it is discovered that
    /// the minimum distance between the words is greater than some threshold.
    ///
    /// To calculate the maximum distance threshold we use the following formula:
    /// <pre>
    /// (1 - minimumSimilarity) * length
    /// </pre>
    /// where length is the shortest term including any prefix that is not part of the similarity comparison.
    /// This formula was derived by solving for what maximum value of distance returns false for the following
    /// statements:
    /// <pre>
    /// similarity = 1 - ((double)distance / (double)(prefixLength + std::min(textlen, targetlen)));
    /// return (similarity > minimumSimilarity);
    /// </pre>
    /// where distance is the Levenshtein distance for the two words.
    ///
    /// Levenshtein distance (also known as edit distance) is a measure of similarity between two strings where
    /// the distance is measured as the number of character deletions, insertions or substitutions required to
    /// transform one string to the other string.
    ///
    /// @param target The target word or phrase.
    /// @return the similarity, 0.0 or less indicates that it matches less than the required threshold and 1.0
    /// indicates that the text and target are identical.
    double similarity(const String& target);

    /// The max Distance is the maximum Levenshtein distance for the text compared to some other value that
    /// results in score that is better than the minimum similarity.
    /// @param m The length of the "other value"
    /// @return The maximum Levenshtein distance that we care about
    int32_t calculateMaxDistance(int32_t m);
};

}

#endif
