# Plan de migration anti-duplication des variables solver

## Objectif

Réduire la duplication dans `src/solver/variable/include/antares/solver/variable` en réutilisant d'abord les bases existantes, puis en introduisant des abstractions supplémentaires uniquement pour les cas restants.

Stratégie : phases courtes, chaque PR livrable seule, vérification fonctionnelle stricte (mêmes captions/unités/colonnes, même chaînage `NextType`).

## État actuel (vérifié)

| Phase | Portée | Statut |
|-------|--------|--------|
| 1 | STS by cluster → `STStorageByClusterBase` | ✅ Terminée |
| 2 | "by plant" + area simples → `DispatchablePlantByClusterBase` / `ByPlantBase` / `Economy_Base` | ✅ Terminée (hors `hydrostorage.h` — voir ci-dessous) |
| 3 | Liens mono-colonne → `EconomyLink_Base` / `StaticLinkBase` | ✅ Terminée |
| 4 | Consolidation bases (`EconomyLinkBase`+`CongestionFee_Base`→`EconomyLink_Base`, `ByPlant_Base`→`STStorageByClusterBase`) | ✅ Terminée |
| 6A | Triviaux `economy/` standalone → `Economy_Base` (incl. `max-mrg`) | ✅ Terminée |
| 6B | `yearEndBuild` / `weekForEachArea` / `AuxiliaryDataType` → `Economy_Base` (CSR, `localMatchingRule`, `overallCostCsr`, `max-mrg-csr`) | ✅ Terminée |
| 6C statique | `MultiColumnBase` + `thermalAirPollutantEmissions` | ✅ Terminée |
| 6C dynamique | `DynamicMultiColumnBase` + migrations `dispatchableGeneration` / `renewableGeneration` / `STSbyGroup` | 🔶 Base créée, migrations bloquées |
| 6D | `reservoirlevel.h` | ✅ Terminée (migration D1) |

### Bases disponibles (après Phase 4)

- `economy/economy_base.h` — area mono-colonne avec hooks optionnels `yearBegin`, `yearEndBuild`, `yearEndBuildForEachThermalCluster`, `hourForEachArea`, `weekForEachArea`, `AuxiliaryDataType`.
- `economy/links/links_base.h` (`EconomyLink_Base`) — lien mono-colonne avec hooks optionnels `hourValue`, `computeHourlyValue`, `buildDigest`.
- `economy/STStorageByCluster_base.h` — by-cluster générique (paramètres `clusterCount`, `fileLevel`, `buildSurveyReport`).
- `economy/DispatchablePlantByCluster_base.h` — by-cluster dispatchable (hooks `yearEndBuild*`, `hourForEachArea`, auxiliaire).
- `economy/links/static_link_base.h` — liens en `simulationEnd` (`flowQuad`, `loopFlow`).
- `economy/multi_column_base.h` — multi-colonnes **statique** (`ColCount` template).
- `economy/dynamic_multi_column_base.h` — multi-colonnes **dynamique** (`columnCount = Category::dynamicColumns`) — 🔶 voir Phase 6C dynamique ci-dessous.

### Dérive connue

- `hydrostorage.h` : la Phase 2 d'origine marquait ce fichier migré, mais il était resté standalone (245 lignes). Migré en Phase 6A (PR A1) vers `Economy_Base`.

## Tests de non-régression existants

- `test-intermediate`, `test-surveyresults`, `test-dynamic-aggregation`, `test-setofareas-reports`, `test-residual-load`.
- `test-migrated-variables-metadata` (`tests/src/solver/variable/test_migrated_variables_metadata.cpp`) — 5 cas Boost.Test, captures `Caption()` / `Unit()` / `columnCount` / `decimal` pour chaque famille migrée.

Ajouter une assertion par nouvelle variable migrée. Pour la Phase 6C dynamique : comparaison de sorties solver (digest + annual survey) avant/après sur une étude de référence multi-groupes.

### Commande de vérification standard

```bash
cd /home/jmarechal/Projects/src
/home/jmarechal/miniconda3/bin/cmake --build --target all --preset Debug-vcpkg
cd /home/jmarechal/CLionProjects/build_simulator
ctest --output-on-failure -R "test-intermediate|test-surveyresults|test-dynamic-aggregation|test-setofareas-reports|test-residual-load|test-migrated-variables-metadata"
```

---

## Travail restant

### PR 1 — Stabiliser `DynamicMultiColumnBase`

La base existe (`economy/dynamic_multi_column_base.h`, 289 lignes) mais ne valide pas l'API de survey utilisée par les 3 fichiers dynamiques restants. Problèmes identifiés à l'inspection :

1. **`localBuildAnnualSurveyReport`** : boucle sur `AncestorType::isPrinted[i]` par colonne. Les implémentations actuelles (`dispatchableGeneration.h` l.239-250, `STSbyGroup.h` l.315-327) font un early-return sur `isPrinted[0]` puis impriment toutes les colonnes sans re-tester `isPrinted[i]`. Uniformiser.
2. **`buildSurveyReport`** : appel `AncestorType::pResults[column].buildSurveyReport(&results, dataLevel, fileLevel, precision)` (signature simplifiée) — les implémentations réelles utilisent `pResults[column].template buildSurveyReport<ResultsType, VCardType>(results, pResults[column], dataLevel, fileLevel, precision)`. La signature templatée doit être restaurée.
3. **`dataLevel`/`fileLevel`/`precision` check manquant dans `buildSurveyReport`** : les impl. actuelles gardent `(dataLevel & categoryDataLevel) && (fileLevel & categoryFileLevel) && (precision & VCardType::precision)` autour du bloc — indispensable pour le routage correct.
4. **`hourForEachArea` signature** : `Traits::setHourlyValues(iv, state, numSpace, descriptors, groupToNumbers)` — les 3 cas n'ont pas tous besoin de `groupToNumbers_` (`STSbyGroup` l'utilise, `dispatchableGeneration` aussi, mais `thermalAirPollutantEmissions` l'ignorerait). Rendre ces paramètres optionnels via `if constexpr (requires { ... })`.

