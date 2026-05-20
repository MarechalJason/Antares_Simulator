# Plan – Design review de `src/solver/variable/`

## Contexte

La campagne anti-duplication documentée dans `VARIABLE_DEDUP_PLAN.md` a supprimé ~3 400 lignes de boilerplate et migré ~22 variables vers 8 bases `Traits`-based (`Economy_Base`, `EconomyLink_Base`, `STStorageByClusterBase`, `DispatchablePlantByClusterBase`, `StaticLinkBase`, `MultiColumnBase`, `DynamicMultiColumnBase`, `TimeSeriesValuesBase`).

Maintenant que l'essentiel du boilerplate est éliminé, la duplication visible a reculé, mais **des incohérences d'API et de structure sont apparues** (naming drift, hooks redondants, organisation de dossiers obsolète). L'objectif de ce plan n'est **pas** plus de dédup ; c'est de cadrer une revue de design pour rendre la surface API cohérente et pérenne avant que d'autres contributeurs s'appuient dessus.

Livrable visé : une liste priorisée de changements architecturaux, chacun indépendamment réalisable, avec des critères de done objectifs et des risques documentés.

## Étendue — ce qui est dans le scope

Revue ciblée sur 9 axes (S1–S9 ci-dessous). Chaque axe est indépendant : le plan peut être exécuté en partie, dans l'ordre recommandé (§Séquencement).

## Étendue — non-objectifs explicites

- **Pas de dédup résiduelle** (`commons/psp.h`, `commons/rowBalance.h`, `adequacy/spilledEnergy.h`) — suit `VARIABLE_DEDUP_PLAN.md`, pas cette revue.
- **Pas de migration de `commons/join.h` ni `economy/links/congestionProbability.h`** — exclus explicitement du plan dédup (infrastructure / ROI négatif).
- **Pas de remplacement du CRTP `IVariable` root** — ancrage dans `area.hxx`, `setofareas.hxx`, `bindConstraints.hxx`, `info.h`, hors scope.
- **Pas de changement des `Caption()` / `Unit()` strings** — contrat de sortie, vérifié byte-à-byte par `test-surveyresults`.
- **Pas de modification de la forme de `Results<R::AllYears::...>` à la source** — le stockage dans `storage/results.hxx` dépend de la récursion exacte. S3 ajoute uniquement des alias, sans toucher au template.
- **Réorg de dossiers (S7)** — décision ouverte, flaggée mais pas imposée (churn git élevé).

## Concerns identifiés (avec évidence code)

### S1 — Dérive de nommage des hooks `Traits::setHourlyValue*` **(HAUTE)**

#### Inventaire complet

**8 points d'entrée dans les bases**, dispatchés par cascades `if constexpr (requires { ... })` :

| # | Entry-point côté base | Fichier:ligne | Signature |
|---|------------------------|---------------|-----------|
| 1 | `Traits::setHourlyValue(iv, aux, state, numSpace)` | `economy_base.h:351-355` | `(IntermediateValues&, Aux&, State&, uint)` |
| 2 | `Traits::checkCondition(aux, state)` + `Traits::value(aux, state)` | `economy_base.h:357-363` | `(Aux&, const State&) → bool` / `(Aux&, const State&) → double` |
| 3 | `Traits::checkCondition(state)` + `Traits::value(state)` | `economy_base.h:364-370` | `(const State&) → bool` / `(const State&) → double` |
| 4 | `Traits::setHourlyValue(clusterValues, state)` | `STStorageByCluster_base.h:195` | `(std::vector<IntermediateValues>&, State&)` |
| 5 | `Traits::setHourlyValuesForCurrentYear(clusterValues, aux, state, numSpace)` | `DispatchablePlantByCluster_base.h:320-327` | `(std::vector<IntermediateValues>&, Aux&, State&, uint)` |
| 6 | `Traits::setHourlyValuesForCurrentYear(clusterValues, state)` | `DispatchablePlantByCluster_base.h:329-333` | `(std::vector<IntermediateValues>&, State&)` |
| 7 | `Traits::setHourlyValues(iv_array, state, numSpace)` | `multi_column_base.h:205`, `dynamic_multi_column_base.h:241,246` | `(IntermediateValues(&)[N] \| std::vector<std::vector<IV>>&, State&, uint)` |
| 8 | `Traits::setHourlyValuesWithDescriptors(iv_array, state, numSpace, descriptors, g2n)` | `dynamic_multi_column_base.h:227,234` | `(..., const std::vector<ColumnDescriptor>&, const std::map<std::string, size_t>&)` |

