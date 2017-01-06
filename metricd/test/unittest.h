/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <functional>
#include <unordered_map>
#include <vector>
#include <string>
#include <string.h>

#define UNIT_TEST(T) \
    static libtransport::UnitTest T(#T); \
    int main() { \
      auto& t = T; \
      return t.run(); \
    }

#define TEST_CASE(T, N, L) \
    static libtransport::UnitTest::TestCase __##T##__case__##N(&T, #N, (L));

#define TEST_INITIALIZER(T, N, L) \
    static libtransport::UnitTest::TestInitializer __##T##__case__##N( \
        &T, (L));

#define EXPECT(X) \
    if (!(X)) { \
      throw std::runtime_error(std::string("expectation failed: ") + #X); \
    }

#define EXPECT_EQ(A, B) EXPECT((A) == (B))

#define EXPECT_TRUE(X) EXPECT(!!(X))
#define EXPECT_FALSE(X) EXPECT(!(X))

namespace libtransport {

class UnitTest {
public:

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
    std::unordered_map<const TestCase*, std::string> errors;

    for (auto test_case : cases_) {
      fprintf(stderr, "    %s::%s", name_, test_case->name_);
      fflush(stderr);
      current_test_case = test_case;

      try {
        test_case->lambda_();
      } catch (const std::exception& e) {
        fprintf(stderr, " \033[1;31m[FAIL]\e[0m\n");
        errors.emplace(test_case, e.what());
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
              "\n\033[1;31m[FAIL] %s::%s\e[0m\n%s\n",
              name_,
              test_case->name_,
              err->second.c_str());
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

} // namespace libtransport

