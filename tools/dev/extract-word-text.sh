#!/usr/bin/env bash
# Extract plain-text versions of all legacy Word documents into tmp/
# so they can be compared against Markdown conversions during review.
set -euo pipefail

repo_root="$(cd "$(dirname "$0")/../.." && pwd)"
src_dir="$repo_root/docs/legacy-word"
out_dir="${1:-$repo_root/tmp/word-text}"
stage_dir="$(mktemp -d "$repo_root/tmp/word-text-stage.XXXXXX")"
manifest="$out_dir/manifest.tsv"

cleanup() {
  rm -rf "$stage_dir"
}
trap cleanup EXIT

if ! command -v soffice >/dev/null 2>&1; then
  echo "error: soffice not found on PATH" >&2
  exit 1
fi

if [ ! -d "$src_dir" ]; then
  echo "error: source directory not found: $src_dir" >&2
  exit 1
fi

mkdir -p "$out_dir"
: >"$manifest"

find "$src_dir" -maxdepth 1 -type f \( -name '*.doc' -o -name '*.docx' \) \
  | sort | while IFS= read -r src; do
    base="$(basename "$src")"
    stem="${base%.*}"
    stage_txt="$stage_dir/$stem.txt"
    final_txt="$out_dir/$stem.txt"

    rm -f "$stage_txt" "$final_txt"

    echo "extracting: $base"
    soffice --headless \
      --convert-to "txt:Text (encoded):UTF8" \
      --outdir "$stage_dir" \
      "$src" >/dev/null

    if [ ! -f "$stage_txt" ]; then
      echo "error: conversion produced no output for $base" >&2
      exit 1
    fi

    mv "$stage_txt" "$final_txt"
    printf '%s\t%s\n' "$src" "$final_txt" >>"$manifest"
  done

echo
echo "wrote extracted text files to: $out_dir"
echo "wrote manifest to: $manifest"
