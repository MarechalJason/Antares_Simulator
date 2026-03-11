// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

namespace Antares::Error
{

enum class InputErrorKind
{
    MissingField,
    InvalidValue,
    ParseError,
    FileNotFound,
    ValidationError,
    NotFound,
    AlreadyExists,
    TypeMismatch,
    ConstraintViolation,
    Unknown
};

struct InputError
{
    InputErrorKind kind;
    std::string message;
    std::optional<std::string> file;
    std::optional<size_t> line;
    std::optional<size_t> column;

    InputError() = default;

    InputError(InputErrorKind kind_, std::string message_):
        kind(kind_),
        message(std::move(message_))
    {
    }

    InputError(InputErrorKind kind_, std::string message_, std::string file_):
        kind(kind_),
        message(std::move(message_)),
        file(std::move(file_))
    {
    }

    InputError(InputErrorKind kind_,
               std::string message_,
               std::string file_,
               size_t line_,
               size_t column_):
        kind(kind_),
        message(std::move(message_)),
        file(std::move(file_)),
        line(line_),
        column(column_)
    {
    }

    std::string toString() const;

    static InputError missingField(std::string_view fieldName, std::string_view context = {});
    static InputError invalidValue(std::string_view value,
                                   std::string_view reason,
                                   std::string_view context = {});
    static InputError parseError(std::string_view message,
                                 std::string_view file = {},
                                 size_t line = 0,
                                 size_t column = 0);
    static InputError fileNotFound(std::string_view path);
    static InputError notFound(std::string_view what, std::string_view inContext = {});
    static InputError alreadyExists(std::string_view what, std::string_view inContext = {});
    static InputError typeMismatch(std::string_view expected, std::string_view actual);
    static InputError validationError(std::string_view message);

    std::runtime_error toRuntimeError() const;
    std::invalid_argument toInvalidArgumentError() const;
};

const char* toString(InputErrorKind kind);

} // namespace Antares::Error
