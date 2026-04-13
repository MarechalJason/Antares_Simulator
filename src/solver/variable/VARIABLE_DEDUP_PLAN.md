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
- [ ] `hydrostorage.h` **non migré** (drift plan/code détecté): `VCardHydroStorage` et la classe `HydroStorage` restent standalones (245 lignes, 0 référence à `Economy_Base`). Probablement à cause de `IntermediateValuesTypeForSpatialAg = std::unique_ptr<IntermediateValuesBaseType[]>` utilisé pour l'agrégation spatiale, non couvert par `VCard_Base`. À reprendre si le ROI le justifie.
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

- [x] `flowQuad.h` – migré vers `StaticLinkBase` via `FlowQuadTraits` (PR C5)
- [x] `loopFlow.h` – migré vers `StaticLinkBase` via `LoopFlowTraits` (PR C5)
- [x] `congestionProbability.h` – colonnes dynamiques, exclu de la migration. Nettoyage interne appliqué: `std::array`/`std::vector` en remplacement des pointeurs bruts (suppression du destructeur manuel), `beforeYearByYearExport` factorisé via un lambda `binarize` et des références locales `src`/`dst`.

Ces 3 fichiers nécessitent des bases dédiées ou des hooks supplémentaires dans les bases existantes.
flowQuad + loopFlow → nouvelle `StaticLinkBase` (PR C5 ci-dessous).

### Phase 4 - Consolidation des bases existantes

Les 7 bases ont ~95% de code identique (~1943 lignes). Objectif: 7 bases → 4+1 petite.

#### Partie A – Consolider 3 bases lien → 1 (`EconomyLink_Base`)

`EconomyLinkBase`, `CongestionFee_Base` ne diffèrent que par le dispatch de `hourForEachLink` et `buildDigest`.
Stratégie: clauses `requires` dans `EconomyLink_Base` (pattern déjà utilisé dans `Economy_Base`).

- [x] **PR A1** – Fusionner `EconomyLinkBase` dans `EconomyLink_Base` (~170 lignes économisées)
  - `links_base.h`: ajouter dispatch `requires` pour `Traits::hourValue(state)` dans `hourForEachLink`
  - `links_base.h`: ajouter hook optionnel `Traits::buildDigest` dans `buildDigest`
  - `flowLinear.h`, `flowLinearAbs.h`: ajouter `decimal`, `computeStats`; changer base → `EconomyLink_Base`
  - Supprimer `EconomyLinkBase.h`

- [x] **PR A2** – Fusionner `CongestionFee_Base` dans `EconomyLink_Base` (~180 lignes économisées)
  - `links_base.h`: ajouter dispatch `requires` pour `Traits::computeHourlyValue(state, up, down)`
  - Déplacer `ResultsType` dans `CongestionFeeTraits`/`CongestionFeeAbsTraits`
  - `congestionFee.h`, `congestionFeeAbs.h`: changer base → `EconomyLink_Base`
  - Supprimer `congestionFee_base.h`

**Résultat: 4 bases mono-valeur → 2** (`Economy_Base` area, `EconomyLink_Base` link)

#### Partie B – Consolider `STStorageByCluster` + `ByPlant` → 1 (`ByClusterBase`)

- [x] **PR B1** – Généraliser `STStorageByClusterBase` avec `Traits::clusterCount`, `Traits::fileLevel` et `Traits::buildSurveyReport`

- [x] **PR B2** – Migrer `productionByRenewablePlant`, supprimer `ByPlant_base.h` (~170 lignes)

**Résultat: 3 bases multi-cluster → 2** (`ByClusterBase`, `DispatchablePlantByClusterBase`)

#### Partie C – Migrer les fichiers standalone restants

- [x] **PR C1** – `STStorageCashFlowByCluster` → `STStorageByClusterBase` (~200 lignes)
- [x] **PR C2** – `waterValue` → `Economy_Base` (~180 lignes) + étendre `VCard_Base` pour `isPossiblyNonApplicable`
- [x] **PR C3** – `nbOfDispatchedUnits` → `Economy_Base` (~190 lignes) + hook `yearEndBuildForEachThermalCluster`
- [x] **PR C4** – `overallCost` (economy + adequacy) → `Economy_Base` (~400 lignes)
- [x] **PR C5** – Créer `StaticLinkBase`, migrer `flowQuad` + `loopFlow` (~350 lignes)

**Total estimé: ~1870 lignes économisées sur 9 PRs.**

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
   - Fait: `tests/src/solver/variable/test_migrated_variables_metadata.cpp` couvre 4 familles (area `Economy_Base`, STS by cluster, dispatchable by cluster, liens `EconomyLink_Base`/`StaticLinkBase`) — 5 cas Boost.Test, exécution < 50 ms.
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
- [x] Vérifier que les symboles publics (`STstorage*ByCluster`) restent inchangés (utilisés dans `economy/all.h` et `adequacy/all.h`, aliases préservés).
- [x] Vérifier captions/units/descriptions inchangées (déportées dans les `Traits` dédiés).
- [x] Construire via le preset: `cmake --build --target antares-solver --preset Debug-vcpkg` — OK (16 objets, link `solver/antares-solver` réussi).
- [x] Exécuter tests ciblés variable/reports — 4/4 OK (`test-intermediate`, `test-surveyresults`, `test-dynamic-aggregation`, `test-setofareas-reports`).

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

Build (depuis la racine du repo):

```bash
/home/jmarechal/miniconda3/bin/cmake --build --target all --preset Debug-vcpkg
```

Tests ciblés (depuis le répertoire de build du preset):

```bash
ctest --output-on-failure -R "test-intermediate|test-surveyresults|test-dynamic-aggregation|test-setofareas-reports"
```

