/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef SIMILARITY_H
#define SIMILARITY_H

#include "fnord-fts/search/Explanation.h"

namespace Lucene {

/// Scoring API.
///
/// Similarity defines the components of Lucene scoring.  Overriding computation of these components is
/// a convenient way to alter Lucene scoring.
///
/// Suggested reading: <a href="http://nlp.stanford.edu/IR-book/html/htmledition/queries-as-vectors-1.html">
/// Introduction To Information Retrieval, Chapter 6</a>.
///
/// The following describes how Lucene scoring evolves from underlying information retrieval models to
/// (efficient) implementation.  We first brief on VSM Score, then derive from it Lucene's Conceptual Scoring
/// Formula, from which, finally, evolves Lucene's Practical Scoring Function (the latter is connected directly
/// with Lucene classes and methods).
///
/// Lucene combines <a href="http://en.wikipedia.org/wiki/Standard_Boolean_model"> Boolean model (BM) of
/// Information Retrieval</a> with <a href="http://en.wikipedia.org/wiki/Vector_Space_Model"> Vector Space Model
/// (VSM) of Information Retrieval</a> - documents "approved" by BM are scored by VSM.
///
/// In VSM, documents and queries are represented as weighted vectors in a multi-dimensional space, where each
/// distinct index term is a dimension, and weights are <a href="http://en.wikipedia.org/wiki/Tfidf">Tf-idf</a>
/// values.
///
/// VSM does not require weights to be Tf-idf values, but Tf-idf values are believed to produce search results
/// of high quality, and so Lucene is using Tf-idf.  Tf and Idf are described in more detail below, but for now,
/// for completion, let's just say that for given term t and document (or query) x, Tf(t,x) varies with the
/// number of occurrences of term t in x (when one increases so does the other) and idf(t) similarly varies with
/// the inverse of the number of index documents containing term t.
///
/// VSM score of document d for query q is the <a href="http://en.wikipedia.org/wiki/Cosine_similarity"> Cosine
/// Similarity</a> of the weighted query vectors V(q) and V(d):
///
/// <br>&nbsp;<br>
/// <table cellpadding="2" cellspacing="2" border="0" align="center">
///   <tr><td>
///   <table cellpadding="1" cellspacing="0" border="1" align="center">
///     <tr><td>
///     <table cellpadding="2" cellspacing="2" border="0" align="center">
///       <tr>
///         <td valign="middle" align="right" rowspan="1">
///           cosine-similarity(q,d) &nbsp; = &nbsp;
///         </td>
///         <td valign="middle" align="center">
///           <table>
///              <tr><td align="center"><small>V(q)&nbsp;&middot;&nbsp;V(d)</small></td></tr>
///              <tr><td align="center">&ndash;&ndash;&ndash;&ndash;&ndash;&ndash;&ndash;&ndash;&ndash;</td></tr>
///              <tr><td align="center"><small>|V(q)|&nbsp;|V(d)|</small></td></tr>
///           </table>
///         </td>
///       </tr>
///     </table>
///     </td></tr>
///   </table>
///   </td></tr>
///   <tr><td>
///   <center><font=-1><u>VSM Score</u></font></center>
///   </td></tr>
/// </table>
/// <br>&nbsp;<br>
///
/// Where V(q) &middot; V(d) is the <a href="http://en.wikipedia.org/wiki/Dot_product">dot product</a> of the
/// weighted vectors, and |V(q)| and |V(d)| are their
/// <a href="http://en.wikipedia.org/wiki/Euclidean_norm#Euclidean_norm">Euclidean norms</a>.
///
/// Note: the above equation can be viewed as the dot product of the normalized weighted vectors, in the sense
/// that dividing V(q) by its euclidean norm is normalizing it to a unit vector.
///
/// Lucene refines VSM score for both search quality and usability:
/// <ul>
/// <li>Normalizing V(d) to the unit vector is known to be problematic in that  it removes all document length
/// information.  For some documents removing this info is probably ok, eg. a document made by duplicating a
/// certain paragraph 10 times, especially if that paragraph is made of distinct terms.  But for a document which
/// contains no duplicated paragraphs, this might be wrong.  To avoid this problem, a different document length
/// normalization factor is used, which normalizes to a vector equal to or larger than the unit vector:
/// doc-len-norm(d).
/// </li>
/// <li>At indexing, users can specify that certain documents are more important than others, by assigning a
/// document boost.  For this, the score of each document is also multiplied by its boost value doc-boost(d).
/// </li>
/// <li>Lucene is field based, hence each query term applies to a single field, document length normalization
/// is by the length of the certain field, and in addition to document boost there are also document fields
/// boosts.
/// </li>
/// <li>The same field can be added to a document during indexing several times, and so the boost of that field
/// is the multiplication of the boosts of the separate additions (or parts) of that field within the document.
/// </li>
/// <li>At search time users can specify boosts to each query, sub-query, and each query term, hence the
/// contribution of a query term to the score of a document is multiplied by the boost of that query term
/// query-boost(q).
/// </li>
/// <li>A document may match a multi term query without containing all the terms of that query (this is correct
/// for some of the queries), and users can further reward documents matching more query terms through a
/// coordination factor, which is usually larger when more terms are matched: coord-factor(q,d).
/// </li>
/// </ul>
///
/// Under the simplifying assumption of a single field in the index, we get Lucene's Conceptual scoring formula:
///
/// <br>&nbsp;<br>
/// <table cellpadding="2" cellspacing="2" border="0" align="center">
///   <tr><td>
///   <table cellpadding="1" cellspacing="0" border="1" align="center">
///     <tr><td>
///     <table cellpadding="2" cellspacing="2" border="0" align="center">
///       <tr>
///         <td valign="middle" align="right" rowspan="1">
///           score(q,d) &nbsp; = &nbsp;
///           <font color="#FF9933">coord-factor(q,d)</font> &middot; &nbsp;
///           <font color="#CCCC00">query-boost(q)</font> &middot; &nbsp;
///         </td>
///         <td valign="middle" align="center">
///           <table>
///              <tr><td align="center"><small><font color="#993399">V(q)&nbsp;&middot;&nbsp;V(d)</font></small></td></tr>
///              <tr><td align="center">&ndash;&ndash;&ndash;&ndash;&ndash;&ndash;&ndash;&ndash;&ndash;</td></tr>
///              <tr><td align="center"><small><font color="#FF33CC">|V(q)|</font></small></td></tr>
///           </table>
///         </td>
///         <td valign="middle" align="right" rowspan="1">
///           &nbsp; &middot; &nbsp; <font color="#3399FF">doc-len-norm(d)</font>
///           &nbsp; &middot; &nbsp; <font color="#3399FF">doc-boost(d)</font>
///         </td>
///       </tr>
///     </table>
///     </td></tr>
///   </table>
///   </td></tr>
///   <tr><td>
///   <center><font=-1><u>Lucene Conceptual Scoring Formula</u></font></center>
///   </td></tr>
/// </table>
/// <br>&nbsp;<br>
///
/// The conceptual formula is a simplification in the sense that (1) terms and documents are fielded and (2)
/// boosts are usually per query term rather than per query.
///
/// We now describe how Lucene implements this conceptual scoring formula, and derive from it Lucene's Practical
/// Scoring Function.
///
/// For efficient score computation some scoring components are computed and aggregated in advance:
/// <ul>
/// <li>Query-boost for the query (actually for each query term) is known when search starts.
/// </li>
/// <li>Query Euclidean norm |V(q)| can be computed when search starts, as it is independent of the document
/// being scored.  From search optimization perspective, it is a valid question why bother to normalize the
/// query at all, because all scored documents will be multiplied by the same |V(q)|, and hence documents ranks
/// (their order by score) will not be affected by this normalization.  There are two good reasons to keep this
/// normalization:
/// <ul>
/// <li>Recall that <a href="http://en.wikipedia.org/wiki/Cosine_similarity"> Cosine Similarity</a> can be used
/// find how similar two documents are. One can use Lucene for eg. clustering, and use a document as a query to
/// compute its similarity to other documents.  In this use case it is important that the score of document d3
/// for query d1 is comparable to the score of document d3 for query d2. In other words, scores of a document for
/// two distinct queries should be comparable.  There are other applications that may require this.  And this is
/// exactly what normalizing the query vector V(q) provides: comparability (to a certain extent) of two or more
/// queries.
/// </li>
/// <li>Applying query normalization on the scores helps to keep the scores around the unit vector, hence preventing
/// loss of score data because of floating point precision limitations.
/// </li>
/// </ul>
/// </li>
/// <li>Document length norm doc-len-norm(d) and document boost doc-boost(d) are known at indexing time.  They are
/// computed in advance and their multiplication is saved as a single value in the index: norm(d). (In the equations
/// below, norm(t in d) means norm(field(t) in doc d) where field(t) is the field associated with term t.)
/// </li>
/// </ul>
///
/// Lucene's Practical Scoring Function is derived from the above.  The color codes demonstrate how it relates to
/// those of the conceptual formula:
///
/// <table cellpadding="2" cellspacing="2" border="0" align="center">
///  <tr><td>
///  <table cellpadding="" cellspacing="2" border="2" align="center">
///  <tr><td>
///   <table cellpadding="2" cellspacing="2" border="0" align="center">
///   <tr>
///     <td valign="middle" align="right" rowspan="1">
///       score(q,d) &nbsp; = &nbsp;
///       <A HREF="#formula_coord"><font color="#FF9933">coord(q,d)</font></A> &nbsp;&middot;&nbsp;
///       <A HREF="#formula_queryNorm"><font color="#FF33CC">queryNorm(q)</font></A> &nbsp;&middot;&nbsp;
///     </td>
///     <td valign="bottom" align="center" rowspan="1">
///       <big><big><big>&sum;</big></big></big>
///     </td>
///     <td valign="middle" align="right" rowspan="1">
///       <big><big>(</big></big>
///       <A HREF="#formula_tf"><font color="#993399">tf(t in d)</font></A> &nbsp;&middot;&nbsp;
///       <A HREF="#formula_idf"><font color="#993399">idf(t)</font></A><sup>2</sup> &nbsp;&middot;&nbsp;
///       <A HREF="#formula_termBoost"><font color="#CCCC00">t.getBoost()</font></A>&nbsp;&middot;&nbsp;
///       <A HREF="#formula_norm"><font color="#3399FF">norm(t,d)</font></A>
///       <big><big>)</big></big>
///     </td>
///   </tr>
///   <tr valigh="top">
///    <td></td>
///    <td align="center"><small>t in q</small></td>
///    <td></td>
///   </tr>
///   </table>
///  </td></tr>
///  </table>
/// </td></tr>
/// <tr><td>
///  <center><font=-1><u>Lucene Practical Scoring Function</u></font></center>
/// </td></tr>
/// </table>
///
/// where
/// <ol>
///    <li>
///      <A NAME="formula_tf"></A>
///      <b>tf(t in d)</b>
///      correlates to the term's frequency, defined as the number of times term t appears in the currently
///      scored document d.  Documents that have more occurrences of a given term receive a higher score.
///      Note that tf(t in q) is assumed to be 1 and therefore it does not appear in this equation,
///      However if a query contains twice the same term, there will be two term-queries with that same term
///      and hence the computation would still be correct (although not very efficient).
///      The default computation for tf(t in d) in {@link DefaultSimilarity#tf(float) DefaultSimilarity} is:
///
///      <br>&nbsp;<br>
///      <table cellpadding="2" cellspacing="2" border="0" align="center">
///        <tr>
///          <td valign="middle" align="right" rowspan="1">
///            {@link DefaultSimilarity#tf(float) tf(t in d)} &nbsp; = &nbsp;
///          </td>
///          <td valign="top" align="center" rowspan="1">
///               frequency<sup><big>&frac12;</big></sup>
///          </td>
///        </tr>
///      </table>
///      <br>&nbsp;<br>
///    </li>
///
///    <li>
///      <A NAME="formula_idf"></A>
///      <b>idf(t)</b> stands for Inverse Document Frequency. This value correlates to the inverse of docFreq
///      (the number of documents in which the term t appears). This means rarer terms give higher contribution
///      to the total score.  idf(t) appears for t in both the query and the document, hence it is squared in
///      the equation. The default computation for idf(t) in {@link DefaultSimilarity#idf(int, int) DefaultSimilarity} is:
///
///      <br>&nbsp;<br>
///      <table cellpadding="2" cellspacing="2" border="0" align="center">
///        <tr>
///          <td valign="middle" align="right">
///            {@link DefaultSimilarity#idf(int, int) idf(t)}&nbsp; = &nbsp;
///          </td>
///          <td valign="middle" align="center">
///            1 + log <big>(</big>
///          </td>
///          <td valign="middle" align="center">
///            <table>
///               <tr><td align="center"><small>numDocs</small></td></tr>
///               <tr><td align="center">&ndash;&ndash;&ndash;&ndash;&ndash;&ndash;&ndash;&ndash;&ndash;</td></tr>
///               <tr><td align="center"><small>docFreq+1</small></td></tr>
///            </table>
///          </td>
///          <td valign="middle" align="center">
///            <big>)</big>
///          </td>
///        </tr>
///      </table>
///      <br>&nbsp;<br>
///    </li>
///
///    <li>
///      <A NAME="formula_coord"></A>
///      <b>coord(q,d)</b>
///      is a score factor based on how many of the query terms are found in the specified document.  Typically, a
///      document that contains more of the query's terms will receive a higher score than another document with
///      fewer query terms.  This is a search time factor computed in {@link #coord(int, int) coord(q,d)} by the
///      Similarity in effect at search time.
///      <br>&nbsp;<br>
///    </li>
///
///    <li><b>
///      <A NAME="formula_queryNorm"></A>
///      queryNorm(q)
///      </b>
///      is a normalizing factor used to make scores between queries comparable.  This factor does not affect
///      document ranking (since all ranked documents are multiplied by the same factor), but rather just attempts
///      to make scores from different queries (or even different indexes) comparable.  This is a search time
///      factor computed by the Similarity in effect at search time.
///
///      The default computation in {@link DefaultSimilarity#queryNorm(float) DefaultSimilarity}
///      produces a <a href="http://en.wikipedia.org/wiki/Euclidean_norm#Euclidean_norm">Euclidean norm</a>:
///      <br>&nbsp;<br>
///      <table cellpadding="1" cellspacing="0" border="0" align="center">
///        <tr>
///          <td valign="middle" align="right" rowspan="1">
///            queryNorm(q)  &nbsp; = &nbsp;
///            {@link DefaultSimilarity#queryNorm(float) queryNorm(sumOfSquaredWeights)}
///            &nbsp; = &nbsp;
///          </td>
///          <td valign="middle" align="center" rowspan="1">
///            <table>
///               <tr><td align="center"><big>1</big></td></tr>
///               <tr><td align="center"><big>
///                  &ndash;&ndash;&ndash;&ndash;&ndash;&ndash;&ndash;&ndash;&ndash;&ndash;&ndash;&ndash;&ndash;&ndash;
///               </big></td></tr>
///               <tr><td align="center">sumOfSquaredWeights<sup><big>&frac12;</big></sup></td></tr>
///            </table>
///          </td>
///        </tr>
///      </table>
///      <br>&nbsp;<br>
///
///      The sum of squared weights (of the query terms) is computed by the query {@link Weight} object.  For example,
///      a {@link BooleanQuery boolean query} computes this value as:
///
///      <br>&nbsp;<br>
///      <table cellpadding="1" cellspacing="0" border="0"n align="center">
///        <tr>
///          <td valign="middle" align="right" rowspan="1">
///            {@link Weight#sumOfSquaredWeights() sumOfSquaredWeights} &nbsp; = &nbsp;
///            {@link Query#getBoost() q.getBoost()} <sup><big>2</big></sup>
///            &nbsp;&middot;&nbsp;
///          </td>
///          <td valign="bottom" align="center" rowspan="1">
///            <big><big><big>&sum;</big></big></big>
///          </td>
///          <td valign="middle" align="right" rowspan="1">
///            <big><big>(</big></big>
///            <A HREF="#formula_idf">idf(t)</A> &nbsp;&middot;&nbsp;
///            <A HREF="#formula_termBoost">t.getBoost()</A>
///            <big><big>) <sup>2</sup> </big></big>
///          </td>
///        </tr>
///        <tr valigh="top">
///          <td></td>
///          <td align="center"><small>t in q</small></td>
///          <td></td>
///        </tr>
///      </table>
///      <br>&nbsp;<br>
///
///    </li>
///
///    <li>
///      <A NAME="formula_termBoost"></A>
///      <b>t.getBoost()</b>
///      is a search time boost of term t in the query q as specified in the query text or as set by application
///      calls to {@link Query#setBoost(float) setBoost()}. Notice that there is really no direct API for accessing
///      a boost of one term in a multi term query, but rather multi terms are represented in a query as multi
///      {@link TermQuery TermQuery} objects, and so the boost of a term in the query is accessible by calling
///      the sub-query {@link Query#getBoost() getBoost()}.
///      <br>&nbsp;<br>
///    </li>
///
///    <li>
///      <A NAME="formula_norm"></A>
///      <b>norm(t,d)</b> encapsulates a few (indexing time) boost and length factors:
///
///      <ul>
///        <li><b>Document boost</b> - set by calling
///        {@link Document#setBoost(float) doc.setBoost()}
///        before adding the document to the index.
///        </li>
///        <li><b>Field boost</b> - set by calling
///        {@link Fieldable#setBoost(float) field.setBoost()}
///        before adding the field to a document.
///        </li>
///        <li>{@link #lengthNorm(String, int) <b>lengthNorm</b>(field)} - computed when the document is added to
///        the index in accordance with the number of tokens of this field in the document, so that shorter fields
///        contribute more to the score.  LengthNorm is computed by the Similarity class in effect at indexing.
///        </li>
///      </ul>
///
///      When a document is added to the index, all the above factors are multiplied.
///      If the document has multiple fields with the same name, all their boosts are multiplied together:
///
///      <br>&nbsp;<br>
///      <table cellpadding="1" cellspacing="0" border="0"n align="center">
///        <tr>
///          <td valign="middle" align="right" rowspan="1">
///            norm(t,d) &nbsp; = &nbsp;
///            {@link Document#getBoost() doc.getBoost()}
///            &nbsp;&middot;&nbsp;
///            {@link #lengthNorm(String, int) lengthNorm(field)}
///            &nbsp;&middot;&nbsp;
///          </td>
///          <td valign="bottom" align="center" rowspan="1">
///            <big><big><big>&prod;</big></big></big>
///          </td>
///          <td valign="middle" align="right" rowspan="1">
///            {@link Fieldable#getBoost() f.getBoost}()
///          </td>
///        </tr>
///        <tr valigh="top">
///          <td></td>
///          <td align="center"><small>field <b>f</b> in d named as <b>t</b></small></td>
///          <td></td>
///        </tr>
///      </table>
///      <br>&nbsp;<br>
///      However the resulted norm value is {@link #encodeNorm(float) encoded} as a single byte before being stored.
///      At search time, the norm byte value is read from the index {@link Directory directory} and {@link
///      #decodeNorm(byte) decoded} back to a float norm value.  This encoding/decoding, while reducing index size,
///      comes with the price of precision loss - it is not guaranteed that decode(encode(x)) = x.  For instance,
///      decode(encode(0.89)) = 0.75.
///      <br>&nbsp;<br>
///      Compression of norm values to a single byte saves memory at search time, because once a field is referenced
///      at search time, its norms - for all documents - are maintained in memory.
///      <br>&nbsp;<br>
///      The rationale supporting such lossy compression of norm values is that given the difficulty (and inaccuracy)
///      of users to express their true information need by a query, only big differences matter.
///      <br>&nbsp;<br>
///      Last, note that search time is too late to modify this norm part of scoring, eg. by using a different
///      {@link Similarity} for search.
///      <br>&nbsp;<br>
///    </li>
/// </ol>
///
/// @see #setDefault(SimilarityPtr)
/// @see IndexWriter#setSimilarity(SimilarityPtr)
/// @see Searcher#setSimilarity(SimilarityPtr)
class Similarity : public LuceneObject {
public:
    Similarity();
    virtual ~Similarity();

