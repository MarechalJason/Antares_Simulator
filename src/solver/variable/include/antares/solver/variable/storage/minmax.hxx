// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_STORAGE_MIN_HXX__
#define __SOLVER_VARIABLE_STORAGE_MIN_HXX__

namespace Antares::Solver::Variable::R::AllYears
{
template<bool OpInferior, class NextT>
inline void MinMaxBase<OpInferior, NextT>::initializeFromStudy(Data::Study& /*study*/)
{
}

template<bool OpInferior, class NextT>
inline void MinMaxBase<OpInferior, NextT>::reset()
{
    if (OpInferior)
    {
        minmax.resetInf();
    }
    else
    {
        minmax.resetSup();
    }
}

template<bool OpInferior, class NextT>
inline void MinMaxBase<OpInferior, NextT>::merge(uint year, const IntermediateValues& rhs)
{
    if (OpInferior)
    {
        minmax.mergeInf(year, rhs);
    }
    else
    {
        minmax.mergeSup(year, rhs);
    }
}

template<bool OpInferior, class NextT>
template<uint Size, class VCardT>
void MinMaxBase<OpInferior, NextT>::InternalExportIndices(SurveyResults& report,
                                                          const MinMaxData::Data* array,
                                                          int fileLevel)
{
    assert(array);
    assert(report.data.columnIndex < report.maxVariables && "Column index out of bounds");

    report.captions[0][report.data.columnIndex] = report.variableCaption;
    report.captions[1][report.data.columnIndex] = report.variableUnit;
    report.captions[2][report.data.columnIndex] = (OpInferior ? "min" : "max");

    int recommendedPrecision = (int)Category::MaxDecimalPrecision(fileLevel);
    uint decimalPrec = (recommendedPrecision < (int)VCardT::decimal) ? (uint)recommendedPrecision
                                                                     : (uint)VCardT::decimal;

    report.nonApplicableStatus[report.data.columnIndex] = *report.isCurrentVarNA;

    Solver::Variable::AssignPrecisionToPrintfFormat(report.precision[report.data.columnIndex],
                                                    decimalPrec);

    double* v = report.values[report.data.columnIndex];
    for (uint i = 0; i != Size; ++i)
    {
        v[i] = array[i].index;
    }

    ++report.data.columnIndex;
}

template<bool OpInferior, class NextT>
template<uint Size, class VCardT>
inline void MinMaxBase<OpInferior, NextT>::InternalExportValues(SurveyResults& report,
                                                                const MinMaxData::Data* array)
{
    assert(array);
    assert(report.data.columnIndex < report.maxVariables && "Column index out of bounds");

    report.captions[0][report.data.columnIndex] = report.variableCaption;
    report.captions[1][report.data.columnIndex] = report.variableUnit;
    report.captions[2][report.data.columnIndex] = (OpInferior ? "min" : "max");
    Solver::Variable::AssignPrecisionToPrintfFormat(report.precision[report.data.columnIndex],
                                                    VCardT::decimal);

    report.nonApplicableStatus[report.data.columnIndex] = *report.isCurrentVarNA;

    double* v = report.values[report.data.columnIndex];
    for (uint i = 0; i != Size; ++i)
    {
        v[i] = array[i].value;
    }

    ++report.data.columnIndex;
}

} // namespace Antares::Solver::Variable::R::AllYears

#endif // __SOLVER_VARIABLE_STORAGE_MIN_HXX__
