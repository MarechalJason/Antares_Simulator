#!/usr/bin/env bash
set -euo pipefail

# Paramètres (modifiable)
START_COMMIT="ba6f5efde2b9eb5a546ad48b83d6b4bdfa6c221b"
END_COMMIT="7b7475268c93062504d803b5c6c06d7648ee719c"
REMOTE="origin"
BRANCH_PREFIX="fix/sortie_"
START_INDEX=1

# Mode: set DRY_RUN=0 pour pousser réellement
DRY_RUN=1

# Pré-vérifications
if [ -n "$(git status --porcelain)" ]; then
  echo "Erreur: l'arbre de travail a des modifications. Committez/stashez d'abord."
  git status --porcelain
  exit 1
fi

echo "Fetching remote ${REMOTE}..."
git fetch --all --prune

# recueillir les commits (inclusive de START_COMMIT)
mapfile -t commits < <(git rev-list --reverse "${START_COMMIT}^..${END_COMMIT}")

if [ ${#commits[@]} -eq 0 ]; then
  echo "Aucun commit trouvé dans la plage ${START_COMMIT}^..${END_COMMIT}"
  exit 0
fi

echo "Found ${#commits[@]} commits between ${START_COMMIT} and ${END_COMMIT} (inclusive)."

i=${START_INDEX}
for sha in "${commits[@]}"; do
  branch="${BRANCH_PREFIX}${i}"
  # vérifier si la branche existe localement ou à distance
  if git show-ref --verify --quiet "refs/heads/${branch}" || git ls-remote --exit-code --heads "${REMOTE}" "${branch}" >/dev/null 2>&1; then
    echo "Skip: branch '${branch}' already exists (local or remote)."
  else
    short=$(git rev-parse --short "$sha")
    subject=$(git show -s --format="%s" "$sha")
    echo "Mapping: ${branch} <- ${short}  (${subject})"
    if [ "${DRY_RUN}" -eq 0 ]; then
      echo "Pushing ${sha} to ${REMOTE} as refs/heads/${branch} ..."
      git push "${REMOTE}" "${sha}:refs/heads/${branch}"
      echo "Pushed ${branch}"
    else
      echo "[DRY-RUN] would push: ${sha} -> ${REMOTE}/refs/heads/${branch}"
    fi
  fi
  ((i++))
done

echo "Done."