    LUCENE_CLASS(Similarity);

protected:
    static const int32_t NO_DOC_ID_PROVIDED;

protected:
    static const Collection<double> NORM_TABLE();

public:
    /// Return the default Similarity implementation used by indexing and search code.
    /// This is initially an instance of {@link DefaultSimilarity}.
    /// @see Searcher#setSimilarity(SimilarityPtr)
    /// @see IndexWriter#setSimilarity(SimilarityPtr)
    static SimilarityPtr getDefault();

    /// Decodes a normalization factor stored in an index.
    /// @see #encodeNorm(double)
    static double decodeNorm(uint8_t b);

    /// Returns a table for decoding normalization bytes.
    /// @see #encodeNorm(double)
    static const Collection<double> getNormDecoder();

    /// Compute the normalization value for a field, given the accumulated state of term processing for this
    /// field (see {@link FieldInvertState}).
    ///
    /// Implementations should calculate a float value based on the field state and then return that value.
    ///
    /// For backward compatibility this method by default calls {@link #lengthNorm(String, int32_t)} passing
    /// {@link FieldInvertState#getLength()} as the second argument, and then multiplies this value by {@link
    /// FieldInvertState#getBoost()}.
    ///
    /// @param field Field name
    /// @param state Current processing state for this field
    /// @return The calculated float norm
    virtual double computeNorm(const String& fieldName, const FieldInvertStatePtr& state);