**Lien** (`links_base.h:192-216`) — cascade à 3 niveaux orthogonale au reste :

| # | Entry-point | Signature |
|---|------------|-----------|
| L1 | `Traits::hourForEachLink(iv, state)` | `(IntermediateValues&, State&)` |
| L2 | `Traits::hourValue(state) → double` | `(State&) → double`, somme sur `iv.hour[h]` |
| L3 | `Traits::computeHourlyValue(state, upstreamPrice, downstreamPrice) → double` | avec pré-fetch des prix depuis `problemeHebdo` |

#### Implémentations concrètes recensées

- `setHourlyValue(IV&, Aux&, State&, uint)` — 7 implémentations (`balance`, `hydroCost`, `hydrostorage`, `overallCost`, `overallCostCsr`, `reservoirlevel`, `dispatchable-generation-margin`, `waterValue`).
- `setHourlyValue(vector<IV>&, State&)` — 5 implémentations STS by cluster (`STStorage{Injection,Withdrawal,Levels,CashFlow}ByCluster`, `productionByRenewablePlant`).
- `setHourlyValuesForCurrentYear(vector<IV>&, [Aux&,] State&, [uint])` — 4 implémentations (`profitByPlant`, `productionByDispatchablePlant`, `minDispatchableGenByPlant`, `nbOfDispatchedUnitsByPlant`).
- `setHourlyValues(IV_array&, State&, uint)` — `thermalAirPollutantEmissions`.
- `setHourlyValuesWithDescriptors(...)` — `STSbyGroup`, `dispatchableGeneration`, `renewableGeneration`.
- `checkCondition/value` — 15+ implémentations (le pattern « condition + valeur scalaire »).
- `hourForEachLink` — 2 (`marginalCost`, `hurdleCosts`).
- `hourValue` — 2 (`flowLinear`, `flowLinearAbs`).
- `computeHourlyValue` — 2 (`congestionFee`, `congestionFeeAbs`).

#### Problèmes concrets

1. **Nom surchargé entre scopes** : `setHourlyValue` signifie à la fois « écrire 1 valeur sur 1 IV » (area, entry 1) ET « écrire N valeurs sur un vecteur IV » (by-cluster, entry 4). Un lecteur ne peut pas inférer le scope depuis le nom.
2. **Redondance cascade dans `economy_base.h`** : les paires `setHourlyValue` vs `checkCondition+value` existent en parallèle parce qu'on n'a jamais migré les Traits existants. `checkCondition+value` est toujours équivalent à un `setHourlyValue` qui teste et assigne.
3. **Suffixe `ForCurrentYear`** (`DispatchablePlantByCluster_base.h`) sans raison structurelle — tous les autres hooks sont « for current year » implicitement via `pValuesForTheCurrentYear`. Drift de nommage historique.
4. **Suffixe `WithDescriptors`** (`dynamic_multi_column_base.h`) — le fallback `setHourlyValues` sans descriptors n'est utilisé par **aucune** variable dynamique réelle (les 3 migrées passent toutes par `WithDescriptors`). Chemin mort.
5. **Dispatch par nom** (`if constexpr (requires { Traits::X... })`) au lieu de par signature rend impossible la surcharge propre et multiplie les branches pour chaque variante de paramètres.

#### API cible unifiée

Un seul nom par concept, dispatch par **signature** (overload) :

