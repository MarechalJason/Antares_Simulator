// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_STORAGE_RESULTS_H__
#define __SOLVER_VARIABLE_STORAGE_RESULTS_H__

#include <tuple>
#include <type_traits>
#include <utility>

#include <antares/study/study.h>

#include "../categories.h"
#include "fwd.h"
#include "intermediate.h"

namespace Antares::Solver::Variable
{
// Variadic typelist wrapper. Preferred form for new call sites:
//   using ResultsProfile = Results<Chain<Average, StdDeviation, Min, Max>>;
// (The legacy nested syntax Results<Average<StdDeviation<...>>> still works
// via ChainToTuple below.)
template<class... Decorators>
struct Chain
{
    using Tuple = std::tuple<Decorators...>;
};

namespace detail
{
// Extract a std::tuple of decorators from a legacy chain head
// (Average<StdDev<Min<Max<>>>> ...) by walking the NextType link to Empty.
template<class T>
struct ChainToTuple;

template<>
struct ChainToTuple<Empty>
{
    using type = std::tuple<>;
};

template<class T>
struct ChainToTuple
{
private:
    template<class U, class Tail>
    struct Prepend;
    template<class U, class... Us>
    struct Prepend<U, std::tuple<Us...>>
    {
        using type = std::tuple<U, Us...>;
    };

public:
    using type = typename Prepend<T, typename ChainToTuple<typename T::NextType>::type>::type;
};

// Accept either a Chain<...> wrapper or a legacy chain head.
template<class T>
struct ResolveTuple
{
    using type = typename ChainToTuple<T>::type;
};

template<class... Ds>
struct ResolveTuple<Chain<Ds...>>
{
    using type = std::tuple<Ds...>;
};

// IsInstanceOf for template-templates with signature <class, int> (Average, Raw, StdDeviation).
template<template<class, int> class Tmpl, class T>
struct IsInstanceOf2 : std::false_type
{
};

template<template<class, int> class Tmpl, class A, int F>
struct IsInstanceOf2<Tmpl, Tmpl<A, F>> : std::true_type
{
};

// Find first index of a decorator in a tuple that is an instance of Tmpl.
// Returns -1 if not found.
template<template<class, int> class Tmpl, class Tuple>
struct FindFirstInstance;

template<template<class, int> class Tmpl>
struct FindFirstInstance<Tmpl, std::tuple<>>
{
    static constexpr int value = -1;
};

template<template<class, int> class Tmpl, class Head, class... Tail>
struct FindFirstInstance<Tmpl, std::tuple<Head, Tail...>>
{
private:
    static constexpr int tail_index = FindFirstInstance<Tmpl, std::tuple<Tail...>>::value;

public:
    static constexpr int value = IsInstanceOf2<Tmpl, Head>::value
                                   ? 0
                                   : (tail_index < 0 ? -1 : tail_index + 1);
};

} // namespace detail

template<class FirstDecoratorT = Empty,
         template<class, int> class DecoratorForSpatialAggregateT = R::AllYears::Raw>
class Results;

using StandardAllYearsDecorators
  = R::AllYears::Average<R::AllYears::StdDeviation<R::AllYears::Min<R::AllYears::Max<>>>>;

template<template<class, int> class DecoratorForSpatialAggregateT = R::AllYears::Raw>
using StandardResults = Results<StandardAllYearsDecorators, DecoratorForSpatialAggregateT>;

/*!
** \brief Aggregated results, composed of a tuple of decorators.
**
** The first template parameter is either a Chain<D1, D2, ...> typelist or the
** head of the legacy nested chain (Average<StdDev<Min<Max<>>>>); both forms
** resolve to std::tuple<D1, D2, ...> and are dispatched via std::apply.
*/
template<class FirstDecoratorT, template<class, int> class DecoratorForSpatialAggregateT>
class Results
{
public:
    using DecoratorTuple = typename detail::ResolveTuple<FirstDecoratorT>::type;