    /// Computes the normalization value for a field given the total number of terms contained in a field.
    /// These values, together with field boosts, are stored in an index and multiplied into scores for hits
    /// on each field by the search code.
    ///
    /// Matches in longer fields are less precise, so implementations of this method usually return smaller
    /// values when numTokens is large, and larger values when numTokens is small.
    ///
    /// Note that the return values are computed under {@link IndexWriter#addDocument(DocumentPtr)} and then
    /// stored using {@link #encodeNorm(double)}.  Thus they have limited precision, and documents must be
    /// re-indexed if this method is altered.
    ///
    /// @param fieldName The name of the field
    /// @param numTokens The total number of tokens contained in fields named fieldName of doc.
    /// @return A normalization factor for hits on this field of this document
    /// @see Field#setBoost(double)
    virtual double lengthNorm(const String& fieldName, int32_t numTokens) = 0;

    /// Computes the normalization value for a query given the sum of the squared weights of each of the query
    /// terms.  This value is multiplied into the weight of each query term. While the classic query
    /// normalization factor is computed as 1/sqrt(sumOfSquaredWeights), other implementations might completely
    /// ignore sumOfSquaredWeights (ie return 1).
    ///
    /// This does not affect ranking, but the default implementation does make scores from different queries
    /// more comparable than they would be by eliminating the magnitude of the Query vector as a factor in the
    /// score.
    ///
    /// @param sumOfSquaredWeights The sum of the squares of query term weights
    /// @return a normalization factor for query weights
    virtual double queryNorm(double sumOfSquaredWeights) = 0;