```cpp
// Area / scalar
static void setHourlyValue(IntermediateValues& iv,
                           State& state,
                           unsigned int numSpace);
// + overload avec Aux si Traits::AuxiliaryDataType est défini :
static void setHourlyValue(IntermediateValues& iv,
                           AuxiliaryDataType& aux,
                           State& state,
                           unsigned int numSpace);

// By cluster (vector of IVs indexé par cluster)
static void setHourlyValue(std::vector<IntermediateValues>& clusterValues,
                           State& state,
                           unsigned int numSpace);
static void setHourlyValue(std::vector<IntermediateValues>& clusterValues,
                           AuxiliaryDataType& aux,
                           State& state,
                           unsigned int numSpace);

// Multi-column statique
static void setHourlyValue(IntermediateValues (&columns)[ColCount],
                           State& state,
                           unsigned int numSpace);

// Multi-column dynamique : descriptors passés en paramètre (pas de WithDescriptors)
static void setHourlyValue(std::vector<std::vector<IntermediateValues>>& columns,
                           State& state,
                           unsigned int numSpace,
                           const std::vector<ColumnDescriptor>& descriptors);

// Lien
static double hourValue(State& state);
// ou
static double hourValue(State& state, double upstreamPrice, double downstreamPrice);
```

**Résultats** :
- 8 entry-points → **1 nom** (`setHourlyValue`) en area/cluster/multi-column.
- Les 2 variantes lien (`hourValue` simple vs avec prix) restent sous **un seul nom** (`hourValue`), surchargé par signature.
- Le couple `checkCondition + value` est **déprécié** : les Traits concernés migrent vers un `setHourlyValue` unique de 4 lignes (`if (cond) iv[state.hourInTheYear] = v;`).
- Le suffixe `WithDescriptors` disparaît — les descriptors sont un paramètre ordinaire.
- Le suffixe `ForCurrentYear` disparaît — convention implicite.

#### Mapping old → new

| Ancien hook | Nouveau hook | Nb implémentations à migrer |
|-------------|--------------|-----------------------------|
| `setHourlyValue(IV&, Aux&, State&, uint)` | `setHourlyValue(IV&, Aux&, State&, uint)` | 8 (inchangé) |
| `setHourlyValue(vector<IV>&, State&)` | `setHourlyValue(vector<IV>&, State&, uint)` | 5 (ajout `numSpace`) |
| `setHourlyValuesForCurrentYear(...)` | `setHourlyValue(vector<IV>&, [Aux&,] State&, uint)` | 4 |
| `setHourlyValues(IV_array&, State&, uint)` | `setHourlyValue(IV_array&, State&, uint)` | 1 |
| `setHourlyValuesWithDescriptors(..., descriptors, g2n)` | `setHourlyValue(vector<vector<IV>>&, State&, uint, descriptors)` | 3 (simplification signature) |
| `checkCondition(state) + value(state)` | `setHourlyValue(IV&, Aux&, State&, uint)` avec `if (cond) iv[...] = v;` | ~15 (mécanique) |
| `checkCondition(Aux&, state) + value(Aux&, state)` | idem avec Aux | 5 |
| `hourForEachLink(iv, state)` | `setHourlyValue(IV&, State&, uint)` côté lien **OU** conserver (cas où on accumule `+=`) | 2 — à décider |
| `hourValue(state) → double` | conserver (utile pour sommation hour par hour) | 2 |
| `computeHourlyValue(state, up, down) → double` | `hourValue(state, up, down) → double` | 2 |

#### Question ouverte : `checkCondition + value`

Le pattern `checkCondition` est aussi utilisé par `lolpCsr`, `loldCsr`, `lolp`, `lold` (via `lolp_base.h` / `lold_base.h`). **Avant** de le déprécier globalement, vérifier que ces bases dédiées (qui portent le sens « compter les violations ») ne veulent pas garder cette sémantique expressive. Possiblement : garder `checkCondition+value` **uniquement** pour `lolp_base`/`lold_base`, et migrer les cas area génériques (`price.h`, `priceCSR.h`, `dtgMarginAfterCsr.h`, `spilledEnergy.h`, `unsupliedEnergy.h`, `domesticUnsuppliedEnergy.h`, `avail-dispatchable-generation.h`, `dispatchable-generation-margin.h`, `inflow.h`, `overflow.h`, `pumping.h`, `nearPriceCap.h`, `residual.h`, `localMatchingRuleViolations.h`) vers `setHourlyValue` explicite.

