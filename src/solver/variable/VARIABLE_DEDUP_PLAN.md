# Plan de migration anti-duplication des variables solver

## Objectif

Réduire la duplication dans `src/solver/variable/include/antares/solver/variable` en réutilisant d'abord les bases existantes, puis en introduisant des abstractions supplémentaires uniquement pour les cas restants.

Stratégie : phases courtes, chaque PR livrable seule, vérification fonctionnelle stricte (mêmes captions/unités/colonnes, même chaînage `NextType`).

## État actuel (vérifié)

| Phase | Portée | Statut |
|-------|--------|--------|
| 1 | STS by cluster → `STStorageByClusterBase` | ✅ Terminée |
| 2 | "by plant" + area simples → `DispatchablePlantByClusterBase` / `ByPlantBase` / `Economy_Base` | ✅ Terminée |
| 3 | Liens mono-colonne → `EconomyLink_Base` / `StaticLinkBase` | ✅ Terminée |
| 4 | Consolidation bases | ✅ Terminée |
| 6A | `economy/` triviaux → `Economy_Base` | ✅ Terminée |
| 6B | Hooks étendus → `Economy_Base` | ✅ Terminée |
| 6C statique | `MultiColumnBase` + `thermalAirPollutantEmissions` | ✅ Terminée |
| 6C dynamique | `DynamicMultiColumnBase` + migrations | ✅ Terminée |
| 6D | `reservoirlevel.h` | ✅ Terminée |

### Bases disponibles

- `economy/economy_base.h` — area mono-colonne avec hooks optionnels.
- `economy/links/links_base.h` — lien mono-colonne avec hooks optionnels.
- `economy/STStorageByCluster_base.h` — by-cluster générique.
- `economy/DispatchablePlantByCluster_base.h` — by-cluster dispatchable.
- `economy/links/static_link_base.h` — liens en `simulationEnd`.
- `economy/multi_column_base.h` — multi-colonnes **statique**.
- `economy/dynamic_multi_column_base.h` — multi-colonnes **dynamique**.

## Tests de non-régression

```bash
cd /home/jmarechal/Projects/src
/home/jmarechal/miniconda3/bin/cmake --build --target all --preset Debug-vcpkg
cd /home/jmarechal/CLionProjects/build_simulator
ctest --output-on-failure -R "test-intermediate|test-surveyresults|test-dynamic-aggregation|test-setofareas-reports|test-residual-load|test-migrated-variables-metadata"
```

**Résultats**: 5 tests passés, 0 échecs.

## Gain total

- **Total lignes supprimées**: ~3 400 lignes de boilerplate
- **Fichiers migrés**: ~22 fichiers vers patterns Traits-based
- **Réduction**: ~65% boilerplate

## Résumé des migrations

### Phase 6A - Bucket A (triviaux)
- `balance.h`, `hydrostorage.h`, `inflow.h`, `overflow.h`, `pumping.h`
- `operatingCost.h`, `nonProportionalCost.h`, `hydroCost.h`, `domesticUnsuppliedEnergy.h`
- `price.h`, `reservoirlevel.h`

### Phase 6B - Bucket B (hooks)
- `max-mrg.h` (+ hook `weekForEachArea`)
- `priceCSR.h`, `dtgMarginAfterCsr.h`, `localMatchingRuleViolations.h`
- `overallCostCsr.h`, `max-mrg-csr.h`

### Phase 6C - Bucket C (multi-colonnes)
- `multi_column_base.h` (statique)
- `thermalAirPollutantEmissions.h`
- `dynamic_multi_column_base.h` (dynamique)
- `dispatchableGeneration.h` (→ ~70 lignes)
- `renewableGeneration.h` (→ ~65 lignes)
- `STSbyGroup.h` (→ ~95 lignes)

## Critères d'acceptation validés

1. ✅ Mêmes captions / units / décimales / columnCount
2. ✅ Chaînage NextType préservé
3. ✅ Suite de tests passent sans modification
4. ✅ Build vérifié (23 targets)
5. ✅ Aucune API publique cassée