    /// Encodes a normalization factor for storage in an index.
    ///
    /// The encoding uses a three-bit mantissa, a five-bit exponent, and the zero-exponent point at 15, thus
    /// representing values from around 7x10^9 to 2x10^-9 with about one significant decimal digit of accuracy.
    /// Zero is also represented.  Negative numbers are rounded up to zero.  Values too large to represent
    /// are rounded down to the largest representable value.  Positive values too small to represent are rounded
    /// up to the smallest positive representable value.
    ///
    /// @see Field#setBoost(double)
    static uint8_t encodeNorm(double f);

    /// Computes a score factor based on a term or phrase's frequency in a document.  This value is multiplied
    /// by the {@link #idf(int32_t, int32_t)} factor for each term in the query and these products are then
    /// summed to form the initial score for a document.
    ///
    /// Terms and phrases repeated in a document indicate the topic of the document, so implementations of this
    /// method usually return larger values when freq is large, and smaller values when freq is small.
    ///
    /// The default implementation calls {@link #tf(double)}.
    ///
    /// @param freq The frequency of a term within a document
    /// @return A score factor based on a term's within-document frequency
    virtual double tf(int32_t freq);

    /// Computes the amount of a sloppy phrase match, based on an edit distance.  This value is summed for
    /// each sloppy phrase match in a document to form the frequency that is passed to {@link #tf(double)}.
    ///
    /// A phrase match with a small edit distance to a document passage more closely matches the document, so
    /// implementations of this method usually return larger values when the edit distance is small and
    /// smaller values when it is large.
    ///
    /// @see PhraseQuery#setSlop(int32_t)
    /// @param distance The edit distance of this sloppy phrase match
    /// @return The frequency increment for this match
    virtual double sloppyFreq(int32_t distance) = 0;

