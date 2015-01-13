/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef ABSTRACTALLTERMDOCS_H
#define ABSTRACTALLTERMDOCS_H

#include "TermDocs.h"

namespace Lucene {

/// Base class for enumerating all but deleted docs.
///
/// NOTE: this class is meant only to be used internally by Lucene; it's only public so it
/// can be shared across packages.
class AbstractAllTermDocs : public TermDocs, public LuceneObject {
public:
    AbstractAllTermDocs(int32_t maxDoc);
    virtual ~AbstractAllTermDocs();

    LUCENE_CLASS(AbstractAllTermDocs);

protected:
    int32_t maxDoc;
    int32_t _doc;

public:
    virtual void seek(const TermPtr& term);
    virtual void seek(const TermEnumPtr& termEnum);
    virtual int32_t doc();
    virtual int32_t freq();
    virtual bool next();
    virtual int32_t read(Collection<int32_t> docs, Collection<int32_t> freqs);
    virtual bool skipTo(int32_t target);
    virtual void close();
    virtual bool isDeleted(int32_t doc) = 0;
};

}

#endif
