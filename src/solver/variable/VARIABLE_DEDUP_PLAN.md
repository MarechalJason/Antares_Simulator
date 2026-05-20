# Plan de migration anti-duplication des variables solver

## Objectif

Réduire la duplication dans `src/solver/variable/include/antares/solver/variable` en réutilisant les bases `Traits`-based déjà en place, et migrer le boilerplate résiduel identifié.

## Phases terminées

| Phase | Portée | Statut |
|-------|--------|--------|
| 1 | STS by cluster → `STStorageByClusterBase` | ✅ |
| 2 | "by plant" + area simples → `DispatchablePlantByClusterBase` / `ByPlantBase` / `Economy_Base` | ✅ |
| 3 | Liens mono-colonne → `EconomyLink_Base` / `StaticLinkBase` | ✅ |
| 4 | Consolidation bases (fusion `EconomyLinkBase`+`CongestionFee_Base`, `ByPlant_Base`→`STStorageByClusterBase`) | ✅ |
| 6A | `economy/` triviaux → `Economy_Base` | ✅ |
| 6B | Hooks étendus (`yearEndBuild`, `weekForEachArea`, `AuxiliaryDataType`) → `Economy_Base` | ✅ |
| 6C statique | `MultiColumnBase` + `thermalAirPollutantEmissions` | ✅ |
| 6C dynamique | `DynamicMultiColumnBase` + `dispatchableGeneration` / `renewableGeneration` / `STSbyGroup` | ✅ |
| 6D | `reservoirlevel.h` | ✅ |

## Bases disponibles

- `economy/economy_base.h` — area mono-colonne (hooks optionnels `yearBegin`, `yearEndBuild`, `yearEndBuildForEachThermalCluster`, `hourForEachArea`, `weekForEachArea`, `AuxiliaryDataType`).
- `economy/links/links_base.h` (`EconomyLink_Base`) — lien mono-colonne.
- `economy/STStorageByCluster_base.h` — by-cluster générique.
- `economy/DispatchablePlantByCluster_base.h` — by-cluster dispatchable.
- `economy/links/static_link_base.h` — liens en `simulationEnd` (flowQuad, loopFlow).
- `economy/multi_column_base.h` — multi-colonnes **statique**.
- `economy/dynamic_multi_column_base.h` — multi-colonnes **dynamique**.
- `commons/timeseries_base.h` — variables timeseries (hydro, thermal, renewable timeseries).

## État actuel (vérifié)

Recherche `class X : public Variable::IVariable<...>` (`grep -l`) dans `include/antares/solver/variable/` → **7 fichiers** contiennent encore une classe `IVariable` standalone :

| Fichier | Lignes | Nature | Migrable ? |
|---------|--------|--------|-----------|
| `commons/psp.h` | 239 | Area mono-colonne, `hourForEachArea` direct | ✅ Oui → `Economy_Base` |
| `commons/rowBalance.h` | 237 | Area mono-colonne, `hourForEachArea` direct | ✅ Oui → `Economy_Base` |
| `adequacy/spilledEnergy.h` | 247 | Area mono-colonne namespace Adequacy | ✅ Oui → `Economy_Base` |
| `commons/join.h` | 353 | **Infrastructure** : `Join<LeftT,RightT>` double héritage, `columnCount=0`, `hasIntermediateValues=0` | ❌ Non — pas une variable de données |
| `economy/links/congestionProbability.h` | 337 | Multi-colonnes **dynamique** lien, `binarize` year-by-year | ⚠️ Possible via nouvelle `DynamicLinkMultiColumnBase` — ROI faible |
| `economy/multi_column_base.h` | 241 | **Base** — garder | — |
| `economy/dynamic_multi_column_base.h` | 322 | **Base** — garder | — |

## Travail restant — 3 PRs

### PR 1 — `commons/psp.h` (239 → ~65 lignes)

Pattern `residual.h`. `PSPTraits` :
- `Caption() = "PSP"`, `Unit() = "MWh"`, `decimal = 0`, `ResultsType = Average<>`.
- `computeStats = computeStatisticsForTheCurrentYear`.
- `setHourlyValue(iv, state, numSpace)` : reprendre le corps actuel de `hourForEachArea` (cf. `psp.h` autour de la ligne 199-202).

