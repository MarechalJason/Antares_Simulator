// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "economy_base.h"
#include "lold_base.h"

namespace Antares::Solver::Variable::Economy
{

struct LOLDTraits: public LOLD_Base_Traits
{
    static std::string Caption()
    {
        return "LOLD";
    }

    static std::string Description()
    {
        return "LOLD";
    }

    using AuxiliaryDataType = detail::EmptyAuxiliaryData;

    static void setHourlyValue(IntermediateValues& iv,
                               AuxiliaryDataType&,
                               const State& state,
                               unsigned int)
    {
        if (state.hourlyResults->ValeursHorairesDeDefaillancePositive[state.hourInTheWeek] > 0.5)
        {
            iv[state.hourInTheYear] = value(state);
        }
    }
};

using VCardLOLD = VCard_Base<LOLDTraits>;

template<class = Container::EndOfList>
using LOLD = Economy_Base<LOLDTraits>;

} // namespace Antares::Solver::Variable::Economy
