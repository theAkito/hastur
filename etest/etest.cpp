// SPDX-FileCopyrightText: 2021-2022 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: BSD-2-Clause

#include "etest/etest.h"

#include <algorithm>
#include <exception>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

namespace etest {
namespace {

int assertion_failures = 0;
int disabled_tests = 0;

struct Test {
    std::string name;
    std::function<void()> body;
};

std::vector<Test> &registry() {
    static std::vector<Test> test_registry;
    return test_registry;
}

struct test_failure : public std::exception {};

std::stringstream test_log{};

} // namespace

int run_all_tests() noexcept {
    std::cout << registry().size() + disabled_tests << " test(s) registered";
    if (disabled_tests == 0) {
        std::cout << "." << std::endl;
    } else {
        std::cout << ", " << disabled_tests << " disabled." << std::endl;
    }

    auto const longest_name = std::max_element(registry().begin(), registry().end(), [](auto const &a, auto const &b) {
        return a.name.size() < b.name.size();
    });

    for (auto const &test : registry()) {
        std::cout << std::left << std::setw(longest_name->name.size()) << test.name << ": ";

        int const before = assertion_failures;

#ifdef __EXCEPTIONS
        try {
            test.body();
        } catch (test_failure const &) {
            ++assertion_failures;
        } catch (std::exception const &e) {
            ++assertion_failures;
            test_log << "Unhandled exception in test body: " << e.what() << '\n';
        } catch (...) {
            ++assertion_failures;
            test_log << "Unhandled unknown exception in test body.\n";
        }
#else
        test.body();
#endif

        if (before == assertion_failures) {
            std::cout << "\u001b[32mPASSED\u001b[0m\n";
        } else {
            std::cout << "\u001b[31;1mFAILED\u001b[0m\n";
            std::cout << std::exchange(test_log, {}).str();
        }
    }

    return assertion_failures > 0 ? 1 : 0;
}

void test(std::string name, std::function<void()> body) noexcept {
    // TODO(robinlinden): push_back -> emplace_back once Clang supports it (C++20/p0960). Not supported as of Clang 13.
    registry().push_back({std::move(name), std::move(body)});
}

// TODO(robinlinden): Save and allow running these by passing some flag.
void disabled_test(std::string, std::function<void()>) noexcept {
    ++disabled_tests;
}

void expect(bool b, etest::source_location const &loc) noexcept {
    if (!b) {
        ++assertion_failures;
        // Check if we're using the real source_location by checking for line == 0.
        if (loc.line() != 0) {
            test_log << "  expectation failure at " << loc.file_name() << "(" << loc.line() << ":" << loc.column()
                     << ")\n";
        }
    }
}

void require(bool b, etest::source_location const &loc) {
    if (!b) {
        if (loc.line() != 0) {
            test_log << "  requirement failure at " << loc.file_name() << "(" << loc.line() << ":" << loc.column()
                     << ")\n";
        }
#ifdef __EXCEPTIONS
        throw test_failure{};
#else
        std::terminate();
#endif
    }
}

std::ostream &log() {
    return test_log;
}

} // namespace etest
