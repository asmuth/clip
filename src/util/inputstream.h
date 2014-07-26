/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_INPUTSTREAM_H
#define _FNORDMETRIC_INPUTSTREAM_H
#include <memory>

namespace fnordmetric {
namespace util {

class InputStream {
public:
  InputStream() {}
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

};

class FileInputStream : public InputStream {
public:
  enum kByteOrderMark {
    BOM_UNKNOWN,
    BOM_UTF8
  };

  /**
   * Open a new file input stream from the provided file path. Throws an
   * exception if the file cannot be opened.
   *
   * @param file_path the path to the file
   */
  static std::unique_ptr<FileInputStream> openFile(
      const std::string& file_path);

  /**
   * Create a new FileInputStream instance from the provided filedescriptor. The
   * input stream takes ownership of the fd and will close() it when destructed.
   *
   * @param fd a valid an opened fd, transfer ownership and close on destruct
   */
  explicit FileInputStream(int fd);

  /**
   * Read the next byte from the file. Returns true if the next byte was read
   * and false if the end of the stream was reached.
   *
   * @param target the target char pointer
   */
  bool readNextByte(char* target) override;

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
};

}
}
#endif
