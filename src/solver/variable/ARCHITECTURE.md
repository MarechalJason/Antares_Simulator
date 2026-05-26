# `solver/variable` — architecture

Variables collect simulation outputs (timeseries, statistics) and produce the
survey reports written to disk. Each variable is a class implementing the
`IVariable` CRTP base.

This document focuses on the **Economy variable family** (area-level scalar
variables like `Balance`, `LOLP`, `LOLD`, `Price`, `SpilledEnergy`, …) which
share a Traits-based base class. Multi-column, dynamic, link, by-cluster and
adequacy variants follow the same pattern with their own base templates
(`MultiColumnBase`, `DynamicMultiColumnBase`, `StaticLinkBase`,
`STStorageByClusterBase`, `DispatchablePlantByClusterBase`, …).

## Files

- `economy_base.h` — `EconomyVariableCard<Traits>` + `EconomyVariableBase<Traits>`
  templates and the hook dispatchers (`Hooks_::*IfSupported`).
- `economy/<variable>.h` — one Traits struct per variable, plus a
  `using <Variable> = EconomyVariableBase<XxxTraits>;` alias.
- `economy/all.h`, `adequacy/all.h` — assemble the full `Container::List<…>`
  for each simulation mode.

## Traits contract

A valid Economy variable Traits struct must provide:

### Required

| Member | Kind | Purpose |
|---|---|---|
| `Caption()` | static method → `std::string` | survey column header |
| `Unit()` | static method → `std::string` | survey column unit |
| `Description()` | static method → `std::string` | long description |
| `ResultsProfile` | type alias | results template (e.g. `StandardResults<>`) |
| `decimal` | `static constexpr uint8_t` | display precision |
| `spatialAggregate` | `static constexpr uint8_t` | `Category::spatialAggregate*` flag |
| `computeStats(IntermediateValues&)` | static method | usually `iv.computeStatisticsForTheCurrentYear()` |

### Optional hooks (methods)

Dispatched via `if constexpr (requires { ... })` in `Hooks_`; absent → no-op.

| Hook | Signature | Phase |
|---|---|---|
| `initializeFromArea` | `(AuxiliaryDataType&, Data::Study*, Data::Area*)` | initialization |
| `yearBegin` | `(IntermediateValues&, AuxiliaryDataType&, uint year, uint numSpace)` | year start |
| `yearEndBuild` | `(IntermediateValues&, AuxiliaryDataType&, State&, uint year, uint numSpace)` | year end (per area) |
| `yearEndBuildForEachThermalCluster` | `(IntermediateValues&, State&, uint year, uint numSpace)` | year end (per thermal cluster) |
| `weekForEachArea` | `(IntermediateValues&, State&, uint numSpace)` | weekly |
| `setHourlyValue` | see below | hourly |
| `isPossiblyNonApplicable` | `static constexpr uint8_t` (0 or 1) | metadata |
| `spatialAggregatePostProcessing` | `static constexpr uint8_t` | metadata |

`setHourlyValue` accepts two shapes; the dispatcher picks the first that exists:

1. `(IntermediateValues&, AuxiliaryDataType&, State&, uint numSpace)` — when the
   variable needs auxiliary data.
2. `(IntermediateValues&, State&, uint numSpace)` — when it doesn't.

If neither is provided, a `static_assert` fires.

## Auxiliary data

A Traits struct may declare a nested `AuxiliaryDataType` (e.g. `Data::Area*`,
`double`, `FlowQuadAuxData`, …). `EconomyVariableBase` stores one such value
per instance (`auxiliaryData_`) and forwards it to every aux-aware hook.

When Traits doesn't declare one, `detail::AuxiliaryDataType<Traits>::type`
falls back to `detail::EmptyAuxiliaryData` (an empty placeholder struct). This
keeps the dispatcher signature uniform whether or not a variable carries
auxiliary data — the 3-arg `setHourlyValue` overload is then preferred and the
empty placeholder is never observed by user code.

## Lifecycle

The container drives each variable through the following sequence; bracketed
steps invoke an optional Traits hook iff it is defined.

```
initializeFromStudy                  → InitializeResultsFromStudy, allocate per-year buffers
initializeFromArea                   → [initializeFromArea]
initializeFromLink                   → no-op
simulationBegin                      → reset all per-year buffers
yearBegin                            → reset year buffer, [yearBegin]
hourBegin                            → no-op
hourForEachArea                      → [setHourlyValue]
weekForEachArea                      → [weekForEachArea]
yearEndBuild                         → [yearEndBuild]
yearEndBuildForEachThermalCluster    → [yearEndBuildForEachThermalCluster]
yearEnd                              → Traits::computeStats
computeSummary                       → merge year buffer into global results
simulationEnd                        → no-op
```

## Namespaces

- `Hooks_` — SFINAE dispatchers (`*IfSupported`) that call optional traits
  methods only when present.
- `detail` — implementation helpers (`AuxiliaryDataType` traits,
  `EmptyAuxiliaryData`, `statisticsCount`). Not public API.

## Future direction

The optional hooks group naturally by lifecycle phase:

- **InitializationPolicy** — `initializeFromArea`
- **YearlyLifecyclePolicy** — `yearBegin`, `yearEndBuild`
- **ThermalClusterPolicy** — `yearEndBuildForEachThermalCluster`
- **HourlyAggregationPolicy** — `weekForEachArea`
- **HourlyComputationPolicy** — `setHourlyValue`

A future refactor could replace the `if constexpr` cascades with tag dispatch
on policy classes.
