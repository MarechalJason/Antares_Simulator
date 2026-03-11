// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <optional>
#include <type_traits>
#include <utility>

#include "antares/exception/InputError.hpp"

namespace Antares
{

template<typename T>
class Result
{
public:
    using ValueType = T;
    using ErrorType = Error::InputError;

    static Result ok(T value)
    {
        Result r;
        r.value_ = std::move(value);
        r.is_ok_ = true;
        return r;
    }

    static Result err(Error::InputError error)
    {
        Result r;
        r.error_ = std::move(error);
        r.is_ok_ = false;
        return r;
    }

    Result() = default;

    bool ok() const noexcept
    {
        return is_ok_;
    }

    bool err() const noexcept
    {
        return !is_ok_;
    }

    const T& value() const&
    {
        return value_.value();
    }

    T&& value() &&
    {
        return std::move(value_.value());
    }

    const Error::InputError& error() const&
    {
        return error_.value();
    }

    Error::InputError&& error() &&
    {
        return std::move(error_.value());
    }

    explicit operator bool() const noexcept
    {
        return is_ok_;
    }

    template<typename F>
    auto and_then(F&& func) -> Result<decltype(func(std::declval<T>()))>
    {
        using U = decltype(func(std::declval<T>()));
        if (is_ok_)
        {
            return func(value_.value());
        }
        return Result<U>::err(error_.value());
    }

    template<typename F>
    auto map(F&& func) -> Result<decltype(func(std::declval<T>()))>
    {
        using U = decltype(func(std::declval<T>()));
        if (is_ok_)
        {
            return Result<U>::ok(func(value_.value()));
        }
        return Result<U>::err(error_.value());
    }

    template<typename F>
    auto map_err(F&& func) -> Result<T>
    {
        if (is_ok_)
        {
            return *this;
        }
        return Result<T>::err(func(error_.value()));
    }

    T value_or(T default_value) const
    {
        if (is_ok_)
        {
            return value_.value();
        }
        return default_value;
    }

    template<typename U>
    Result<U> map_to_err() const
    {
        if (is_ok_)
        {
            return Result<U>::ok(U{});
        }
        return Result<U>::err(error_.value());
    }

private:
    bool is_ok_ = false;
    std::conditional_t<std::is_object_v<T>, std::optional<T>, std::monostate> value_;
    std::optional<Error::InputError> error_;
};

template<typename T>
class Result<T&>
{
public:
    using ValueType = T&;
    using ErrorType = Error::InputError;

    static Result ok(T& value)
    {
        Result r;
        r.ptr_ = std::addressof(value);
        r.is_ok_ = true;
        return r;
    }

    static Result err(Error::InputError error)
    {
        Result r;
        r.error_ = std::move(error);
        r.is_ok_ = false;
        return r;
    }

    bool ok() const noexcept
    {
        return is_ok_;
    }

    bool err() const noexcept
    {
        return !is_ok_;
    }

    T& value() const
    {
        return *ptr_;
    }

    const Error::InputError& error() const
    {
        return error_.value();
    }

    explicit operator bool() const noexcept
    {
        return is_ok_;
    }

    template<typename F>
    auto and_then(F&& func) -> Result<decltype(func(std::declval<T&>()))>
    {
        using U = decltype(func(std::declval<T&>()));
        if (is_ok_)
        {
            return func(*ptr_);
        }
        return Result<U>::err(error_.value());
    }

    template<typename F>
    auto map(F&& func) -> Result<decltype(func(std::declval<T&>()))>
    {
        using U = decltype(func(std::declval<T&>()));
        if (is_ok_)
        {
            return Result<U>::ok(func(*ptr_));
        }
        return Result<U>::err(error_.value());
    }

    T& value_or(T& default_value) const
    {
        if (is_ok_)
        {
            return *ptr_;
        }
        return default_value;
    }

private:
    bool is_ok_ = false;
    T* ptr_ = nullptr;
    std::optional<Error::InputError> error_;
};

template<>
class Result<void>
{
public:
    using ValueType = void;
    using ErrorType = Error::InputError;

    static Result ok()
    {
        Result r;
        r.is_ok_ = true;
        return r;
    }

    static Result err(Error::InputError error)
    {
        Result r;
        r.error_ = std::move(error);
        r.is_ok_ = false;
        return r;
    }

    bool ok() const noexcept
    {
        return is_ok_;
    }

    bool err() const noexcept
    {
        return !is_ok_;
    }

    const Error::InputError& error() const
    {
        return error_.value();
    }

    explicit operator bool() const noexcept
    {
        return is_ok_;
    }

    template<typename F>
    auto and_then(F&& func) -> Result<decltype(func())>
    {
        using U = decltype(func());
        if (is_ok_)
        {
            return func();
        }
        return Result<U>::err(error_.value());
    }

    template<typename F>
    auto map(F&& func) -> Result<decltype(func())>
    {
        using U = decltype(func());
        if (is_ok_)
        {
            return Result<U>::ok(func());
        }
        return Result<U>::err(error_.value());
    }

private:
    bool is_ok_ = false;
    std::optional<Error::InputError> error_;
};

} // namespace Antares