    static constexpr std::size_t count = std::tuple_size_v<DecoratorTuple>;

private:
    template<class Tup>
    struct CategoryFileFold;

    template<class... Ds>
    struct CategoryFileFold<std::tuple<Ds...>>
    {
        static constexpr int value = (0 | ... | Ds::categoryFile);
    };

public:
    static constexpr int categoryFile = CategoryFileFold<DecoratorTuple>::value;

    void initializeFromStudy(Antares::Data::Study& study)
    {
        std::apply([&](auto&... d) { (d.initializeFromStudy(study), ...); }, decorators_);
    }

    void reset()
    {
        std::apply([](auto&... d) { (d.reset(), ...); }, decorators_);
    }

    void merge(uint year, const IntermediateValues& data)
    {
        std::apply([&](auto&... d) { (d.merge(year, data), ...); }, decorators_);
    }

    template<class S, class VCardT>
    void buildSurveyReport(SurveyResults& report,
                           const S& results,
                           int dataLevel,
                           int fileLevel,
                           int precision) const
    {
        std::apply(
          [&](const auto&... d)
          {
              (d.template buildSurveyReport<S, VCardT>(report,
                                                       results,
                                                       dataLevel,
                                                       fileLevel,
                                                       precision),
               ...);
          },
          decorators_);
    }

    template<class VCardT>
    void buildDigest(SurveyResults& report, int digestLevel, int dataLevel) const
    {
        std::apply(
          [&](const auto&... d)
          { (d.template buildDigest<VCardT>(report, digestLevel, dataLevel), ...); },
          decorators_);
    }

    Antares::Memory::Stored<double>::ConstReturnType hourlyValuesForSpatialAggregate() const
    {
        constexpr int idx = detail::
          FindFirstInstance<DecoratorForSpatialAggregateT, DecoratorTuple>::value;
        if constexpr (idx >= 0)
        {
            const auto& d = std::get<idx>(decorators_);
            if constexpr (requires { d.hourlyForSpatialAggregate(); })
            {
                return d.hourlyForSpatialAggregate();
            }
            else
            {
                return Antares::Memory::Stored<double>::NullValue();
            }
        }
        else
        {
            return Antares::Memory::Stored<double>::NullValue();
        }
    }

    // Convenience accessors for external code that previously relied on the
    // inheritance-exposed sibling member.
    auto& avgdata()
    {
        constexpr int idx = detail::FindFirstInstance<R::AllYears::Average, DecoratorTuple>::value;
        static_assert(idx >= 0, "avgdata() requires an R::AllYears::Average decorator");
        return std::get<idx>(decorators_).avgdata;
    }

    const auto& avgdata() const
    {
        constexpr int idx = detail::FindFirstInstance<R::AllYears::Average, DecoratorTuple>::value;
        static_assert(idx >= 0, "avgdata() requires an R::AllYears::Average decorator");
        return std::get<idx>(decorators_).avgdata;
    }

    auto& rawdata()
    {
        constexpr int idx = detail::FindFirstInstance<R::AllYears::Raw, DecoratorTuple>::value;
        static_assert(idx >= 0, "rawdata() requires an R::AllYears::Raw decorator");
        return std::get<idx>(decorators_).rawdata;
    }

    const auto& rawdata() const
    {
        constexpr int idx = detail::FindFirstInstance<R::AllYears::Raw, DecoratorTuple>::value;
        static_assert(idx >= 0, "rawdata() requires an R::AllYears::Raw decorator");
        return std::get<idx>(decorators_).rawdata;
    }

private:
    DecoratorTuple decorators_;
}; // class Results

} // namespace Antares::Solver::Variable

#include "average.h"
#include "empty.h"
#include "minmax.h"
#include "raw.h"
#include "stdDeviation.h"

#endif // __SOLVER_VARIABLE_STORAGE_RESULTS_H__
