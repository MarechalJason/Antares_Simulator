# Plan de migration anti-duplication des variables solver

## Checklist de démarrage

- [ ] Cartographier les variables avec boilerplate `VCard` / `Statistics` / cycle de vie.
- [ ] Prioriser les migrations "faible risque" vers les bases déjà présentes.
- [ ] Migrer par lots petits et indépendants avec critères d'acceptation explicites.
- [ ] Sécuriser avec tests ciblés + non-régression de sorties.
- [ ] Traiter ensuite les cas complexes via hooks dédiés.

## Objectif

Réduire la duplication dans `src/solver/variable/include/antares/solver/variable` en réutilisant d'abord les bases existantes (pas de refonte big-bang), puis en introduisant des abstractions supplémentaires uniquement pour les cas restants.

Stratégie: phases courtes, chacune livrable seule, avec vérification fonctionnelle stricte (mêmes sorties, mêmes captions/unités, même chaînage `NextType`) pour limiter les régressions.

## Bases déjà disponibles

- `economy/economy_base.h`
- `economy/links/EconomyLinkBase.h`
- `economy/ByPlant_base.h`
- `economy/DispatchablePlantByCluster_base.h`
- `economy/STStorageByCluster_base.h`
- `commons/timeseries_base.h`

## Steps

1. Établir un inventaire priorisé des duplications dans `economy/` et `economy/links/`, centré sur `Statistics`, `initializeFrom*`, `simulationBegin/End`, `year*`.
2. Migrer le lot STS vers `STStorageByClusterBase` en transformant:
   - `STStorageInjectionByCluster.h`
   - `STStorageWithdrawalByCluster.h`
   - `STStorageLevelsByCluster.h`
   en variantes `Traits`.
3. Migrer le lot dispatchable "simple" vers `DispatchablePlantByClusterBase`/`ByPlantBase`, en ciblant d'abord `minDispatchableGenByPlant.h`, puis `NbOfDispatchedUnitsByPlant` selon compatibilité des hooks `yearEndBuild*`.
4. Migrer le lot liens "mono-colonne" vers `EconomyLinkBase.h` ou `links_base.h`, en conservant les comportements spécifiques `buildDigest`.
5. Traiter les cas complexes (`ProductionByDispatchablePlant`, `CongestionProbability`, éventuellement `ProfitByPlant`) via extension de base avec hooks explicites.
6. Finaliser avec une passe transverse de rationalisation `VCard`/`Statistics` (mixin partagé) si et seulement si les phases précédentes restent stables.

## Phases (priorisées)

### Phase 1 - Faible risque, fort gain

Variables STS quasi isomorphes vers `STStorageByClusterBase`.

### Phase 2 - Faible à moyen risque

Variables "by plant" sans logique annuelle spéciale vers `ByPlantBase` / `DispatchablePlantByClusterBase`.

Avancement:

- [x] `minDispatchableGenByPlant.h` migré vers `DispatchablePlantByCluster_base.h`
- [x] `DispatchablePlantByCluster_base.h` étendu avec un hook optionnel `hourForEachArea`
- [x] `nbOfDispatchedUnitsByPlant.h` migré vers `DispatchablePlantByCluster_base.h`
- [x] `DispatchablePlantByCluster_base.h` étendu avec hooks optionnels `yearEndBuildPrepareDataForEachThermalCluster` et `yearEndBuildForEachThermalCluster`
- [x] `npCostByDispatchablePlant.h` migré vers `DispatchablePlantByCluster_base.h`
- [x] `productionByRenewablePlant.h` migré vers `ByPlant_base.h`
- [x] `profitByPlant.h` migré vers `DispatchablePlantByCluster_base.h`
- [x] `productionByDispatchablePlant.h` migré vers `DispatchablePlantByCluster_base.h`
- [x] `DispatchablePlantByCluster_base.h` étendu avec un stockage auxiliaire optionnel (traits) pour les cas avec état annuel additionnel
- [x] `residual.h` migré vers `economy_base.h`
- [x] `economy_base.h` étendu avec un stockage auxiliaire optionnel (traits) pour `initializeFromArea` / `yearBegin` / `hourForEachArea`
- [x] `avail-dispatchable-generation.h` migré vers `economy_base.h`
- [x] `dispatchable-generation-margin.h` migré vers `economy_base.h`
- [x] `nearPriceCap.h` migré vers `economy_base.h`
- [x] `hydrostorage.h` migré vers `economy_base.h`
- [x] `spilledEnergy.h` migré vers `economy_base.h`

### Phase 3 - Moyen risque

Variables de lien mono-colonne vers `EconomyLinkBase` / `links_base`.

Avancement:

- [x] `congestionFee.h` migré vers `congestionFee_base.h` via `CongestionFeeTraits`
- [x] `congestionFeeAbs.h` migré vers `congestionFee_base.h` via `CongestionFeeAbsTraits`
- [x] `flowLinear.h` migré vers `EconomyLinkBase.h` via `FlowLinearTraits` (buildDigest personnalisé)
- [x] `flowLinearAbs.h` migré vers `EconomyLinkBase.h` via `FlowLinearAbsTraits`
- [x] `marginalCost.h` migré vers `links_base.h` via `MarginalCostTraits` (decimal=2, computeAverages)
- [x] `hurdleCosts.h` migré vers `links_base.h` via `HurdleCostsTraits` (logique conditionnelle hourForEachLink)

### Phase 3b - Futur (cycles de vie spécialisés)

Variables de lien avec cycles alternatifs (pas `pNbYearsParallel`, calcul en `simulationEnd`/`initializeFromAreaLink`):

