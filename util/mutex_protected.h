// SPDX-FileCopyrightText: 2022 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: BSD-2-Clause

#ifndef UTIL_MUTEX_PROTECTED_H_
#define UTIL_MUTEX_PROTECTED_H_

#include <mutex>
#include <utility>

namespace util {

template<typename T>
class MutexProtected {
public:
    MutexProtected() = default;

    template<typename ...Args>
    explicit MutexProtected(Args && ...args) : protected_{std::forward<Args>(args)...} {}

    explicit MutexProtected(T &&value) : protected_{std::move(value)} {}

    decltype(auto) with_lock(auto action) {
        std::unique_lock lock{mutex_};
        return action(protected_);
    }

    T copy_value() const {
        std::unique_lock lock{mutex_};
        return protected_;
    }

private:
    T protected_{};
    mutable std::mutex mutex_{};
};

} // namespace util

#endif
