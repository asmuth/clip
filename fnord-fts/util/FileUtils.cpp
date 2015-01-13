/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include "LuceneThread.h"
#include "StringUtils.h"
#include "FileUtils.h"

#if defined(_WIN32) || defined(_WIN64)
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#else
#include <unistd.h>
#endif

namespace Lucene {

namespace FileUtils {

bool fileExists(const String& path) {
    try {
        return boost::filesystem::exists(path.c_str());
    } catch (...) {
        return false;
    }
}

uint64_t fileModified(const String& path) {
    try {
        return (uint64_t)boost::filesystem::last_write_time(path.c_str());
    } catch (...) {
        return 0;
    }
}

bool touchFile(const String& path) {
    try {
        boost::filesystem::last_write_time(path.c_str(), time(NULL));
        return true;
    } catch (...) {
        return false;
    }
}

int64_t fileLength(const String& path) {
    try {
        int64_t fileSize = (int64_t)boost::filesystem::file_size(path.c_str());
        for (int32_t i = 0; fileSize == 0 && i < 100; ++i) {
            LuceneThread::threadYield();
            fileSize = (int64_t)boost::filesystem::file_size(path.c_str());
        }
        return fileSize;
    } catch (...) {
        return 0;
    }
}

bool setFileLength(const String& path, int64_t length) {
    try {
        if (!fileExists(path)) {
            return false;
        }
#if defined(_WIN32) || defined(_WIN64)
        int32_t fd = _wopen(path.c_str(), _O_WRONLY | _O_CREAT | _O_BINARY, _S_IWRITE);
        return _chsize(fd, (long)length) == 0;
#else
        return truncate(boost::filesystem::path(path).c_str(), (off_t)length) == 0;
#endif
    } catch (...) {
        return false;
    }
}

bool removeFile(const String& path) {
    try {
        return boost::filesystem::remove(path.c_str());
    } catch (...) {
        return false;
    }
}

bool copyFile(const String& source, const String& dest) {
    try {
        boost::filesystem::copy_file(source.c_str(), dest.c_str());
        return true;
    } catch (...) {
        return false;
    }
}

bool createDirectory(const String& path) {
    try {
        return boost::filesystem::create_directory(path.c_str());
    } catch (...) {
        return false;
    }
}

bool removeDirectory(const String& path) {
    try {
        boost::filesystem::remove_all(path.c_str());
        return true;
    } catch (...) {
        return false;
    }
}

bool isDirectory(const String& path) {
    try {
        return boost::filesystem::is_directory(path.c_str());
    } catch (...) {
        return false;
    }
}

bool listDirectory(const String& path, bool filesOnly, HashSet<String> dirList) {
    try {
        for (boost::filesystem::directory_iterator dir(path.c_str()); dir != boost::filesystem::directory_iterator(); ++dir) {
            if (!filesOnly || !boost::filesystem::is_directory(dir->status())) {
                dirList.add(dir->path().filename().wstring().c_str());
            }
        }
        return true;
    } catch (...) {
        return false;
    }
}

bool copyDirectory(const String& source, const String& dest) {
    try {
        HashSet<String> dirList(HashSet<String>::newInstance());
        if (!listDirectory(source, true, dirList)) {
            return false;
        }

        createDirectory(dest);

        for (HashSet<String>::iterator file = dirList.begin(); file != dirList.end(); ++file) {
            copyFile(joinPath(source, *file), joinPath(dest, *file));
        }

        return true;
    } catch (...) {
        return false;
    }
}

String joinPath(const String& path, const String& file) {
    try {
        boost::filesystem::path join(path.c_str());
        join /= file.c_str();
        return join.wstring().c_str();
    } catch (...) {
        return path;
    }
}

String extractPath(const String& path) {
    try {
        boost::filesystem::wpath parentPath(path.c_str());
        return parentPath.parent_path().wstring().c_str();
    } catch (...) {
        return path;
    }
}

String extractFile(const String& path) {
    try {
        boost::filesystem::wpath fileName(path.c_str());
        return fileName.filename().wstring().c_str();
    } catch (...) {
        return path;
    }
}

}
}
