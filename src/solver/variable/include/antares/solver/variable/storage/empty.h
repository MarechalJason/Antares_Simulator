// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_STORAGE_EMPTY_H__
#define __SOLVER_VARIABLE_STORAGE_EMPTY_H__

namespace Antares::Solver::Variable
{
// Sentinel used as the legacy chain terminator for the decorator typelist.
// Decorators no longer inherit from one another; this type only marks the
// end of the chain so the typelist extractor in Results can stop.
struct Empty
{
    using NextType = void;
};

} // namespace Antares::Solver::Variable

#endif // __SOLVER_VARIABLE_STORAGE_EMPTY_H__
