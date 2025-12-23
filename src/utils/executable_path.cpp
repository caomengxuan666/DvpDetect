/*
 *  Copyright © 2025 [caomengxuan666]
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the “Software”), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 *  - File: executable_path.cpp
 *  - CreationYear: 2025
 *  - Date: Sat Dec 20 2025
 *  - Username: Administrator
 *  - CopyrightYear: 2025
 */

#include "utils/executable_path.h"

#include <string>

#ifdef _WIN32
#include <shlwapi.h>  // PathRemoveFileSpec
#include <windows.h>
#pragma comment(lib, "shlwapi.lib")
#else
#include <libgen.h>
#include <limits.h>
#include <unistd.h>

#include <string>

#endif

namespace DvpUtils {

std::string getExecutablePath() {
#ifdef _WIN32
  char path[MAX_PATH];
  if (GetModuleFileNameA(nullptr, path, MAX_PATH) == 0) {
    return "";
  }
  return std::string(path);
#else
  char result[PATH_MAX];
  ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
  return count != -1 ? std::string(result, count) : "";
#endif
}

std::string getExecutableDirectory() {
  std::string exec_path = getExecutablePath();
  if (exec_path.empty()) {
    return "";
  }

#ifdef _WIN32
  char dir[MAX_PATH];
  strcpy_s(dir, exec_path.c_str());
  PathRemoveFileSpecA(dir);
  return std::string(dir);
#else
  return std::string(dirname(const_cast<char *>(exec_path.c_str())));
#endif
}

}  // namespace DvpUtils
