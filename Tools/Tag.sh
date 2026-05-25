#!/usr/bin/env bash
set -euo pipefail

MODE="minor"
PUSH=false
DRY_RUN=false
SKIP_GRAPH=false

usage() {
  cat <<'USAGE'
Usage:
  ./Tools/Tag.sh [minor|patch|major|init] [--push] [--dry-run] [--skip-graph]

Behavior:
  first run with no existing version tag -> 1.0.0
  default / minor                  -> 1.1.0, 1.2.0, ...
  patch                            -> 1.1.1, 1.1.2, ...
  major                            -> 2.0.0, 3.0.0, ...
  init                             -> force the initial 1.0.0 tag

Options:
  --push       Push the new tag to origin after creating it.
  --dry-run    Print what would happen without creating or pushing tags.
  --skip-graph Skip Graphify knowledge graph generation.
  -h, --help   Show this help text.
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
    --dry-run)
      DRY_RUN=true
      ;;
    --skip-graph)
      SKIP_GRAPH=true
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

graphify_command() {
  if [[ -x "$repo_root/venv/bin/graphify" ]]; then
    echo "$repo_root/venv/bin/graphify"
  elif command -v graphify >/dev/null 2>&1; then
    command -v graphify
  else
    return 1
  fi
}

generate_knowledge_graph() {
  local graphify_cmd="$1"

  echo "$repo_name: generating Graphify knowledge graph"
  if ! "$graphify_cmd" update .; then
    echo "$repo_name: Graphify update failed; building a fresh graph"
    "$graphify_cmd" update . --force
  fi
  normalize_graphify_paths
}

normalize_graphify_paths() {
  local output_dir="$repo_root/graphify-out"

  if [[ ! -d "$output_dir" ]]; then
    return
  fi

  python3 - "$repo_root" "$output_dir" <<'PY'
import json
import os
import pathlib
import re
import sys

repo_root = pathlib.Path(sys.argv[1]).resolve()
output_dir = pathlib.Path(sys.argv[2]).resolve()
repo_root_text = str(repo_root)
repo_root_prefix = repo_root_text + os.sep
sanitized_repo_root = re.sub(r"[^0-9A-Za-z]+", "_", repo_root_text.strip(os.sep)).strip("_").lower()
sanitized_repo_prefix = sanitized_repo_root + "_"

def relativize_text(value):
    if not isinstance(value, str):
        return value
    if value == repo_root_text:
        return "."
    return (
        value
        .replace(repo_root_prefix, "")
        .replace(repo_root_text, ".")
        .replace(sanitized_repo_prefix, "")
        .replace(sanitized_repo_root, ".")
    )

def relativize_json(value):
    if isinstance(value, dict):
        return {relativize_text(key): relativize_json(item) for key, item in value.items()}
    if isinstance(value, list):
        return [relativize_json(item) for item in value]
    return relativize_text(value)

root_marker = output_dir / ".graphify_root"
if root_marker.exists():
    root_marker.write_text(".\n", encoding="utf-8")

for path in output_dir.rglob("*"):
    if not path.is_file() or path.name == ".graphify_root":
        continue

    if path.suffix == ".json":
        try:
            data = json.loads(path.read_text(encoding="utf-8"))
        except (UnicodeDecodeError, json.JSONDecodeError):
            continue
        path.write_text(json.dumps(relativize_json(data), indent=2) + "\n", encoding="utf-8")
        continue

    if path.suffix in {".html", ".md", ".txt"}:
        try:
            text = path.read_text(encoding="utf-8")
        except UnicodeDecodeError:
            continue
        path.write_text(relativize_text(text), encoding="utf-8")
PY
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
current_tag="$(latest_version_tag)"
new_tag="$(next_version "$current_tag" "$MODE")"
commit_sha="$(git rev-parse --short HEAD)"

if git rev-parse -q --verify "refs/tags/$new_tag" >/dev/null; then
  echo "$repo_name: tag $new_tag already exists; skipping"
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
  exit 0
fi

if [[ "$SKIP_GRAPH" == false ]]; then
  if graphify_cmd="$(graphify_command)"; then
    generate_knowledge_graph "$graphify_cmd"
  else
    echo "$repo_name: graphify command not found; run ./Tools/Setup.sh first or install graphifyy" >&2
    exit 1
  fi
fi

git tag -a "$new_tag" -m "Release $new_tag"
echo "$repo_name: created tag $new_tag on $commit_sha"

if [[ "$PUSH" == true ]]; then
  git push origin "$new_tag"
  echo "$repo_name: pushed tag $new_tag to origin"
fi
