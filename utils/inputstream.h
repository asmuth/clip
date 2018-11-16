/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _signaltk_INPUTSTREAM_H
#define _signaltk_INPUTSTREAM_H
#include <memory>
#include <string>
#include "buffer.h"

namespace signaltk {

class InputStream {
public:

  /**
   * Get the stdin input stream
   */
  static std::unique_ptr<InputStream> getStdin();

  InputStream(const std::string& filename = "<anonymous input stream>");
  InputStream(const InputStream& other) = delete;
  InputStream& operator=(const InputStream& other) = delete;
  virtual ~InputStream() {}

  /**
   * Read the next byte from the input stream. Returns true if the next byte was
   * read and false if the end of the stream was reached.
   *
   * @param target the target char pointer
   */
  virtual bool readNextByte(char* target) = 0;

  /**
   * Read N bytes from the stream and copy the data into the provided string.
   * Returns the number of bytes read.
   *
   * @param target the string to copy the data into
   */
  virtual size_t readNextBytes(std::string* target, size_t n_bytes);

  /**
   * Read N bytes from the stream and copy the data into the provided buffer
   * Returns the number of bytes read.
   *
   * @param target the string to copy the data into
   */
  virtual size_t readNextBytes(Buffer* target, size_t n_bytes);

  /**
   * Read from the stream until EOF and copy the data into the provided string.
   * Returns the number of bytes read.
   *
   * WARNING: this method might never return if the input stream does not have
   * an EOF!
   *
   * @param target the string to copy the data into
   */
  virtual size_t readUntilEOF(std::string* target);

  /**
   * Return the input stream filename
   */
  const std::string& getFileName() const;

  /**
   * Set the input stream filename
   */
  void setFileName(const std::string& filename);

private:
  std::string filename_;
};

class RewindableInputStream : public InputStream {
public:

  /**
   * Rewind the input stream
   */
  virtual void rewind() = 0;

};

class FileInputStream : public RewindableInputStream {
public:
  enum kByteOrderMark {
    BOM_UNKNOWN,
    BOM_UTF8
  };

  /**
   * Open a new file input stream from the provided file path. Throws an
   * exception if the file cannot be opened. The file will be automatically
   * closed on destroy.
   *
   * @param file_path the path to the file
   */
  static std::unique_ptr<FileInputStream> openFile(
      const std::string& file_path);

  /**
   * Create a new FileInputStream instance from the provided filedescriptor. If
   * close on_destroy is true, the fd will be close()ed when the input stream
   * is destroyed.
   *
   * @param fd a valid fd
   * @param close_on_destroy close the fd on destroy?
   */
  explicit FileInputStream(int fd, bool close_on_destroy = false);

  /**
   * Close the fd if close_on_destroy is true
   */
  ~FileInputStream();

  /**
   * Read the next byte from the file. Returns true if the next byte was read
   * and false if the end of the stream was reached.
   *
   * @param target the target char pointer
   */
  bool readNextByte(char* target) override;

  /**
   * Rewind the input stream
   */
  void rewind() override;

  /**
   * Read the byte order mark of the file
   */
  kByteOrderMark readByteOrderMark();

protected:
  void readNextChunk();
  char buf_[8192]; // FIXPAUL make configurable
  size_t buf_len_;
  size_t buf_pos_;
  int fd_;
  bool close_on_destroy_;
};

class StringInputStream : public RewindableInputStream {
public:

  /**
   * Create a new InputStream from the provided string
   *
   * @param string the input string
   */
  static std::unique_ptr<StringInputStream> fromString(
      const std::string& string);

  /**
   * Create a new InputStream from the provided string
   *
   * @param string the input string
   */
  StringInputStream(const std::string& string);

  /**
   * Read the next byte from the file. Returns true if the next byte was read
   * and false if the end of the stream was reached.
   *
   * @param target the target char pointer
   */
  bool readNextByte(char* target) override;

  /**
   * Rewind the input stream
   */
  void rewind() override;

protected:
  std::string str_;
  size_t cur_;
};

class BufferInputStream : public RewindableInputStream {
public:

  /**
   * Create a new InputStream from the provided buffer
   *
   * @param string the input string
   */
  static std::unique_ptr<BufferInputStream> fromBuffer(const Buffer* buffer);

  /**
   * Create a new InputStream from the provided string
   *
   * @param string the input string
   */
  BufferInputStream(const Buffer* buffer);

  /**
   * Read the next byte from the file. Returns true if the next byte was read
   * and false if the end of the stream was reached.
   *
   * @param target the target char pointer
   */
  bool readNextByte(char* target) override;

  /**
   * Rewind the input stream
   */
  void rewind() override;

protected:
  const Buffer* buf_;
  size_t cur_;
};

}
#endif