**Livrables PR 1** :
- Corriger les 4 points ci-dessus dans `dynamic_multi_column_base.h`.
- Ajouter un test `test-dynamic-multi-column-base` avec un `Traits` stub vérifiant : init area → `nbColumns` = descripteurs, yearEnd → computeStats, buildSurveyReport → captions/units dynamiques.

### PR 2 — `dispatchableGeneration.h` (297 → ~60 lignes)

**Traits** `DispatchableGenerationTraits` :
- `Caption() = "Dispatch. Gen."`, `Unit() = "MWh"`, `decimal = 0`.
- `ResultsType = Average<StdDev<Min<Max<>>>>`.
- `buildColumnDescriptors(area)` : set trié de `cluster->getGroup()` pour chaque cluster enabled → vector de `{groupName, "MWh"}`.
- `setHourlyValues(pValues, state, numSpace, descriptors, groupToNumbers)` : pour chaque cluster enabled, `pValues[groupToNumbers[cluster->getGroup()]][state.hourInTheYear] += state.thermal[area->index].thermalClustersProductions[cluster->enabledIndex]`.

**Alias** : `using DispatchableGeneration = DynamicMultiColumnBase<DispatchableGenerationTraits, NextT>;`.

**Critère d'acceptation** : digest et annual survey identiques avant/après sur étude de référence avec ≥ 2 groupes thermiques.

### PR 3 — `renewableGeneration.h` (299 → ~60 lignes)

Analogue PR 2 avec `area->renewable.list.each_enabled()` et `renewable[area->index].renewableClustersProductions`. Même pattern Traits.

### PR 4 — `STSbyGroup.h` (374 → ~80 lignes)

Le plus complexe des 3 : **3 sous-colonnes** `{INJECTION, WITHDRAWAL, LEVEL}` par groupe STS, **units hétérogènes** (`MW` pour inj/withd, `MWh` pour level), **computeStats hétérogène** (average pour level, statistics pour inj/withd).

**Prérequis DynamicMultiColumnBase** : confirmer que `ColumnDescriptor.{caption, unit}` permet bien des units distinctes par colonne (déjà prévu l.13-15 de `dynamic_multi_column_base.h`). Confirmer que `Traits::perColumnComputeStats(iv, col)` est bien appelé (l.186-204 — déjà présent).

**Traits** `STSbyGroupTraits` :
- `buildColumnDescriptors(area)` : pour chaque groupe unique dans `area->shortTermStorage.storagesByIndex`, pousse 3 descripteurs `{group + "_INJECTION", "MW"}`, `{group + "_WITHDRAWAL", "MW"}`, `{group + "_LEVEL", "MWh"}`.
- `setHourlyValues` : boucle sur `storagesByIndex`, remplit les 3 colonnes par groupe (cf. `STSbyGroup.h` l.238-268).
- `perColumnComputeStats(iv, col)` : `col % 3 == 2 ? computeAveragesForCurrentYearFromHourlyResults() : computeStatisticsForTheCurrentYear()`.

**Alias** : `using STSbyGroup = DynamicMultiColumnBase<STSbyGroupTraits, NextT>;`.

**Critère d'acceptation** : digest et annual survey identiques avant/après sur étude de référence avec ≥ 2 groupes STS de clusters différents.

## Ordre d'exécution

```
PR 1 (stabiliser DynamicMultiColumnBase)
   ├── PR 2 (dispatchableGeneration)    │ parallélisables
   ├── PR 3 (renewableGeneration)       │
   └── PR 4 (STSbyGroup)                ← dernier, car exerce perColumnComputeStats + units hétérogènes
```

## Gain estimé

- Acquis (Phases 1-6 hors C dynamique) : ~2 400 lignes de boilerplate supprimées.
- Potentiel restant (PR 2-4) : ~970 → ~200 lignes, soit **~770 lignes économisées**.

## Risques restants

1. **Régression digest/survey pour C dynamique** : les 3 fichiers ont `columnCount = Category::dynamicColumns`, impactant la structure du rapport et les captions générées runtime. **Capture d'une sortie de référence obligatoire avant PR 2**.
2. **API `buildSurveyReport` templatée** : PR 1 doit préserver la signature `template<ResultsType, VCardType>` sinon `DispatchableGenCountProdStatistics` ne compilera pas.
3. **`AncestorType::isPrinted` indexation dynamique** : `isPrinted` est un array de taille `columnCount`. Pour `dynamicColumns`, vérifier le dimensionnement runtime (cf. `VariableAccessor` pour `dynamicColumns`).

## Critères d'acceptation (tous PRs)

1. Mêmes captions / units / décimales / `columnCount` que l'implémentation précédente.
2. Chaînage `NextType::*` préservé dans toutes les étapes du cycle.
3. Suite complète de 6 tests passe sans modification.
4. Pour Phase 6C dynamique : digest et annual survey **byte-identiques** avant/après sur étude de référence.
5. Aucune API publique (`using DispatchableGeneration = ...`) cassée côté `economy/all.h` et `adequacy/all.h`.
