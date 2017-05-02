/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _libstx_INPUTSTREAM_H
#define _libstx_INPUTSTREAM_H
#include <memory>
#include <string>
#include "buffer.h"
#include "autoref.h"
#include "file.h"

class FileInputStream;

class InputStream : public RefCounted {
public:

  /**
   * Get the stdin input stream
   */
  static std::unique_ptr<FileInputStream> getStdin();

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
   * Check if the end of this input stream was reached. Returns true if the
   * end was reached, false otherwise
   */
  virtual bool eof() = 0;

  /**
   * Read N bytes from the stream and copy the data into the provided string,
   * appending after any existing data if the string is not empty.
   *
   * Returns the number of bytes read.
   *
   * @param target the string to copy the data into
   * @param n_bytes the number of bytes to read
   */
  size_t readNextBytes(std::string* target, size_t n_bytes);

  /**
   * Read N bytes from the stream and copy the data into the provided buffer
   * Returns the number of bytes read.
   *
   * @param target the string to copy the data into
   * @param n_bytes the number of bytes to read
   */
  virtual size_t readNextBytes(void* target, size_t n_bytes);

  /**
   * Skip the next N bytes in the stream. Returns the number of bytes skipped.
   *
   * @param n_bytes the number of bytes to skip
   */
  virtual size_t skipNextBytes(size_t n_bytes) = 0;

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
   * Read from the stream until the next '\n' character and copy the data into
   * the provided string. Returns the number of bytes read.
   *
   * WARNING: this method might never return if the input stream does not
   * contain '\n' characters and does not have an EOF!
   *
   * Returns false if the EOF was reached and true if it wasn't.
   *
   * @param target the string to copy the data into
   */
  virtual bool readLine(std::string* target);

  /**
   * Reads a uint8 from the stream. Throws an exception on error
   */
  virtual uint8_t readUInt8();

  /**
   * Reads a uint16 from the stream. Throws an exception on error
   */
  virtual uint16_t readUInt16();

  /**
   * Reads a uint32 from the stream. Throws an exception on error
   */
  virtual uint32_t readUInt32();

  /**
   * Reads a uint64 from the stream. Throws an exception on error
   */
  virtual uint64_t readUInt64();

  /**
   * Reads a LEB128 encoded uint64 from the stream. Throws an exception on error
   */
  virtual uint64_t readVarUInt();

  /**
   * Reads a string from the stream. Throws an exception on error
   */
  virtual String readString(size_t size);

  /**
   * Reads a LEB128 prefix-length-encoded string from the stream. Throws an
   * exception on error
   */
  virtual String readLenencString();

  /**
   * Reads a IEEE754 encoded double from the stream. Throws an exception on
   * error
   */
  virtual double readDouble();

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

  /**
   * Seek to the provided offset in number of bytes from the beginning of the
   * stream
   */
  virtual void seekTo(size_t offset) = 0;

};

class FileInputStream : public RewindableInputStream {
public:
  enum kByteOrderMark {
    BOM_UNKNOWN,
    BOM_UTF8,
    BOM_UTF16
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
  static std::unique_ptr<FileInputStream> fromFileDescriptor(
      int fd,
      bool close_on_destroy = false);

  /**
   * Create a new FileInputStream instance from the provided File.
   *
   * @param file the opened file
   */
  static std::unique_ptr<FileInputStream> fromFile(File&& file);

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
   * Create a new FileInputStream instance from the provided File.
   *
   * @param file the opened file
   */
  explicit FileInputStream(File&& file);

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
   * Read N bytes from the stream and copy the data into the provided buffer
   * Returns the number of bytes read.
   *
   * @param target the string to copy the data into
   * @param n_bytes the number of bytes to read
   */
  size_t readNextBytes(void* target, size_t n_bytes) override;

  /**
   * Skip the next N bytes in the stream. Returns the number of bytes skipped.
   *
   * @param n_bytes the number of bytes to skip
   */
  size_t skipNextBytes(size_t n_bytes) override;

  /**
   * Check if the end of this input stream was reached. Returns true if the
   * end was reached, false otherwise
   */
  bool eof() override;

  /**
   * Rewind the input stream
   */
  void rewind() override;

  /**
   * Seek to the provided offset in number of bytes from the beginning of the
   * file. Raises en exception if the provided offset is out of bounds of the
  * underlying file
   */
  void seekTo(size_t offset) override;

  /**
   * Returns true if this input stream is connected to a TTY/terminal and false
   * otherwise
   */
  bool isTTY() const;

  /**
   * Read the byte order mark of the file
   */
  kByteOrderMark readByteOrderMark();

protected:
  bool readNextChunk();
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
   * Skip the next N bytes in the stream. Returns the number of bytes skipped.
   *
   * @param n_bytes the number of bytes to skip
   */
  size_t skipNextBytes(size_t n_bytes) override;

  /**
   * Check if the end of this input stream was reached. Returns true if the
   * end was reached, false otherwise
   */
  bool eof() override;

  /**
   * Rewind the input stream
   */
  void rewind() override;

  /**
   * Seek to the provided offset in number of bytes from the beginning of the
   * string. Sets the position to EOF if the provided offset is larger than
   * the underlying string's size
   */
  void seekTo(size_t offset) override;

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
   * Skip the next N bytes in the stream. Returns the number of bytes skipped.
   *
   * @param n_bytes the number of bytes to skip
   */
  size_t skipNextBytes(size_t n_bytes) override;

  /**
   * Check if the end of this input stream was reached. Returns true if the
   * end was reached, false otherwise
   */
  bool eof() override;

  /**
   * Rewind the input stream
   */
  void rewind() override;

  /**
   * Seek to the provided offset in number of bytes from the beginning of the
   * buffer. Sets the position to EOF if the provided offset is larger than
   * the underlying buffer's size
   */
  void seekTo(size_t offset) override;

protected:
  const Buffer* buf_;
  size_t cur_;
};

class MemoryInputStream : public RewindableInputStream {
public:

  /**
   * Create a new InputStream from the provided string
   *
   * @param string the input string
   */
  MemoryInputStream(const void* data, size_t size);

  /**
   * Read the next byte from the file. Returns true if the next byte was read
   * and false if the end of the stream was reached.
   *
   * @param target the target char pointer
   */
  bool readNextByte(char* target) override;

  /**
   * Skip the next N bytes in the stream. Returns the number of bytes skipped.
   *
   * @param n_bytes the number of bytes to skip
   */
  size_t skipNextBytes(size_t n_bytes) override;

  /**
   * Check if the end of this input stream was reached. Returns true if the
   * end was reached, false otherwise
   */
  bool eof() override;

  /**
   * Rewind the input stream
   */
  void rewind() override;

  /**
   * Seek to the provided offset in number of bytes from the beginning of the
   * pointed to memory region. Sets the position to EOF if the provided offset
   * is larger than size of the underlying memory regio
   */
  void seekTo(size_t offset) override;

protected:
  const void* data_;
  size_t size_;
  size_t cur_;
};

#endif
