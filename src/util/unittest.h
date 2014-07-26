/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_UTIL_UNITTEST_H
#define _FNORDMETRIC_UTIL_UNITTEST_H
#include "runtimeexception.h"
#include <functional>
#include <unordered_map>
#include <vector>
#include <string>

namespace fnordmetric {
namespace util {

#define UNIT_TEST(T) \
    static fnordmetric::util::UnitTest T(#T); \
    int main() { \
      auto& t = T; \
      return t.run(); \
    }

#define TEST_CASE(T, N, L) \
    static fnordmetric::util::UnitTest::TestCase __##T##__case__##N(&T, #N, (L));

#define TEST_INITIALIZER(T, N, L) \
    static fnordmetric::util::UnitTest::TestInitializer __##T##__case__##N( \
        &T, (L));

#define EXPECT(X) \
    if (!(X)) { \
      RAISE( \
          fnordmetric::util::UnitTest::ExpectationFailed, \
          "expectation failed: %s", #X); \
    }

#define EXPECT_EQ(X, Y) \
    EXPECT((X) == (Y));

#define EXPECT_EXCEPTION(E, L) \
    { \
      bool raised = false; \
      try { \
        L(); \
      } catch (fnordmetric::util::RuntimeException e) { \
        raised = true; \
        auto msg = e.getMessage().c_str(); \
        if (strcmp(msg, E) != 0) { \
          RAISE( \
              fnordmetric::util::UnitTest::ExpectationFailed, \
              "excepted exception '%s' but got '%s'", E, msg); \
        } \
      } \
      if (!raised) { \
        RAISE( \
            fnordmetric::util::UnitTest::ExpectationFailed, \
            "excepted exception '%s' but got no exception", E); \
      } \
    }

class UnitTest {
public:
  static const int kExpectationFailed = 0;
  struct ExpectationFailed : public RuntimeException {
    template <typename... T>
    ExpectationFailed(
        const char* message, T... args) :
        RuntimeException(message, args...) {}
  };

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
    for (auto initializer : initializers_) {
      initializer->lambda_();
    }

    fprintf(stderr, "%s\n", name_);

    const TestCase* current_test_case = nullptr;
    int num_tests_passed = 0;
    std::unordered_map<const TestCase*, RuntimeException> errors;

    for (auto test_case : cases_) {
      fprintf(stderr, "    %s::%s", name_, test_case->name_);
      fflush(stderr);
      current_test_case = test_case;

      try {
        test_case->lambda_();
      } catch (fnordmetric::util::RuntimeException e) {
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
              "\n\033[1;31m[FAIL] %s::%s\e[0m",
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
