/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
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
#ifndef _plotfx_UTIL_UNITTEST_H
#define _plotfx_UTIL_UNITTEST_H

#include <functional>
#include <unordered_map>
#include <vector>
#include <string>
#include <string.h>

#include "utils/exception.h"
#include "utils/fileutil.h"
#include "utils/inputstream.h"
#include "utils/outputstream.h"
#include "utils/inspect.h"
#include "utils/random.h"
#include "utils/return_code.h"

const char kExpectationFailed[] = "ExpectationFailed";

#define UNIT_TEST(T) \
    static plotfx::test::UnitTest T(#T); \
    int main() { \
      auto& t = T; \
      return t.run(); \
    }

#define TEST_CASE(T, N, L) \
    static plotfx::test::UnitTest::TestCase __##T##__case__##N(&T, #N, (L));

#define TEST_INITIALIZER(T, N, L) \
    static plotfx::test::UnitTest::TestInitializer __##T##__case__##N( \
        &T, (L));

#define EXPECT(X) \
    if (!(X)) { \
      RAISE( \
          kExpectationFailed, \
          "expectation failed: %s", #X); \
    }


void EXPECT_TRUE(bool val) {
  if (!val) {
    RAISE(
        kExpectationFailed,
        "expectation failed: expected TRUE, got FALSE");
  }
}

void EXPECT_FALSE(bool val) {
  if (val) {
    RAISE(
        kExpectationFailed,
        "expectation failed: expected FALSE, got TRUE");
  }
}

void CHECK_RC(plotfx::Status rc) {
  if (rc != plotfx::OK) {
    std::exit(1);
  }
}

void CHECK_RC(plotfx::ReturnCode rc) {
  if (!rc.isSuccess()) {
    RAISE(
        kExpectationFailed,
        rc.getMessage());
  }
}

template <typename T1, typename T2>
void EXPECT_EQ(T1 left, T2 right) {
  if (!(left == right)) {
    RAISE(
        kExpectationFailed,
        "expectation failed: %s == %s",
        plotfx::inspect<T1>(left).c_str(),
        plotfx::inspect<T2>(right).c_str());
  }
}

#define EXPECT_EXCEPTION(E, L) \
    { \
      bool raised = false; \
      try { \
        L(); \
      } catch (plotfx::Exception e) { \
        raised = true; \
        auto msg = e.getMessage().c_str(); \
        if (strcmp(msg, E) != 0) { \
          RAISE( \
              kExpectationFailed, \
              "excepted exception '%s' but got '%s'", E, msg); \
        } \
      } \
      if (!raised) { \
        RAISE( \
            kExpectationFailed, \
            "excepted exception '%s' but got no exception", E); \
      } \
    }

#define EXPECT_FILES_EQ(F1, F2) \
  { \
    auto one = plotfx::FileInputStream::openFile(F1); \
    auto two = plotfx::FileInputStream::openFile(F2); \
    std::string one_str; \
    std::string two_str; \
    one->readUntilEOF(&one_str); \
    two->readUntilEOF(&two_str); \
    if (one_str != two_str) { \
      std::string filename1(F1); \
      std::string filename2(F2); \
      RAISE( \
          kExpectationFailed, \
          "expected files '%s' and '%s' to be equal, but the differ", \
          filename1.c_str(), filename2.c_str()); \
    } \
  }


namespace plotfx {
namespace test {

class UnitTest {
public:

  static std::string tempFilePath() {
    return "/tmp/_libfnord_test_tmp/";
  }

  static std::string testDataPath() {
    return "./";
  }

  class TestCase {
  public:
    TestCase(
        UnitTest* test,
        const char* name,
        std::function<void ()> lambda) :
        name_(name),
        lambda_(lambda) {
      test->addTestCase(this);
    }

    const char* name_;
    std::function<void ()> lambda_;
  };

  class TestInitializer {
  public:
    TestInitializer(
        UnitTest* test,
        std::function<void ()> lambda) :
        lambda_(lambda) {
      test->addInitializer(this);
    }

    std::function<void ()> lambda_;
  };

  UnitTest(const char* name) : name_(name) {}

  void addTestCase(const TestCase* test_case) {
    cases_.push_back(test_case);
  }

  void addInitializer(const TestInitializer* init) {
    initializers_.push_back(init);
  }

  int run() {
    plotfx::FileUtil::mkdir_p(UnitTest::tempFilePath());

    for (auto initializer : initializers_) {
      initializer->lambda_();
    }

    fprintf(stderr, "%s\n", name_);

    const TestCase* current_test_case = nullptr;
    int num_tests_passed = 0;
    std::unordered_map<const TestCase*, plotfx::Exception> errors;

    for (auto test_case : cases_) {
      fprintf(stderr, "    %s::%s", name_, test_case->name_);
      fflush(stderr);
      current_test_case = test_case;

      try {
        test_case->lambda_();
      } catch (plotfx::Exception e) {
        fprintf(stderr, " \033[1;31m[FAIL]\e[0m\n");
        errors.emplace(test_case, e);
        continue;
      }

      num_tests_passed++;
      fprintf(stderr, " \033[1;32m[PASS]\e[0m\n");
    }

    if (num_tests_passed != cases_.size()) {
      for (auto test_case : cases_) {
        const auto& err = errors.find(test_case);

        if (err != errors.end()) {
          fprintf(
              stderr,
              "\n\033[1;31m[FAIL] %s::%s\e[0m\n",
              name_,
              test_case->name_);
          err->second.debugPrint();
        }
      }

      fprintf(stderr, 
          "\n\033[1;31m[FAIL] %i/%i tests failed :(\e[0m\n",
          (int) cases_.size() - num_tests_passed,
          (int) cases_.size());
      return 1;
    }

    return 0;
  }

protected:
  const char* name_;
  std::vector<const TestCase*> cases_;
  std::vector<const TestInitializer*> initializers_;
};

}
}
#endif