**Alias** : `using PSP = Economy_Base<PSPTraits, NextT>;` + `using VCardPSP = VCard_Base<PSPTraits>;` (préserver le nom public).

**Test** : ajouter dans `test_migrated_variables_metadata.cpp` :
```cpp
BOOST_CHECK_EQUAL(VCardPSP::Caption(), "PSP");
BOOST_CHECK_EQUAL(VCardPSP::Unit(), "MWh");
```

### PR 2 — `commons/rowBalance.h` (237 → ~65 lignes)

Identique à PR 1. `RowBalanceTraits` :
- `Caption() = "ROW BAL."`, `Unit() = "MWh"`, `decimal = 0`, `ResultsType = Raw<>`.
- `computeStats = computeStatisticsForTheCurrentYear`.
- `setHourlyValue(iv, state, numSpace)` : copier le corps actuel.

**Vérifier** que `VCard_Base` expose bien `ResultsType = Raw<>` compatible (cf. `congestionProbability.h` et `nearPriceCap.h` pour le précédent).

### PR 3 — `adequacy/spilledEnergy.h` (247 → ~70 lignes)

Même pattern, namespace `Adequacy` (cf. `adequacy/overallCost.h` qui utilise déjà `Economy_Base` depuis le namespace `Variable::Adequacy`).

`SpilledEnergyTraits` :
- `Caption() = "SPIL. ENRG"`, `Unit() = "MWh"`, `decimal = 0`.
- `computeStats = computeStatisticsForTheCurrentYear`.
- `setHourlyValue` : corps actuel `hourForEachArea`.

**Attention** : ne pas confondre avec `economy/spilledEnergy.h` (déjà migré en PR Phase 2). Le caption `"SPIL. ENRG"` doit rester identique pour la rétro-compatibilité de sortie.

## Non-objectifs explicites

- **`commons/join.h`** — infrastructure de composition (`Join<LeftT, RightT>` double héritage), pas une variable de données. Ne pas migrer.
- **`economy/links/congestionProbability.h`** — multi-colonnes dynamique avec logique `beforeYearByYearExport` (binarize). Nécessiterait une `DynamicLinkMultiColumnBase` distincte de `DynamicMultiColumnBase` (area vs link). Gain ~240 lignes pour ~250 lignes de nouvelle base → ROI nul. Garder standalone.

## Ordre d'exécution

PR 1, PR 2, PR 3 sont **totalement indépendantes** — parallélisables. Aucune extension de base requise.

## Gain estimé

- **Déjà économisé** : ~3 400 lignes (Phases 1-6).
- **PRs 1-3** : ~720 → ~200 lignes = **~520 lignes supplémentaires**.
- **Total cumul prévisible** : ~3 920 lignes de boilerplate supprimées.

## Tests de non-régression

```bash
cd /home/jmarechal/Projects/src
/home/jmarechal/miniconda3/bin/cmake --build --target all --preset Debug-vcpkg
cd /home/jmarechal/CLionProjects/build_simulator
ctest --output-on-failure -R "test-intermediate|test-surveyresults|test-dynamic-aggregation|test-setofareas-reports|test-residual-load|test-migrated-variables-metadata"
```

**Dernier run vérifié** : 5 tests passent, 0 échec.

## Critères d'acceptation (PRs 1-3)

1. Mêmes `Caption()` / `Unit()` / `decimal` / `columnCount` que l'implémentation précédente (vérifié via `test_migrated_variables_metadata.cpp`).
2. Chaînage `NextType::*` préservé sur `initializeFrom*`, `simulationBegin/End`, `yearBegin/End`, `hourBegin`, `hourForEachArea`, `computeSummary`.
3. Symboles publics `PSP`, `RowBalance`, `Adequacy::SpilledEnergy` préservés (usages dans `commons/*`, `adequacy/all.h`, `economy/all.h` inchangés).
4. Suite de 6 tests passe sans modification.