#### Plan de refactor (PR séquentielles)

1. **Ajouter les nouveaux overloads `setHourlyValue`** dans `economy_base.h`, `STStorageByCluster_base.h`, `DispatchablePlantByCluster_base.h`, `multi_column_base.h`, `dynamic_multi_column_base.h`. Les anciens hooks restent reconnus (coexistence temporaire via `if constexpr` chaîné).
2. **Migrer les Traits area/cluster** pour renommer `setHourlyValuesForCurrentYear` → `setHourlyValue`. Mécanique, un fichier à la fois. Tests : `test-migrated-variables-metadata`, `test-surveyresults`, `test-intermediate` sur chaque lot.
3. **Migrer les Traits multi-column dynamique** : remplacer `setHourlyValuesWithDescriptors` par `setHourlyValue(..., descriptors)`. Supprimer le paramètre `groupToNumbers_` (les 3 Traits peuvent reconstruire la map depuis `descriptors` — ou on la porte dans `AuxiliaryDataType`).
4. **Renommer `computeHourlyValue` → `hourValue(state, up, down)`** (lien). 2 fichiers.
5. **Migrer `checkCondition + value` (area uniquement, pas LOLP/LOLD)** vers `setHourlyValue` explicite. ~15 fichiers mécaniques, chacun passe de 10 lignes à 5.
6. **Supprimer les vieux entry-points** dans les 5 bases une fois les Traits migrés. Nettoyage des cascades `if constexpr`.

#### Done si

- Un seul nom `setHourlyValue` (aire/cluster/multi-column) + un seul nom `hourValue` (lien) dans tout `src/solver/variable/include`.
- Aucune cascade `if constexpr (requires { ... })` à plus de 2 branches par base (la 2e pour la variante avec Aux).
- `checkCondition + value` restent **uniquement** dans `lolp_base.h` / `lold_base.h` si décision retenue (sinon 0 sites).
- Suffixes `ForCurrentYear` / `WithDescriptors` totalement éliminés.
- Suite de tests complète passe, et un snapshot digest + annual survey sur étude de référence reste byte-identique avant/après.

#### Effort estimé

- Refactor des bases + ajout des overloads : ~½ j.
- Migration des Traits (35-40 fichiers mécaniquement similaires) : 1,5 j.
- Validation digest/survey byte-à-byte : ½ j.
- **Total : ~2,5 j**, avec S8 en filet de sécurité.

#### Risques

1. **Ambiguïté de surcharge** : certains Traits définissent à la fois `setHourlyValue(IV&, State&, uint)` et `setHourlyValue(IV&, Aux&, State&, uint)` — si `Aux = EmptyAuxiliaryData` on peut avoir des résolutions ambiguës. Mitigation : utiliser SFINAE ou `requires` sur le membre `AuxiliaryDataType` pour n'exposer qu'un overload à la fois.
2. **ADL cross-namespace** : `Variable::Economy::setHourlyValue` surchargé dans `Variable::Adequacy::setHourlyValue` — les Traits locaux doivent rester qualifiés `Traits::setHourlyValue`. Mitigation : garder toute résolution à travers le Traits concret, pas d'ADL.
3. **Signature `std::vector<std::vector<IV>>&`** sur multi-column dynamique plus verbeuse que `pValuesForTheCurrentYear[numSpace]` du côté base. Mitigation : introduire un alias `using DynamicColumnsIV = ...;` dans `dynamic_multi_column_base.h`.

### S2 — `economy_base.h` explose en hooks optionnels **(HAUTE)**

Neuf membres `Traits` optionnels dispatchés par `if constexpr (requires { ... })` :
`yearEndBuild` (`economy_base.h:213`), `yearEndBuildForEachThermalCluster` (L233), `weekForEachArea` (L287), `initializeFromArea` (L329), `yearBegin` (L340), `setHourlyValue` / `checkCondition` + `value` (cascade L346-371), plus `AuxiliaryDataType` (L29-39) et `isPossiblyNonApplicable` (L88-98).

