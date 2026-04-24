// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/solver/variable/tuple_variable_list.h"
#include "antares/solver/variable/variable.h" // for Container::EndOfList

namespace Antares::Solver::Variable::Common
{

// Variadic composition of output variables.
//
// Each Vi is a class template `template<class NextT = Container::EndOfList> ...`.
// Instantiating `Vi<>` yields a standalone leaf terminated by EndOfList (no chain).
// We flatten these into a tuple-based dispatcher that aggregates events by folding,
// replacing the prior recursive `Head<Tail<...<Last<EndOfList>>>>` CRTP chain.
template<template<class> class... Vs>
struct ComposeAll
{
    using type = Container::TupleVariableList<Vs<Container::EndOfList>...>;
};

} // namespace Antares::Solver::Variable::Common
