// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_END_OF_LIST_END_OF_LIST_H__
#define __SOLVER_END_OF_LIST_END_OF_LIST_H__

// Terminator for the CRTP variable chain. Provides no-op implementations
// for all hooks. Post-A.2, most hooks are not called on EndOfList anymore
// (variables no longer propagate via NextType::), but storage decorators
// and the legacy CRTP root still expect these methods to exist.

#include <antares/study/study.h>
#include "state.h"
#include "surveyresults.h"

namespace Antares::Solver::Variable::Container
{
class EndOfList
{
public:
    using NextType = void;

    static constexpr size_t count = 0;

    template<int, int>
    struct Statistics
    {
        static constexpr int count = 0;
    };

    template<class PredicateT>
    static void RetrieveVariableList(PredicateT&)
    {
    }

    EndOfList() = default;
    ~EndOfList() = default;

    // --- Lifecycle ---
    static void initializeFromStudy([[maybe_unused]] Data::Study& study)
    {
    }
    static void initializeFromArea(Data::Study*, Data::Area*)
    {
    }
    static void initializeFromAreaLink(Data::Study*, Data::AreaLink*)
    {
    }
    static void initializeFromThermalCluster(Data::Study*, Data::Area*, Data::ThermalCluster*)
    {
    }

    void broadcastNonApplicability(bool)
    {
    }
    void getPrintStatusFromStudy([[maybe_unused]] Data::Study& study)
    {
    }
    void supplyMaxNumberOfColumns([[maybe_unused]] Data::Study& study)
    {
    }

    // --- Simulation ---
    static void simulationBegin()
    {
    }
    static void simulationEnd()
    {
    }
    static void yearBegin(unsigned int, unsigned int)
    {
    }
    static void yearEndBuildPrepareDataForEachThermalCluster(State&, uint, uint)
    {
    }
    static void yearEndBuildForEachThermalCluster(State&, uint, uint)
    {
    }
    static void yearEndBuild(State&, unsigned int, unsigned int)
    {
    }
    static void yearEnd(unsigned int, unsigned int)
    {
    }
    static void computeSummary(unsigned int, unsigned int)
    {
    }

    template<class V>
    void yearEndSpatialAggregates(V&, unsigned int, uint)
    {
    }
    template<class V, class SetT>
    void yearEndSpatialAggregates(V&, unsigned int, const SetT&, uint)
    {
    }

    template<class V>
    static void computeSpatialAggregatesSummary(V&, unsigned int, unsigned int)
    {
    }
    template<class V>
    static void simulationEndSpatialAggregates(V&)
    {
    }
    template<class V, class SetT>
    void simulationEndSpatialAggregates(V&, const SetT&)
    {
    }

    static void weekBegin(State&)
    {
    }
    static void weekForEachArea(State&, uint)
    {
    }
    static void weekEnd(State&)
    {
    }

    static void hourBegin(unsigned int)
    {
    }
    static void hourForEachArea(State&, uint)
    {
    }
    static void hourForEachLink(State&, uint)
    {
    }
    static void hourEnd(State&, unsigned int)
    {
    }

    // --- Reporting ---
    static void buildSurveyReport(SurveyResults&, int, int, int)
    {
    }
    static void buildAnnualSurveyReport(SurveyResults&, int, int, int, uint)
    {
    }
    static void buildDigest(SurveyResults&, int, int)
    {
    }
    static void beforeYearByYearExport(uint, uint)
    {
    }

    template<class I>
    static void provideInformations(I&)
    {
    }

    // --- Spatial aggregates ---
    template<class SearchVCardT, class O>
    static void computeSpatialAggregateWith(O&)
    {
        assert(false);
    }
    template<class SearchVCardT, class O>
    static void computeSpatialAggregateWith(O&, const Data::Area*)
    {
        assert(false);
    }
    template<class SearchVCardT, class O>
    static void computeSpatialAggregateWith(O&, const Data::Area*, uint)
    {
    }

    // --- Result retrieval ---
    template<class VCardToFindT>
    const double* retrieveHourlyResultsForCurrentYear(uint) const
    {
        return nullptr;
    }
    template<class VCardToFindT, class O>
    static void retrieveResultsForArea(O**, const Data::Area*)
    {
    }
    template<class VCardToFindT, class O>
    static void retrieveResultsForThermalCluster(O**, const Data::ThermalCluster*)
    {
    }
    template<class VCardToFindT, class O>
    static void retrieveResultsForLink(O**, const Data::AreaLink*)
    {
    }

    void localBuildAnnualSurveyReport(SurveyResults&, int, int, unsigned int) const
    {
    }

}; // class EndOfList

} // namespace Antares::Solver::Variable::Container

#endif // __SOLVER_END_OF_LIST_END_OF_LIST_H__