    /// Computes a score factor based on a term or phrase's frequency in a document.  This value is multiplied
    /// by the {@link #idf(int32_t, int32_t)} factor for each term in the query and these products are then
    /// summed to form the initial score for a document.
    ///
    /// Terms and phrases repeated in a document indicate the topic of the document, so implementations of this
    /// method usually return larger values when freq is large, and smaller values when freq is small.
    ///
    /// @param freq The frequency of a term within a document
    /// @return A score factor based on a term's within-document frequency
    virtual double tf(double freq) = 0;

    /// Computes a score factor for a simple term and returns an explanation for that score factor.
    ///
    /// The default implementation uses:
    /// <pre>
    /// idf(searcher->docFreq(term), searcher->maxDoc());
    /// </pre>
    ///
    /// Note that {@link Searcher#maxDoc()} is used instead of {@link IndexReader#numDocs() IndexReader#numDocs()}
    /// because also {@link Searcher#docFreq(TermPtr)} is used, and when the latter is inaccurate, so is {@link
    /// Searcher#maxDoc()}, and in the same direction.  In addition, {@link Searcher#maxDoc()} is more efficient
    /// to compute.
    ///
    /// @param term The term in question
    /// @param searcher The document collection being searched
    /// @return An IDFExplain object that includes both an idf score factor and an explanation for the term.
    virtual IDFExplanationPtr idfExplain(const TermPtr& term, const SearcherPtr& searcher);