**À investiguer** : (a) recenser quels hooks co-existent en pratique par Traits concret ; (b) évaluer un regroupement en « policies » (`ThermalClusterPolicy`, `WeeklyPolicy`, `AuxiliaryPolicy`) via tag dispatch plutôt que la liste plate.

**Done si** : chaque base documente un contrat fermé de ≤ 5 hooks. Effort : 2-3 j de design.

### S3 — `ResultsType` non factorisé **(MOYEN)**

Le template `Results<R::AllYears::Average<R::AllYears::StdDeviation<R::AllYears::Min<R::AllYears::Max<>>>>>` (et ses variantes plus courtes) est dupliqué verbatim dans **44 sites répartis sur 39 fichiers** (`grep R::AllYears::StdDeviation`).

**À faire** : ajouter dans `storage/results.h` :
```cpp
using StandardResults = Results<R::AllYears::Average<R::AllYears::StdDeviation<R::AllYears::Min<R::AllYears::Max<>>>>>;
using AverageOnly     = Results<R::AllYears::Average<>>;
using RawOnly         = Results<R::AllYears::Raw<>>;
```
Remplacer les sites. **Ne pas** changer la structure sous-jacente — c'est un alias de lisibilité.

**Done si** : plus d'un seul site avec `R::AllYears::StdDeviation` littéralement (les alias eux-mêmes). Effort : ½ j.

### S4 — API de reporting à 3 sorties partiellement redondantes **(MOYEN)**

`localBuildAnnualSurveyReport` est quasi verbatim dans `economy_base.h:306-322`, `links_base.h:238-254`, et ~6 autres bases (même pattern `isCurrentVarNA` / `isPrinted[0]` / `variableCaption` / `variableUnit` / `buildAnnualSurveyReport`). `buildDigest` n'est surchargé que dans `links_base.h:219` et `commons/join.h:253` — ailleurs c'est le passthrough default de `variable.hxx:313`. `buildSurveyReport` vit dans `info.h`.

**À faire** : factoriser `localBuildAnnualSurveyReport` dans une fonction helper générique sur `IVariable` ou un mixin CRTP. Vérifier si `buildDigest` justifie encore un override par base.

**Done si** : une seule implémentation du bloc `if (isPrinted[0]) { caption/unit; buildAnnualSurveyReport; }`. Couvert par `test-surveyresults` existant. Effort : 1 j.

### S5 — `economy_base.h` et `links_base.h` partagent ~80 % de structure **(MOYEN)**

Comparer `economy_base.h:149-266` (area) et `links_base.h:102-177` (link) : `initializeFromStudy`, `simulationBegin/End`, `yearBegin`, `yearEnd`, `computeSummary`, `hourBegin`, `retrieveRawHourlyValuesForCurrentYear`, `localBuildAnnualSurveyReport` sont **identiques**. Seules différences : `categoryDataLevel` (area vs link), le hook de routage (`hourForEachArea` vs `hourForEachLink`), et `initializeFromArea` / `initializeFromAreaLink`.

**À faire** : introduire `MonoColumnBase<Traits, Topology, NextT>` avec `Topology ∈ {AreaTopology, LinkTopology}`. Les tags portent `categoryDataLevel`, le nom du hook de routage, et le dispatch aux initializers. `VCard_Base` et `VCard_LinkBase` convergent vers un seul template.

**Done si** : une seule base mono-colonne (~250 lignes) remplace les deux actuelles (381 + 264). Effort : 2-3 j.

### S6 — Chaîne variadique `NextType` qui fuit **(MOYEN)**

Chaque base réécrit la récursion `Statistics<CDataLevel, CFile>` (`economy_base.h:133-144` et `links_base.h:88-99` — identiques), l'enum `count = 1 + NextT::count`, et doit appeler `NextType::xxx(...)` à la fin de **chaque hook** (~50 sites). Ajouter un nouveau hook exige d'éditer `endoflist.h`, `container.hxx`, `area.hxx`, `setofareas.hxx`, `bindConstraints.hxx`, `info.h`, toutes les bases, et `variable.hxx`.

