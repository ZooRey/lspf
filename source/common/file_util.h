// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
// This is a public header file, it must only include public header files.

#ifndef __BASE_FILEUTIL_H
#define __BASE_FILEUTIL_H

#include <string>

// not thread safe
class AppendFile
{
 public:
  explicit AppendFile(const std::string &filename);

  ~AppendFile();

  void append(const char* logline, const size_t len);

  void flush();

  size_t writtenBytes() const { return writtenBytes_; }

 private:

  size_t write(const char* logline, size_t len);

  FILE* fp_;
  char buffer_[64*1024];
  size_t writtenBytes_;
};


#endif  // MUDUO_BASE_FILEUTIL_H