    /// Computes a score factor for a phrase.
    ///
    /// The default implementation sums the idf factor for each term in the phrase.
    ///
    /// @param terms The terms in the phrase
    /// @param searcher The document collection being searched
    /// @return An IDFExplain object that includes both an idf score factor for the phrase and an explanation
    /// for each term.
    virtual IDFExplanationPtr idfExplain(Collection<TermPtr> terms, const SearcherPtr& searcher);

    /// Computes a score factor based on a term's document frequency (the number of documents which contain the
    /// term).  This value is multiplied by the {@link #tf(int32_t)} factor for each term in the query and these
    /// products are then summed to form the initial score for a document.
    ///
    /// Terms that occur in fewer documents are better indicators of topic, so implementations of this method
    /// usually return larger values for rare terms, and smaller values for common terms.
    ///
    /// @param docFreq The number of documents which contain the term
    /// @param numDocs The total number of documents in the collection
    /// @return A score factor based on the term's document frequency
    virtual double idf(int32_t docFreq, int32_t numDocs) = 0;

    /// Computes a score factor based on the fraction of all query terms that a document contains.  This value
    /// is multiplied into scores.
    ///
    /// The presence of a large portion of the query terms indicates a better match with the query, so
    /// implementations of this method usually return larger values when the ratio between these parameters is
    /// large and smaller values when the ratio between them is small.
    ///
    /// @param overlap The number of query terms matched in the document
    /// @param maxOverlap The total number of terms in the query
    /// @return A score factor based on term overlap with the query
    virtual double coord(int32_t overlap, int32_t maxOverlap) = 0;

    /// Calculate a scoring factor based on the data in the payload.  Overriding implementations are responsible
    /// for interpreting what is in the payload.  Lucene makes no assumptions about what is in the byte array.
    ///
    /// The default implementation returns 1.
    ///
    /// @param docId The docId currently being scored.  If this value is {@link #NO_DOC_ID_PROVIDED}, then it
    /// should be assumed that the PayloadQuery implementation does not provide document information
    /// @param fieldName The fieldName of the term this payload belongs to
    /// @param start The start position of the payload
    /// @param end The end position of the payload
    /// @param payload The payload byte array to be scored
    /// @param offset The offset into the payload array
    /// @param length The length in the array
    /// @return An implementation dependent float to be used as a scoring factor
    virtual double scorePayload(int32_t docId, const String& fieldName, int32_t start, int32_t end, ByteArray payload, int32_t offset, int32_t length);
};

}

#endif