**À faire** (scope réduit, pragmatique) : hoisting de `Statistics` et `count` dans un helper CRTP sur `IVariable` ; ne **pas** remplacer le variadic par tuple maintenant (coût > bénéfice court terme).

**Done si** : ajouter un futur hook `dayForEachArea` touche ≤ 2 fichiers. Effort : 1 j pour le hoist seul, 4-5 j pour une refonte complète (non recommandé à ce stade).

### S7 — Organisation de dossiers obsolète **(FAIBLE, décision ouverte)**

`commons/psp.h`, `commons/rowBalance.h`, `commons/load.h`, `commons/generation.h` sont toutes des area mono-colonne « commons » partagées. `economy/balance.h`, `economy/price.h` le sont aussi. Le split `commons/` vs `economy/` ne tracke plus aucune invariant structurel. `adequacy/` ne contient plus que 3 fichiers.

**Réorg proposée** (à décider) : `bases/`, `area/`, `link/`, `cluster/`, `timeseries/`, `infrastructure/`.

**Done si** : un nouveau contributeur localise une variable à partir de sa catégorie sans grep. **Risque** : churn git élevé, `git blame` cassé. Effort : ½ j mécanique + revue des `#include`. **Flagger pour décision d'équipe** avant exécution.

### S8 — Couverture de tests design-level insuffisante **(MOYEN)**

`test_migrated_variables_metadata.cpp` vérifie uniquement `Caption()`/`Unit()`/`columnCount`/`decimal` statiques. Aucun test ne valide :
- que `if constexpr (requires { ... })` dispatche la bonne branche pour un Traits donné ;
- qu'un Traits sans hook optionnel compile et se comporte en no-op ;
- que `NextType::hourForEachArea(...)` est bien appelé pour chaque variable d'une chaîne.

**À faire** :
- harnais `static_assert` par base (un Traits complet, un Traits minimal) ;
- test d'instrumentation : chaîne `NextT` de 3 éléments, Traits espion qui compte les appels par hook ;
- tests snapshot `buildAnnualSurveyReport` sur un représentant par base.

**Done si** : un futur refactor sur S1/S2/S4 est rattrapé mécaniquement par CI. Effort : 2 j. **À faire en premier** (filet de sécurité pour tout le reste).

### S9 — Documentation / savoir tribal **(FAIBLE)**

- `economy_base.h:107-108` commente « Base class for economy variables like LOLP and LOLD » — obsolète (LOLP/LOLD utilisent `lolp_base.h` / `lold_base.h`).
- Pas de doxygen sur le contrat `Traits`.
- Cascade de 7 dispatchs dans `economy_base.h:346-371` sans commentaire d'ordre de priorité.
- Détection de `AuxiliaryDataType` (`economy_base.h:25-39`) non documentée.

**À faire** : entête par base listant membres `Traits` requis + optionnels avec signatures ; un `docs/TRAITS.md` « comment écrire une nouvelle variable ».

**Done si** : un nouveau contributeur écrit une variable sans lire le code existant. Effort : 1 j.

## Ordre de lecture recommandé pour le reviewer

1. `src/solver/variable/VARIABLE_DEDUP_PLAN.md` (contexte historique).
2. `src/solver/variable/include/antares/solver/variable/variable.h` + `variable.hxx` (racine CRTP).
3. `src/solver/variable/include/antares/solver/variable/endoflist.h` (terminator, contrat des hooks).
4. `src/solver/variable/include/antares/solver/variable/economy/economy_base.h` (base la plus riche, S1 / S2 / S4).
5. `src/solver/variable/include/antares/solver/variable/economy/links/links_base.h` (comparaison pour S5).
6. `src/solver/variable/include/antares/solver/variable/economy/multi_column_base.h` + `dynamic_multi_column_base.h` (S1 divergence pluriel).
7. `src/solver/variable/include/antares/solver/variable/economy/DispatchablePlantByCluster_base.h` + `STStorageByCluster_base.h` (S1 divergence `ForCurrentYear`).
8. `src/solver/variable/include/antares/solver/variable/commons/timeseries_base.h` (check convergence S5).
9. Exemples concrets : `economy/balance.h`, `economy/dispatchableGeneration.h`, `economy/links/flowQuad.h`.
10. `src/solver/variable/include/antares/solver/variable/container.h` + `info.h` (côté List du chain, S6).