- [ ] `flowQuad.h` – `initializeFromAreaLink`, `simulationEnd` (data pointers, merge(0, ...))
- [ ] `loopFlow.h` – `initializeFromAreaLink`, `simulationEnd`, template hook `InitializeResultsFromStudy`
- [ ] `congestionProbability.h` – colonnes dynamiques, exclu

Ces 3 fichiers nécessitent des bases dédiées ou des hooks supplémentaires dans les bases existantes.

### Phase 4 - Haut risque

Variables avec mémoire/agrégation spécifique (`ProductionByDispatchablePlant`, etc.) avec hooks dédiés.

### Phase 5 - Optionnelle

Uniformisation globale `VCard`/`Statistics` si le ROI est confirmé.

## Critères d'acceptation

1. **Comportement identique**: mêmes valeurs de rapports annuels/digest pour les variables migrées (captions, unités, colonnes, précision).
2. **Chaînage intact**: appels `NextType` inchangés dans toutes les étapes du cycle (`initialize`, `simulation`, `year`, `hour`).
3. **Aucune régression visible**: pas de changement fonctionnel sur les suites existantes de `src/tests/src/solver/variable`.
4. **Réduction mesurable**: suppression nette de boilerplate par lot (baisse claire des blocs dupliqués `VCard` + `Statistics` + lifecycle).
5. **Migration réversible par lot**: chaque PR reste autonome, limitée à un groupe de variables homogène.

## Risques et mitigations

1. **Divergences statistiques subtiles** (`computeStatisticsForTheCurrentYear` vs `computeAveragesForCurrentYearFromHourlyResults`)
   - Mitigation: encapsuler dans `Traits::computeStats` obligatoire.
2. **Colonnes dynamiques/multi-colonnes** (`Category::dynamicColumns`, cas `CongestionProbability`)
   - Mitigation: exclure des premières phases, ajouter des hooks dédiés.
3. **Régressions de reporting** (`localBuildAnnualSurveyReport`, `buildDigest`)
   - Mitigation: comparer des sorties de référence par variable migrée.
4. **Effets de bord mémoire** (pointeurs bruts historiques)
   - Mitigation: migrer d'abord les classes sans gestion mémoire custom.
5. **Scope creep**
   - Mitigation: plafond de fichiers par phase et validation systématique en fin de lot.

## Stratégie de tests

1. Étendre les tests unitaires dans `src/tests/src/solver/variable` pour valider le cycle de vie standardisé (`initializeFromStudy`, `yearBegin`, `yearEnd`, `computeSummary`) via un faux `Traits`.
2. Ajouter des tests de non-régression de métadonnées (`Caption`, `Unit`, nombre de colonnes) et de `Statistics<count>` pour les classes migrées.
3. Ajouter des tests ciblés `buildAnnualSurveyReport` / `buildDigest` sur les variables migrées de `economy/links`.
4. Exécuter les tests existants: `test-intermediate`, `test-surveyresults`, `test-dynamic-aggregation`, `test-setofareas-reports` après chaque phase.
5. Pour les phases 3-4, comparer les sorties solver avant/après sur un petit jeu d'étude de référence, centré sur les variables touchées.

## Décisions à confirmer

1. Périmètre du premier sprint:
   - Option A: STS uniquement
   - Option B: STS + liens simples
   - Option C: inclure dispatchable simple dès le premier sprint
2. Règle de découpage PR: 1 famille homogène par PR.
3. La phase mixin `VCard`/`Statistics` est-elle obligatoire, ou conditionnée au niveau de duplication restant ?

## Sprint 1 valide (Option A)

Périmètre retenu: **STS by cluster uniquement**.

Fichiers inclus:

- `src/solver/variable/include/antares/solver/variable/economy/STStorageInjectionByCluster.h`
- `src/solver/variable/include/antares/solver/variable/economy/STStorageWithdrawalByCluster.h`
- `src/solver/variable/include/antares/solver/variable/economy/STStorageLevelsByCluster.h`

Fichiers explicitement exclus de la PR #1:

- `src/solver/variable/include/antares/solver/variable/economy/STSbyGroup.h`
- `src/solver/variable/include/antares/solver/variable/economy/STStorageCashFlowByCluster.h`

### PR #1 - Checklist exécutable

- [x] Migrer `STStorageInjectionByCluster` vers `STStorageByClusterBase` via un `Traits` dédié.
- [x] Migrer `STStorageWithdrawalByCluster` vers `STStorageByClusterBase` via un `Traits` dédié.
- [x] Migrer `STStorageLevelsByCluster` vers `STStorageByClusterBase` avec hook de stats "average".
- [ ] Vérifier que les symboles publics (`STstorage*ByCluster`) restent inchangés.
- [ ] Vérifier captions/units/descriptions inchangées.
- [ ] Construire `antares-solver`.
- [ ] Exécuter tests ciblés variable/reports.

### Ordre de migration

1. `STStorageInjectionByCluster.h`
2. `STStorageWithdrawalByCluster.h`
3. `STStorageLevelsByCluster.h`

### Critères de done PR #1

1. Plus de cycle de vie dupliqué dans les 3 headers STS migrés.
2. Aucune rupture d'API template publique dans les listes Economy/Adequacy.
3. Build solver OK.
4. Tests ciblés OK.

### Validation minimale

Depuis `src/`:

```bash
cmake --build ../build-debug --target antares-solver -j$(nproc)
```

Depuis `build-debug/`:

```bash
ctest --output-on-failure -R "test-intermediate|test-surveyresults|test-dynamic-aggregation|test-setofareas-reports"
```

