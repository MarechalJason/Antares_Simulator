// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_ECONOMY_LoopFlow_H__
#define __SOLVER_VARIABLE_ECONOMY_LoopFlow_H__

#include <cstring>

#include "static_link_base.h"

namespace Antares::Solver::Variable::Economy
{

struct LoopFlowTraits
{
    static std::string Caption()
    {
        return "LOOP FLOW";
    }

    static std::string Unit()
    {
        return "MWh";
    }

    static std::string Description()
    {
        return "Loop flow";
    }

    typedef Results<R::AllYears::Raw< // Raw values
      >>
      ResultsType;

    // Data is loaded once from the link parameters when the link is initialized.
    // No per-year processing needed: simulationEnd will compute stats + merge.
    template<class Aux>
    static void onInitializeFromAreaLink(IntermediateValues& iv,
                                         Aux&,
                                         Data::Study*,
                                         Data::AreaLink* link)
    {
        if (link->useLoopFlow)
        {
            unsigned int height = link->parameters.height;
            (void)::memcpy(iv.hour,
                           link->parameters[Data::fhlLoopFlow],
                           sizeof(double) * height);
        }
    }
};

using VCardLoopFlow = VCardStaticLinkBase<LoopFlowTraits>;

/*!
** \brief Loop flow
*/
template<class NextT = Container::EndOfList>
using LoopFlow = StaticLinkBase<LoopFlowTraits, NextT>;

} // namespace Antares::Solver::Variable::Economy

#endif // __SOLVER_VARIABLE_ECONOMY_LoopFlow_H__