## Séquencement recommandé des PRs

1. **S8** — filet de sécurité avant tout refactor structurel.
2. **S3** — alias de lisibilité, débloque la lecture du reste.
3. **S1** — unification de nommage, mécanique après S8.
4. **S4** — factorisation reporting.
5. **S9** — docs.
6. **S2 + S5** ensemble — touchent tous deux `economy_base.h` / `links_base.h`, batcher pour éviter la merge pain.
7. **S6** (hoist seul, pas refonte variadic).
8. **S7** — décision d'équipe (yes/no), exécution conditionnelle.

## Fichiers critiques impactés

- `src/solver/variable/include/antares/solver/variable/economy/economy_base.h` (S1, S2, S4, S5)
- `src/solver/variable/include/antares/solver/variable/economy/links/links_base.h` (S1, S4, S5)
- `src/solver/variable/include/antares/solver/variable/economy/multi_column_base.h` (S1)
- `src/solver/variable/include/antares/solver/variable/economy/dynamic_multi_column_base.h` (S1)
- `src/solver/variable/include/antares/solver/variable/economy/DispatchablePlantByCluster_base.h` (S1)
- `src/solver/variable/include/antares/solver/variable/economy/STStorageByCluster_base.h` (S1)
- `src/solver/variable/include/antares/solver/variable/storage/results.h` (S3 — ajout d'alias)
- `src/solver/variable/include/antares/solver/variable/variable.h` / `variable.hxx` (S4, S6)
- `src/solver/variable/include/antares/solver/variable/endoflist.h` (S6)
- Nouveau `src/solver/variable/docs/TRAITS.md` (S9)
- Nouveau test `src/tests/src/solver/variable/test_traits_contract.cpp` (S8)

## Vérification

Pour chaque PR issue de ce plan :

```bash
cd /home/jmarechal/Projects/src
/home/jmarechal/miniconda3/bin/cmake --build --target all --preset Debug-vcpkg
cd /home/jmarechal/CLionProjects/build_simulator
ctest --output-on-failure -R "test-intermediate|test-surveyresults|test-dynamic-aggregation|test-setofareas-reports|test-residual-load|test-migrated-variables-metadata|test-traits-contract"
```

Pour S1/S2/S4/S5 : comparaison byte-à-byte du digest + annual survey avant/après sur une étude de référence couvrant area, link, cluster thermique, STS, renewable. Reprendre la méthode de capture utilisée pour les PRs C2a/b/c du plan dédup.

## Critères de done (globaux)

1. Nommage des hooks `Traits::` converge (≤ 2 noms pour « setHourlyValue »).
2. Chaque base publie son contrat `Traits` en tête de fichier (requis / optionnels / signatures).
3. Suite de tests complète passe sans modification après chaque PR.
4. Captures digest + survey byte-identiques avant/après pour les PRs structurelles (S1, S2, S4, S5).
5. Aucune API publique (`using Foo = Base<Traits, NextT>`) cassée.

## Risques

1. **Régression silencieuse de sortie** — S1/S4 manipulent le chemin de reporting. Mitigation : S8 en premier + comparaison digest byte-à-byte.
2. **Sur-engineering de S2** — le dispatch par policies peut alourdir la compilation ou réduire la lisibilité. Mitigation : prototype sur 1 base avant de généraliser.
3. **Merge pain S2+S5** — deux refactors sur les mêmes fichiers. Mitigation : les faire dans la même PR.
4. **Churn git S7** — `git blame` cassé, revues polluées. Mitigation : exécuter via `git mv` (pas `rm`+`add`) et avec `--follow` dans les review tools.
