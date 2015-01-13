/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include <iostream>
#include "CheckIndex.h"
#include "_CheckIndex.h"
#include "SegmentInfos.h"
#include "SegmentInfo.h"
#include "SegmentReader.h"
#include "Directory.h"
#include "IndexInput.h"
#include "BitVector.h"
#include "Term.h"
#include "TermEnum.h"
#include "TermPositions.h"
#include "Document.h"
#include "FSDirectory.h"
#include "InfoStream.h"
#include "StringUtils.h"

namespace Lucene {

bool CheckIndex::_assertsOn = false;

CheckIndex::CheckIndex(const DirectoryPtr& dir) {
    this->dir = dir;
}

CheckIndex::~CheckIndex() {
}

void CheckIndex::setInfoStream(const InfoStreamPtr& out) {
    infoStream = out;
}

void CheckIndex::msg(const String& msg) {
    if (infoStream) {
        *infoStream << msg << L"\n";
    }
}

IndexStatusPtr CheckIndex::checkIndex() {
    return checkIndex(Collection<String>());
}

IndexStatusPtr CheckIndex::checkIndex(Collection<String> onlySegments) {
    SegmentInfosPtr sis(newLucene<SegmentInfos>());
    IndexStatusPtr result(newLucene<IndexStatus>());
    result->dir = dir;
    try {
        sis->read(dir);
    } catch (...) {
        msg(L"ERROR: could not read any segments file in directory");
        result->missingSegments = true;
        return result;
    }

    int32_t numSegments = sis->size();
    String segmentsFileName(sis->getCurrentSegmentFileName());
    IndexInputPtr input;

    try {
        input = dir->openInput(segmentsFileName);
    } catch (...) {
        msg(L"ERROR: could not open segments file in directory");
        result->cantOpenSegments = true;
        return result;
    }

    int32_t format = 0;
    try {
        format = input->readInt();
    } catch (...) {
        msg(L"ERROR: could not read segment file version in directory");
        result->missingSegmentVersion = true;
        if (input) {
            input->close();
        }
        return result;
    }
    if (input) {
        input->close();
    }

    String sFormat;
    bool skip = false;

    if (format == SegmentInfos::FORMAT) {
        sFormat = L"FORMAT [Lucene Pre-2.1]";
    }
    if (format == SegmentInfos::FORMAT_LOCKLESS) {
        sFormat = L"FORMAT_LOCKLESS [Lucene 2.1]";
    } else if (format == SegmentInfos::FORMAT_SINGLE_NORM_FILE) {
        sFormat = L"FORMAT_SINGLE_NORM_FILE [Lucene 2.2]";
    } else if (format == SegmentInfos::FORMAT_SHARED_DOC_STORE) {
        sFormat = L"FORMAT_SHARED_DOC_STORE [Lucene 2.3]";
    } else {
        if (format == SegmentInfos::FORMAT_CHECKSUM) {
            sFormat = L"FORMAT_CHECKSUM [Lucene 2.4]";
        } else if (format == SegmentInfos::FORMAT_DEL_COUNT) {
            sFormat = L"FORMAT_DEL_COUNT [Lucene 2.4]";
        } else if (format == SegmentInfos::FORMAT_HAS_PROX) {
            sFormat = L"FORMAT_HAS_PROX [Lucene 2.4]";
        } else if (format == SegmentInfos::FORMAT_USER_DATA) {
            sFormat = L"FORMAT_USER_DATA [Lucene 2.9]";
        } else if (format == SegmentInfos::FORMAT_DIAGNOSTICS) {
            sFormat = L"FORMAT_DIAGNOSTICS [Lucene 2.9]";
        } else if (format < SegmentInfos::CURRENT_FORMAT) {
            sFormat = L"int=" + StringUtils::toString(format) + L" [newer version of Lucene than this tool]";
            skip = true;
        } else {
            sFormat = StringUtils::toString(format) + L" [Lucene 1.3 or prior]";
        }
    }

    result->segmentsFileName = segmentsFileName;
    result->numSegments = numSegments;
    result->segmentFormat = sFormat;
    result->userData = sis->getUserData();
    String userDataString;
    if (!sis->getUserData().empty()) {
        userDataString = L" userData(size)=" + StringUtils::toString(sis->getUserData().size());
    }

    msg(L"Segments file=" + segmentsFileName + L" numSegments=" + StringUtils::toString(numSegments) +
        L" version=" + sFormat + userDataString);

    if (onlySegments) {
        result->partial = true;
        msg(L"\nChecking only these segments:");
        for (Collection<String>::iterator s = onlySegments.begin(); s != onlySegments.end(); ++s) {
            msg(L" " + *s);
        }
        result->segmentsChecked.addAll(onlySegments.begin(), onlySegments.end());
        msg(L":");
    }

    if (skip) {
        msg(L"\nERROR: this index appears to be created by a newer version of Lucene than this tool was compiled on;" \
            L" please re-compile this tool on the matching version of Lucene; exiting");
        result->toolOutOfDate = true;
        return result;
    }

    result->newSegments = boost::dynamic_pointer_cast<SegmentInfos>(sis->clone());
    result->newSegments->clear();

    for (int32_t i = 0; i < numSegments; ++i) {
        SegmentInfoPtr info(sis->info(i));
        if (onlySegments && !onlySegments.contains(info->name)) {
            continue;
        }
        SegmentInfoStatusPtr segInfoStat(newLucene<SegmentInfoStatus>());
        result->segmentInfos.add(segInfoStat);
        msg(L"  name=" + info->name + L" docCount=" + StringUtils::toString(info->docCount));
        segInfoStat->name = info->name;
        segInfoStat->docCount = info->docCount;

        int32_t toLoseDocCount = info->docCount;

        SegmentReaderPtr reader;

        try {
            msg(L"    compound=" + StringUtils::toString(info->getUseCompoundFile()));
            segInfoStat->compound = info->getUseCompoundFile();
            msg(L"    hasProx=" + StringUtils::toString(info->getHasProx()));
            segInfoStat->hasProx = info->getHasProx();
            msg(L"    numFiles=" + StringUtils::toString(info->files().size()));
            segInfoStat->numFiles = info->files().size();
            msg(L"    size (MB)=" + StringUtils::toString((double)info->sizeInBytes() / (double)(1024 * 1024)));
            segInfoStat->sizeMB = (double)info->sizeInBytes() / (double)(1024 * 1024);
            MapStringString diagnostics(info->getDiagnostics());
            segInfoStat->diagnostics = diagnostics;
            if (!diagnostics.empty()) {
                msg(L"    diagnostics (size)= " + StringUtils::toString(diagnostics.size()));
            }

            int32_t docStoreOffset = info->getDocStoreOffset();
            if (docStoreOffset != -1) {
                msg(L"    docStoreOffset=" + StringUtils::toString(docStoreOffset));
                segInfoStat->docStoreOffset = docStoreOffset;
                msg(L"    docStoreSegment=" + info->getDocStoreSegment());
                segInfoStat->docStoreSegment = info->getDocStoreSegment();
                msg(L"    docStoreIsCompoundFile=" + StringUtils::toString(info->getDocStoreIsCompoundFile()));
                segInfoStat->docStoreCompoundFile = info->getDocStoreIsCompoundFile();
            }
            String delFileName(info->getDelFileName());
            if (delFileName.empty()) {
                msg(L"    no deletions");
                segInfoStat->hasDeletions = false;
            } else {
                msg(L"    has deletions [delFileName=" + delFileName + L"]");
                segInfoStat->hasDeletions = true;
                segInfoStat->deletionsFileName = delFileName;
            }
            msg(L"    test: open reader.........");
            reader = SegmentReader::get(true, info, IndexReader::DEFAULT_TERMS_INDEX_DIVISOR);

            segInfoStat->openReaderPassed = true;

            int32_t numDocs = reader->numDocs();
            toLoseDocCount = numDocs;
            if (reader->hasDeletions()) {
                if (reader->deletedDocs->count() != info->getDelCount()) {
                    boost::throw_exception(RuntimeException(L"delete count mismatch: info=" + StringUtils::toString(info->getDelCount()) +
                                                            L" vs deletedDocs.count()=" + StringUtils::toString(reader->deletedDocs->count())));
                }
                if (reader->deletedDocs->count() > reader->maxDoc()) {
                    boost::throw_exception(RuntimeException(L"too many deleted docs: maxDoc()=" + StringUtils::toString(reader->maxDoc()) +
                                                            L" vs deletedDocs.count()=" + StringUtils::toString(reader->deletedDocs->count())));
                }
                if (info->docCount - numDocs != info->getDelCount()) {
                    boost::throw_exception(RuntimeException(L"delete count mismatch: info=" + StringUtils::toString(info->getDelCount()) +
                                                            L" vs reader=" + StringUtils::toString((info->docCount - numDocs))));
                }
                segInfoStat->numDeleted = info->docCount - numDocs;
                msg(L"OK [" + StringUtils::toString(segInfoStat->numDeleted) + L" deleted docs]");
            } else {
                if (info->getDelCount() != 0) {
                    boost::throw_exception(RuntimeException(L"delete count mismatch: info=" + StringUtils::toString(info->getDelCount()) +
                                                            L" vs reader=" + StringUtils::toString(info->docCount - numDocs)));
                }
                msg(L"OK");
            }
            if (reader->maxDoc() != info->docCount) {
                boost::throw_exception(RuntimeException(L"SegmentReader.maxDoc() " + StringUtils::toString(reader->maxDoc()) +
                                                        L" != SegmentInfos.docCount " + StringUtils::toString(info->docCount)));
            }
            msg(L"    test: fields..............");
            HashSet<String> fieldNames(reader->getFieldNames(IndexReader::FIELD_OPTION_ALL));
            msg(L"OK [" + StringUtils::toString(fieldNames.size()) + L" fields]");
            segInfoStat->numFields = fieldNames.size();

            // Test Field Norms
            segInfoStat->fieldNormStatus = testFieldNorms(Collection<String>::newInstance(fieldNames.begin(), fieldNames.end()), reader);

            // Test the Term Index
            segInfoStat->termIndexStatus = testTermIndex(info, reader);

            // Test Stored Fields
            segInfoStat->storedFieldStatus = testStoredFields(info, reader);

            // Test Term Vectors
            segInfoStat->termVectorStatus = testTermVectors(info, reader);

            // Rethrow the first exception we encountered.  This will cause stats for failed segments to be incremented properly
            if (!segInfoStat->fieldNormStatus->error.isNull()) {
                boost::throw_exception(RuntimeException(L"Field Norm test failed"));
            } else if (!segInfoStat->termIndexStatus->error.isNull()) {
                boost::throw_exception(RuntimeException(L"Term Index test failed"));
            } else if (!segInfoStat->storedFieldStatus->error.isNull()) {
                boost::throw_exception(RuntimeException(L"Stored Field test failed"));
            } else if (!segInfoStat->termVectorStatus->error.isNull()) {
                boost::throw_exception(RuntimeException(L"Term Vector test failed"));
            }

            msg(L"");
        } catch (...) {
            msg(L"FAILED");
            String comment(L"fixIndex() would remove reference to this segment");
            msg(L"    WARNING: " + comment + L"; full exception:");
            msg(L"");
            result->totLoseDocCount += toLoseDocCount;
            ++result->numBadSegments;

            if (reader) {
                reader->close();
            }

            continue;
        }
        if (reader) {
            reader->close();
        }

        // Keeper
        result->newSegments->add(boost::dynamic_pointer_cast<SegmentInfo>(info->clone()));
    }

    if (result->numBadSegments == 0) {
        result->clean = true;
        msg(L"No problems were detected with this index.\n");
    } else {
        msg(L"WARNING: " + StringUtils::toString(result->numBadSegments) +
            L" broken segments (containing " + StringUtils::toString(result->totLoseDocCount) +
            L" documents) detected");
    }

    return result;
}

FieldNormStatusPtr CheckIndex::testFieldNorms(Collection<String> fieldNames, const SegmentReaderPtr& reader) {
    FieldNormStatusPtr status(newLucene<FieldNormStatus>());

    try {
        // Test Field Norms
        msg(L"    test: field norms.........");

        ByteArray b(ByteArray::newInstance(reader->maxDoc()));
        for (Collection<String>::iterator fieldName = fieldNames.begin(); fieldName != fieldNames.end(); ++fieldName) {
            if (reader->hasNorms(*fieldName)) {
                reader->norms(*fieldName, b, 0);
                ++status->totFields;
            }
        }

        msg(L"OK [" + StringUtils::toString(status->totFields) + L" fields]");
    } catch (LuceneException& e) {
        msg(L"ERROR [" + e.getError() + L"]");
        status->error = e;
    }

    return status;
}

TermIndexStatusPtr CheckIndex::testTermIndex(const SegmentInfoPtr& info, const SegmentReaderPtr& reader) {
    TermIndexStatusPtr status(newLucene<TermIndexStatus>());

    try {
        msg(L"    test: terms, freq, prox...");

        TermEnumPtr termEnum(reader->terms());
        TermPositionsPtr termPositions(reader->termPositions());

        // Used only to count up # deleted docs for this term
        MySegmentTermDocsPtr myTermDocs(newLucene<MySegmentTermDocs>(reader));

        int32_t maxDoc = reader->maxDoc();

        while (termEnum->next()) {
            ++status->termCount;
            TermPtr term(termEnum->term());
            int32_t docFreq = termEnum->docFreq();
            termPositions->seek(term);
            int32_t lastDoc = -1;
            int32_t freq0 = 0;
            status->totFreq += docFreq;
            while (termPositions->next()) {
                ++freq0;
                int32_t doc = termPositions->doc();
                int32_t freq = termPositions->freq();
                if (doc <= lastDoc) {
                    boost::throw_exception(RuntimeException(L"term " + term->toString() +
                                                            L": doc " + StringUtils::toString(doc) +
                                                            L" <= lastDoc " + StringUtils::toString(lastDoc)));
                }
                if (doc >= maxDoc) {
                    boost::throw_exception(RuntimeException(L"term " + term->toString() +
                                                            L": doc " + StringUtils::toString(doc) +
                                                            L" >= maxDoc " + StringUtils::toString(maxDoc)));
                }

                lastDoc = doc;
                if (freq <= 0) {
                    boost::throw_exception(RuntimeException(L"term " + term->toString() +
                                                            L": doc " + StringUtils::toString(doc) +
                                                            L": freq " + StringUtils::toString(freq) +
                                                            L" is out of bounds"));
                }

                int32_t lastPos = -1;
                status->totPos += freq;
                for (int32_t j = 0; j < freq; ++j) {
                    int32_t pos = termPositions->nextPosition();
                    if (pos < -1) {
                        boost::throw_exception(RuntimeException(L"term " + term->toString() +
                                                                L": doc " + StringUtils::toString(doc) +
                                                                L": pos " + StringUtils::toString(pos) +
                                                                L" is out of bounds"));
                    }
                    if (pos < lastPos) {
                        boost::throw_exception(RuntimeException(L"term " + term->toString() +
                                                                L": doc " + StringUtils::toString(doc) +
                                                                L": pos " + StringUtils::toString(pos) +
                                                                L" < lastPos " + StringUtils::toString(lastPos)));
                    }
                    lastPos = pos;
                }
            }

            // Now count how many deleted docs occurred in this term
            int32_t delCount;
            if (reader->hasDeletions()) {
                myTermDocs->seek(term);
                while (myTermDocs->next()) {
                }
                delCount = myTermDocs->delCount;
            } else {
                delCount = 0;
            }

            if (freq0 + delCount != docFreq) {
                boost::throw_exception(RuntimeException(L"term " + term->toString() +
                                                        L"docFreq=" + StringUtils::toString(docFreq) +
                                                        L" != num docs seen " + StringUtils::toString(freq0) +
                                                        L" + num docs deleted " + StringUtils::toString(delCount)));
            }
        }

        msg(L"OK [" + StringUtils::toString(status->termCount) + L" terms; " + StringUtils::toString(status->totFreq) +
            L" terms/docs pairs; " + StringUtils::toString(status->totPos) + L" tokens]");
    } catch (LuceneException& e) {
        msg(L"ERROR [" + e.getError() + L"]");
        status->error = e;
    }

    return status;
}

StoredFieldStatusPtr CheckIndex::testStoredFields(const SegmentInfoPtr& info, const SegmentReaderPtr& reader) {
    StoredFieldStatusPtr status(newLucene<StoredFieldStatus>());

    try {
        msg(L"    test: stored fields.......");

        // Scan stored fields for all documents
        for (int32_t j = 0; j < info->docCount; ++j) {
            if (!reader->isDeleted(j)) {
                ++status->docCount;
                DocumentPtr doc(reader->document(j, FieldSelectorPtr()));
                status->totFields += doc->getFields().size();
            }
        }

        // Validate docCount
        if (status->docCount != reader->numDocs()) {
            boost::throw_exception(RuntimeException(L"docCount=" + StringUtils::toString(status->docCount) +
                                                    L" but saw " + StringUtils::toString(status->docCount) +
                                                    L" undeleted docs"));
        }

        msg(L"OK [" + StringUtils::toString(status->totFields) + L" total field count; avg " +
            StringUtils::toString((double)status->totFields / (double)status->docCount) + L" fields per doc]");
    } catch (LuceneException& e) {
        msg(L"ERROR [" + e.getError() + L"]");
        status->error = e;
    }

    return status;
}

TermVectorStatusPtr CheckIndex::testTermVectors(const SegmentInfoPtr& info, const SegmentReaderPtr& reader) {
    TermVectorStatusPtr status(newLucene<TermVectorStatus>());

    try {
        msg(L"    test: term vectors........");

        for (int32_t j = 0; j < info->docCount; ++j) {
            if (!reader->isDeleted(j)) {
                ++status->docCount;
                Collection<TermFreqVectorPtr> tfv(reader->getTermFreqVectors(j));
                if (tfv) {
                    status->totVectors += tfv.size();
                }
            }
        }

        msg(L"OK [" + StringUtils::toString(status->totVectors) + L" total vector count; avg " +
            StringUtils::toString((double)status->totVectors / (double)status->docCount) + L" term/freq vector fields per doc]");
    } catch (LuceneException& e) {
        msg(L"ERROR [" + e.getError() + L"]");
        status->error = e;
    }

    return status;
}

void CheckIndex::fixIndex(const IndexStatusPtr& result) {
    if (result->partial) {
        boost::throw_exception(IllegalArgumentException(L"can only fix an index that was fully checked (this status checked a subset of segments)"));
    }
    result->newSegments->commit(result->dir);
}

bool CheckIndex::testAsserts() {
    _assertsOn = true;
    return true;
}

bool CheckIndex::assertsOn() {
    BOOST_ASSERT(testAsserts());
    return _assertsOn;
}

int CheckIndex::main(Collection<String> args) {
    bool doFix = false;
    Collection<String> onlySegments(Collection<String>::newInstance());
    String indexPath;
    for (Collection<String>::iterator arg = args.begin(); arg != args.end(); ++arg) {
        if (*arg == L"-fix") {
            doFix = true;
        } else if (*arg == L"-segment") {
            if (arg + 1 == args.end()) {
                std::wcout << L"ERROR: missing name for -segment option\n";
                return 1;
            }
            ++arg;
            onlySegments.add(*arg);
        } else {
            if (!indexPath.empty()) {
                std::wcout << L"ERROR: unexpected extra argument '" << *arg << L"'\n";
                return 1;
            }
            indexPath = *arg;
        }
    }

    if (indexPath.empty()) {
        std::wcout << L"\nERROR: index path not specified\n";
        std::wcout << L"Usage: CheckIndex pathToIndex [-fix] [-segment X] [-segment Y]\n";
        std::wcout << L"\n";
        std::wcout << L"  -fix: actually write a new segments_N file, removing any problematic segments\n";
        std::wcout << L"  -segment X: only check the specified segments.  This can be specified multiple\n";
        std::wcout << L"              times, to check more than one segment, eg '-segment _2 -segment _a'.\n";
        std::wcout << L"              You can't use this with the -fix option\n";
        std::wcout << L"\n";
        std::wcout << L"**WARNING**: -fix should only be used on an emergency basis as it will cause\n";
        std::wcout << L"documents (perhaps many) to be permanently removed from the index.  Always make\n";
        std::wcout << L"a backup copy of your index before running this!  Do not run this tool on an index\n";
        std::wcout << L"that is actively being written to.  You have been warned!\n";
        std::wcout << L"\n";
        std::wcout << L"Run without -fix, this tool will open the index, report version information\n";
        std::wcout << L"and report any exceptions it hits and what action it would take if -fix were\n";
        std::wcout << L"specified.  With -fix, this tool will remove any segments that have issues and\n";
        std::wcout << L"write a new segments_N file.  This means all documents contained in the affected\n";
        std::wcout << L"segments will be removed.\n";
        std::wcout << L"\n";
        std::wcout << L"This tool exits with exit code 1 if the index cannot be opened or has any\n";
        std::wcout << L"corruption, else 0.\n\n";
        return 1;
    }

    if (!assertsOn()) {
        std::wcout << L"\nNOTE: testing will be more thorough if you run with '-ea', so assertions are enabled\n";
    }

    if (onlySegments.empty()) {
        onlySegments.reset();
    } else if (doFix) {
        std::wcout << L"ERROR: cannot specify both -fix and -segment\n";
        return 1;
    }

    std::wcout << L"\nOpening index @ " << indexPath << L"\n\n";
    DirectoryPtr dir;
    try {
        dir = FSDirectory::open(indexPath);
    } catch (...) {
        std::wcout << L"ERROR: could not open directory \"" << indexPath << L"\"; exiting\n";
        return 1;
    }

    CheckIndexPtr checker(newLucene<CheckIndex>(dir));
    checker->setInfoStream(newLucene<InfoStreamOut>());

    IndexStatusPtr result(checker->checkIndex(onlySegments));
    if (result->missingSegments) {
        return 1;
    }

    if (!result->clean) {
        if (!doFix) {
            std::wcout << L"WARNING: would write new segments file, and " << result->totLoseDocCount << L" documents would be lost, if -fix were specified\n\n";
        } else {
            std::wcout << L"WARNING: " << result->totLoseDocCount << L" documents will be lost\n";
            std::wcout << L"NOTE: will write new segments file in 5 seconds; this will remove " << result->totLoseDocCount;
            std::wcout << L" docs from the index. THIS IS YOUR LAST CHANCE TO CTRL+C!\n";
            for (int32_t sec = 0; sec < 5; ++sec) {
                LuceneThread::threadSleep(1000);
                std::wcout << L"  " << (5 - sec) << L"...\n";
            }
            std::wcout << L"Writing...\n";
            checker->fixIndex(result);
            std::wcout << L"OK\n";
            std::wcout << L"Wrote new segments file \"" << result->newSegments->getCurrentSegmentFileName() << L"\"\n";
        }
    }

    std::wcout << L"\n";
    return ((result && result->clean) ? 0 : 1);
}

IndexStatus::IndexStatus() {
    clean = false;
    missingSegments = false;
    cantOpenSegments = false;
    missingSegmentVersion = false;
    numSegments = false;
    segmentInfos = Collection<SegmentInfoStatusPtr>::newInstance();
    segmentsChecked = Collection<String>::newInstance();
    toolOutOfDate = false;
    totLoseDocCount = 0;
    numBadSegments = 0;
    partial = false;
}

IndexStatus::~IndexStatus() {
}

SegmentInfoStatus::SegmentInfoStatus() {
    docCount = 0;
    compound = false;
    numFiles = 0;
    sizeMB = 0;
    docStoreOffset = -1;
    docStoreCompoundFile = false;
    hasDeletions = false;
    numDeleted = 0;
    openReaderPassed = false;
    numFields = 0;
    hasProx = false;
}

SegmentInfoStatus::~SegmentInfoStatus() {
}

FieldNormStatus::FieldNormStatus() {
    totFields = 0;
}

FieldNormStatus::~FieldNormStatus() {
}

TermIndexStatus::TermIndexStatus() {
    termCount = 0;
    totFreq = 0;
    totPos = 0;
}

TermIndexStatus::~TermIndexStatus() {
}

StoredFieldStatus::StoredFieldStatus() {
    docCount = 0;
    totFields = 0;
}

StoredFieldStatus::~StoredFieldStatus() {
}

TermVectorStatus::TermVectorStatus() {
    docCount = 0;
    totVectors = 0;
}

TermVectorStatus::~TermVectorStatus() {
}

MySegmentTermDocs::MySegmentTermDocs(const SegmentReaderPtr& p) : SegmentTermDocs(p) {
    delCount = 0;
}

MySegmentTermDocs::~MySegmentTermDocs() {
}

void MySegmentTermDocs::seek(const TermPtr& term) {
    SegmentTermDocs::seek(term);
    delCount = 0;
}

void MySegmentTermDocs::skippingDoc() {
    ++delCount;
}

}
