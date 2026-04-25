// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_STORAGE_MINMAX_H__
#define __SOLVER_VARIABLE_STORAGE_MINMAX_H__

#include "minmax-data.h"

namespace Antares::Solver::Variable::R::AllYears
{
class Min;
class Max;

struct MinMaxBase
{
public:
    static constexpr int categoryFile = Variable::Category::FileLevel::allFile;

    static const char* Name()
    {
        return "minmaxbase";
    }

    MinMaxBase() = default;
    ~MinMaxBase() = default;

    void initializeFromStudy(Data::Study& study);

    template<class S, class VCardT>
    void buildSurveyReport(SurveyResults& report,
                           const S& results,
                           int dataLevel,
                           int fileLevel,
                           int precision) const
    {
        if (fileLevel & Category::FileLevel::id)
        {
            switch (precision)
            {
            case Category::hourly:
                InternalExportIndices<HOURS_PER_YEAR, VCardT>(report,
                                                              Memory::RawPointer(
                                                                minmax.hourly.data()),
                                                              fileLevel);
                break;
            case Category::daily:
                InternalExportIndices<DAYS_PER_YEAR, VCardT>(report,
                                                             minmax.daily.data(),
                                                             fileLevel);
                break;
            case Category::weekly:
                InternalExportIndices<WEEKS_PER_YEAR, VCardT>(report,
                                                              minmax.weekly.data(),
                                                              fileLevel);
                break;
            case Category::monthly:
                InternalExportIndices<MONTHS_PER_YEAR, VCardT>(report,
                                                               minmax.monthly.data(),
                                                               fileLevel);
                break;
            case Category::annual:
                InternalExportIndices<1, VCardT>(report, minmax.annual.data(), fileLevel);
                break;
            }
        }
        else
        {
            switch (precision)
            {
            case Category::hourly:
                InternalExportValues<HOURS_PER_YEAR, VCardT>(report,
                                                             Memory::RawPointer(
                                                               minmax.hourly.data()));
                break;
            case Category::daily:
                InternalExportValues<DAYS_PER_YEAR, VCardT>(report, minmax.daily.data());
                break;
            case Category::weekly:
                InternalExportValues<WEEKS_PER_YEAR, VCardT>(report, minmax.weekly.data());
                break;
            case Category::monthly:
                InternalExportValues<MONTHS_PER_YEAR, VCardT>(report, minmax.monthly.data());
                break;
            case Category::annual:
                InternalExportValues<1, VCardT>(report, minmax.annual.data());
                break;
            }
        }
    }

    template<class VCardT>
    void buildDigest(SurveyResults& /*report*/, int /*digestLevel*/, int /*dataLevel*/) const
    {
        // No digest output for min/max.
    }

    void reset();

    void merge(uint year, const IntermediateValues& rhs);

protected:
    MinMaxData minmax;

private:
    template<uint Size, class VCardT>
    static void InternalExportIndices(SurveyResults& report,
                                      const MinMaxData::Data* array,
                                      int fileLevel);

    template<uint Size, class VCardT>
    static void InternalExportValues(SurveyResults& report, const MinMaxData::Data* array);

}; // class MinMaxBase

template<bool OpInferior>
class MinMaxBaseT : public MinMaxBase
{
};

class Min: public MinMaxBaseT<true>
{
public:
    static const char* Name()
    {
        return "min";
    }
};

class Max: public MinMaxBaseT<false>
{
public:
    static const char* Name()
    {
        return "max";
    }
};

} // namespace Antares::Solver::Variable::R::AllYears

#include "minmax.hxx"

#endif // __SOLVER_VARIABLE_STORAGE_MINMAX_H__
