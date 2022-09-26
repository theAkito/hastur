// SPDX-FileCopyrightText: 2022 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: BSD-2-Clause

#include "util/mutex_protected.h"

#include "etest/etest.h"

#include <string>
#include <utility>

using etest::expect_eq;
using util::MutexProtected;

namespace {

class TestClass {
public:
    TestClass(std::string a, int b, double c) : a_{std::move(a)}, b_{b}, c_{c} {}

    std::string const &a() const { return a_; }
    int b() const { return b_; }
    double c() const { return c_; }

private:
    std::string a_;
    int b_;
    double c_;
};

struct TestStruct {
    std::string a{};
    int b{};
    double c{};
};

} // namespace

int main() {
    etest::test("int", [] {
        MutexProtected<int> v;
        expect_eq(v.copy_value(), 0);
        v.with_lock([](auto &value) { value = 13; });
        expect_eq(v.copy_value(), 13);
    });

    etest::test("3-arg c-tor", [] {
        MutexProtected<TestClass> v{"hello", 10, 32.};
        expect_eq(v.with_lock([](auto const &value) { return value.c(); }), 32.);
    });

    etest::test("no c-tor", [] {
        MutexProtected<TestStruct> v1{};
        expect_eq(v1.with_lock([](auto const &value) { return value.c; }), 0.);
        MutexProtected<TestStruct> v2{"hello", 12};
        expect_eq(v2.with_lock([](auto const &value) { return value.b; }), 12);
    });

    return etest::run_all_tests();
}
