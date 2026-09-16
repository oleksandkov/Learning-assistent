#pragma once

#include <iostream>
#include <string>
#include <exception>
#include <vector>
#include <functional>

namespace sokoban::test {

struct TestCase {
    std::string suite;
    std::string name;
    std::function<void()> func;
};

class TestRunner {
public:
    static TestRunner& instance() {
        static TestRunner r;
        return r;
    }

    void addTest(std::string suite, std::string name, std::function<void()> func) {
        tests_.push_back({std::move(suite), std::move(name), std::move(func)});
    }

    int runAll() {
        int passed = 0;
        int failed = 0;

        std::cout << "\n================== SOKOBAN TEST SUITE ==================\n";
        for (const auto& tc : tests_) {
            std::cout << "[ RUN      ] " << tc.suite << "::" << tc.name << "\n";
            try {
                tc.func();
                std::cout << "[       OK ] " << tc.suite << "::" << tc.name << "\n";
                passed++;
            } catch (const std::exception& e) {
                std::cout << "[  FAILED  ] " << tc.suite << "::" << tc.name << " -> Exception: " << e.what() << "\n";
                failed++;
            } catch (...) {
                std::cout << "[  FAILED  ] " << tc.suite << "::" << tc.name << " -> Unknown exception\n";
                failed++;
            }
        }
        std::cout << "========================================================\n";
        std::cout << "Total tests: " << tests_.size() << " | Passed: " << passed << " | Failed: " << failed << "\n\n";

        return (failed == 0) ? 0 : 1;
    }

private:
    std::vector<TestCase> tests_;
};

#define SOKOBAN_TEST(suiteName, testName) \
    static void test_##suiteName##_##testName(); \
    namespace { \
        struct Reg_##suiteName##_##testName { \
            Reg_##suiteName##_##testName() { \
                sokoban::test::TestRunner::instance().addTest(#suiteName, #testName, test_##suiteName##_##testName); \
            } \
        } global_reg_##suiteName##_##testName; \
    } \
    static void test_##suiteName##_##testName()

#define SOKOBAN_ASSERT(cond) \
    do { \
        if (!(cond)) { \
            throw std::runtime_error(std::string("Assertion failed: ") + #cond + " at " + __FILE__ + ":" + std::to_string(__LINE__)); \
        } \
    } while (0)

#define SOKOBAN_ASSERT_EQ(a, b) \
    do { \
        if ((a) != (b)) { \
            throw std::runtime_error(std::string("Assertion failed: ") + #a + " == " + #b + " at " + __FILE__ + ":" + std::to_string(__LINE__)); \
        } \
    } while (0)

#define SOKOBAN_ASSERT_THROWS(expr, ExceptionType) \
    do { \
        bool caught = false; \
        try { \
            expr; \
        } catch (const ExceptionType&) { \
            caught = true; \
        } catch (...) {} \
        if (!caught) { \
            throw std::runtime_error(std::string("Expected exception ") + #ExceptionType + " for " + #expr + " at " + __FILE__ + ":" + std::to_string(__LINE__)); \
        } \
    } while (0)

} // namespace sokoban::test
