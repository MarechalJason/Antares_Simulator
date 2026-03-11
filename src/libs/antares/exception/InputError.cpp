// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/exception/InputError.hpp"

#include <fmt/core.h>

namespace Antares::Error
{

const char* toString(InputErrorKind kind)
{
    switch (kind)
    {
    case InputErrorKind::MissingField:
        return "MissingField";
    case InputErrorKind::InvalidValue:
        return "InvalidValue";
    case InputErrorKind::ParseError:
        return "ParseError";
    case InputErrorKind::FileNotFound:
        return "FileNotFound";
    case InputErrorKind::ValidationError:
        return "ValidationError";
    case InputErrorKind::NotFound:
        return "NotFound";
    case InputErrorKind::AlreadyExists:
        return "AlreadyExists";
    case InputErrorKind::TypeMismatch:
        return "TypeMismatch";
    case InputErrorKind::ConstraintViolation:
        return "ConstraintViolation";
    case InputErrorKind::Unknown:
    default:
        return "Unknown";
    }
}

std::string InputError::toString() const
{
    std::string result = fmt::format("[{}] {}", toString(kind), message);
    if (file)
    {
        result += fmt::format(" (file: {}", *file);
        if (line)
        {
            result += fmt::format(":{}", *line);
            if (column)
            {
                result += fmt::format(":{}", *column);
            }
        }
        result += ")";
    }
    return result;
}

InputError InputError::missingField(std::string_view fieldName, std::string_view context)
{
    InputError err;
    err.kind = InputErrorKind::MissingField;
    if (context.empty())
    {
        err.message = fmt::format("Missing required field: {}", fieldName);
    }
    else
    {
        err.message = fmt::format("Missing required field: {} in {}", fieldName, context);
    }
    return err;
}

InputError InputError::invalidValue(std::string_view value,
                                    std::string_view reason,
                                    std::string_view context)
{
    InputError err;
    err.kind = InputErrorKind::InvalidValue;
    if (context.empty())
    {
        err.message = fmt::format("Invalid value '{}': {}", value, reason);
    }
    else
    {
        err.message = fmt::format("Invalid value '{}' in {}: {}", value, context, reason);
    }
    return err;
}

InputError InputError::parseError(std::string_view message,
                                  std::string_view file,
                                  size_t line,
                                  size_t column)
{
    InputError err;
    err.kind = InputErrorKind::ParseError;
    err.message = std::string(message);
    if (!file.empty())
    {
        err.file = std::string(file);
        err.line = line;
        err.column = column;
    }
    return err;
}

InputError InputError::fileNotFound(std::string_view path)
{
    InputError err;
    err.kind = InputErrorKind::FileNotFound;
    err.message = fmt::format("File not found: {}", path);
    err.file = std::string(path);
    return err;
}

InputError InputError::notFound(std::string_view what, std::string_view inContext)
{
    InputError err;
    err.kind = InputErrorKind::NotFound;
    if (inContext.empty())
    {
        err.message = fmt::format("{} not found", what);
    }
    else
    {
        err.message = fmt::format("{} not found in {}", what, inContext);
    }
    return err;
}

InputError InputError::alreadyExists(std::string_view what, std::string_view inContext)
{
    InputError err;
    err.kind = InputErrorKind::AlreadyExists;
    if (inContext.empty())
    {
        err.message = fmt::format("{} already exists", what);
    }
    else
    {
        err.message = fmt::format("{} already exists in {}", what, inContext);
    }
    return err;
}

InputError InputError::typeMismatch(std::string_view expected, std::string_view actual)
{
    InputError err;
    err.kind = InputErrorKind::TypeMismatch;
    err.message = fmt::format("Type mismatch: expected {}, got {}", expected, actual);
    return err;
}

InputError InputError::validationError(std::string_view message)
{
    InputError err;
    err.kind = InputErrorKind::ValidationError;
    err.message = std::string(message);
    return err;
}

std::runtime_error InputError::toRuntimeError() const
{
    return std::runtime_error(toString());
}

std::invalid_argument InputError::toInvalidArgumentError() const
{
    return std::invalid_argument(toString());
}

} // namespace Antares::Error
