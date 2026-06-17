#!/usr/bin/env bash
set -euo pipefail

MODE="minor"
PUSH=true
DRY_RUN=false

usage() {
  cat <<'USAGE'
Usage:
  ./Tools/Tag.sh [minor|patch|major|init] [--dry-run] [--local-only]

Behavior:
  first run with no existing version tag -> 1.0.0
  default / minor                  -> 1.1.0, 1.2.0, ...
  patch                            -> 1.1.1, 1.1.2, ...
  major                            -> 2.0.0, 3.0.0, ...
  init                             -> force the initial 1.0.0 tag

Options:
  --dry-run    Print what would happen without creating or pushing tags.
  --local-only Create the tag locally without pushing it to origin.
  --push       Push the new tag to origin after creating it (default).
  -h, --help   Show this help text.

Graphify generation and graphify snapshot branches are disabled.
USAGE
}

for arg in "$@"; do
  case "$arg" in
    minor|patch|major|init)
      MODE="$arg"
      ;;
    --push)
      PUSH=true
      ;;
    --local-only)
      PUSH=false
      ;;
    --dry-run)
      DRY_RUN=true
      ;;
    --skip-graph|--snapshot-branch|--no-snapshot-branch)
      echo "Ignoring disabled Graphify/snapshot option: $arg" >&2
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      echo "Unknown argument: $arg" >&2
      usage >&2
      exit 2
      ;;
  esac
done

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd "$script_dir/.." && pwd)"

cd "$repo_root"

latest_version_tag() {
  git tag --list '[0-9]*.[0-9]*.[0-9]*' --sort=-v:refname | head -n 1
}

require_merged_master() {
  local current_branch local_sha remote_sha

  current_branch="$(git branch --show-current)"
  if [[ "$current_branch" != "master" ]]; then
    echo "$repo_name: tagger must be run from master after the PR is merged; current branch is $current_branch" >&2
    exit 1
  fi

  git fetch origin master --tags

  local_sha="$(git rev-parse HEAD)"
  remote_sha="$(git rev-parse origin/master)"

  if [[ "$local_sha" != "$remote_sha" ]]; then
    echo "$repo_name: local master is not up to date with origin/master" >&2
    echo "$repo_name: run git pull --ff-only, then run the tagger again" >&2
    exit 1
  fi
}

next_version() {
  local current="$1"
  local mode="$2"
  local major minor patch

  if [[ -z "$current" ]]; then
    echo "1.0.0"
    return
  fi

  IFS='.' read -r major minor patch <<< "$current"

  case "$mode" in
    init)
      echo "1.0.0"
      ;;
    minor)
      echo "${major}.$((minor + 1)).0"
      ;;
    patch)
      echo "${major}.${minor}.$((patch + 1))"
      ;;
    major)
      echo "$((major + 1)).0.0"
      ;;
  esac
}

repo_name="$(basename "$repo_root")"

if [[ "$PUSH" == true ]]; then
  require_merged_master
fi

current_tag="$(latest_version_tag)"
new_tag="$(next_version "$current_tag" "$MODE")"
commit_sha="$(git rev-parse --short HEAD)"

if git rev-parse -q --verify "refs/tags/$new_tag" >/dev/null; then
  echo "$repo_name: tag $new_tag already exists; skipping"
  exit 0
fi

if [[ "$PUSH" == true ]] && git ls-remote --exit-code --tags origin "$new_tag" >/dev/null 2>&1; then
  echo "$repo_name: tag $new_tag already exists on origin; skipping"
  exit 0
fi

if [[ -n "$(git status --porcelain)" ]]; then
  echo "$repo_name: warning: working tree has uncommitted changes; tagging committed HEAD $commit_sha"
fi

if [[ "$DRY_RUN" == true ]]; then
  if [[ -z "$current_tag" ]]; then
    echo "$repo_name: would create tag $new_tag on $commit_sha"
  else
    echo "$repo_name: would create tag $new_tag on $commit_sha (latest tag: $current_tag)"
  fi
  if [[ "$PUSH" == true ]]; then
    echo "$repo_name: would push tag $new_tag to origin"
  fi
  exit 0
fi

git tag -a "$new_tag" -m "Release $new_tag"
echo "$repo_name: created tag $new_tag on $commit_sha"

if [[ "$PUSH" == true ]]; then
  git push origin "$new_tag"
  echo "$repo_name: pushed tag $new_tag to origin"
fi
