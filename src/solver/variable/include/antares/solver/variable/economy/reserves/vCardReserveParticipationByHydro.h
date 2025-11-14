
#pragma once

#include "../../storage/results.h"

namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace Economy
{

struct VCardReserveParticipationByHydro
{
    static std::string Caption()
    {
        return "HYDRO RESERVE PARTICIPATION";
    }

    static std::string Unit()
    {
        return "Reserve Participation Power - MWh";
    }

    static std::string Description()
    {
        return "Reserve Participation from hydro to a reserve";
    }

    typedef Results<R::AllYears::Average<>> ResultsType;
    typedef VCardReserveParticipationByHydro VCardForSpatialAggregate;

    enum
    {
        categoryDataLevel = Category::DataLevel::area,
        categoryFileLevel = ResultsType::categoryFile & (Category::FileLevel::de),
        precision = Category::all,
        nodeDepthForGUI = +0,
        decimal = 2,
        columnCount = Category::dynamicColumns,
        spatialAggregate = Category::spatialAggregateSum,
        spatialAggregateMode = Category::spatialAggregateEachYear,
        spatialAggregatePostProcessing = 0,
        hasIntermediateValues = 1,
        isPossiblyNonApplicable = 0,
    };

    typedef IntermediateValues IntermediateValuesDeepType;
    typedef std::vector<IntermediateValues> IntermediateValuesBaseType;
    typedef std::vector<IntermediateValuesBaseType> IntermediateValuesType;
};

} // namespace Economy
} // namespace Variable
} // namespace Solver
} // namespace Antares
