#!/usr/bin/env bash

set -euo pipefail

# ---- defaults (can be overridden by flags/env) ------------------------------
SRC_DIR="${SRC_DIR:-./build}"
LIB_BASE="${LIB_BASE:-MyFirstPass}"                                   # base name without prefix/suffix
PIPELINE='module(module-count-globals,function(hello-func,fn-stats))' # -passes pipeline (new PM)
INPUT="${INPUT:-./test.ll}"                                           # default input
OPT_BIN="${OPT_BIN:-opt}"                                             # path to opt (or just "opt")
DISABLE_OUTPUT=1                                                      # default: don't write bitcode
OUTFILE=""                                                            # set via -o to write output

# ---- helpers ----------------------------------------------------------------
die() {
  echo "error: $*" >&2
  exit 1
}

usage() {
  cat <<EOF
Usage: $(basename "$0") [options] [--] [input.ll]

Options:
  -s, --src-dir DIR       Directory containing the built plugin (default: $SRC_DIR)
  -b, --lib-base NAME     Plugin base name without 'lib' prefix (default: $LIB_BASE)
  -p, --pipeline STR      Pass pipeline for -passes=... (default: $PIPELINE)
  -O, --opt PATH          Path to 'opt' binary (default: $OPT_BIN)
  -o, --out FILE          Write transformed module to FILE (disables -disable-output)
  -v, --verbose           Print resolved paths and versions
  --legacy PIPE           Use LEGACY pass manager with -load/-LEGACYPASS (expert)
  -h, --help              Show this help

Environment vars respected:
  SRC_DIR, LIB_BASE, PIPELINE, INPUT, OPT_BIN

Examples:
  $(basename "$0") -s ./build -b MyFirstPass -p hello-func ./test.ll
  $(basename "$0") --out out.bc ./test.ll
EOF
}

# ---- parse args -------------------------------------------------------------
VERBOSE=0
USE_LEGACY=0
LEGACY_PIPE=""

ARGS=()
while [[ $# -gt 0 ]]; do
  case "$1" in
  -s | --src-dir)
    SRC_DIR="$2"
    shift 2
    ;;
  -b | --lib-base)
    LIB_BASE="$2"
    shift 2
    ;;
  -p | --pipeline)
    PIPELINE="$2"
    shift 2
    ;;
  -O | --opt)
    OPT_BIN="$2"
    shift 2
    ;;
  -o | --out)
    OUTFILE="$2"
    DISABLE_OUTPUT=0
    shift 2
    ;;
  -v | --verbose)
    VERBOSE=1
    shift
    ;;
  --legacy)
    USE_LEGACY=1
    LEGACY_PIPE="$2"
    shift 2
    ;;
  -h | --help)
    usage
    exit 0
    ;;
  --)
    shift
    ARGS+=("$@")
    break
    ;;
  -*) die "unknown option: $1 (see -h)" ;;
  *)
    ARGS+=("$1")
    shift
    ;;
  esac
done

# Input file: last positional, default to \$INPUT
if [[ ${#ARGS[@]} -gt 0 ]]; then
  INPUT="${ARGS[-1]}"
fi

# ---- platform & paths -------------------------------------------------------
OS="$(uname -s || true)"
case "$OS" in
Darwin) LIB_EXT="dylib" ;;
Linux) LIB_EXT="so" ;;
*) LIB_EXT="so" ;; # safe default for other Unix
esac

LIB_NAME="lib${LIB_BASE}.${LIB_EXT}"
PLUGIN_PATH="${SRC_DIR%/}/${LIB_NAME}"

# ---- validations ------------------------------------------------------------
command -v "$OPT_BIN" >/dev/null 2>&1 || die "'$OPT_BIN' not found in PATH. Use -O / --opt."

[[ -r "$INPUT" ]] || die "input IR not found: $INPUT"
[[ -r "$PLUGIN_PATH" ]] || die "plugin not found: $PLUGIN_PATH (looked in $SRC_DIR)"

# Check opt is new enough to support -load-pass-plugin (LLVM 11+; reliably 14+)
if ! "$OPT_BIN" --version >/dev/null 2>&1; then
  die "failed to run '$OPT_BIN --version'"
fi

# Extract major version if possible
OPT_VER_STR="$("$OPT_BIN" --version 2>/dev/null | head -n1 || true)"
OPT_MAJOR=""
if [[ "$OPT_VER_STR" =~ version[[:space:]]+([0-9]+) ]]; then
  OPT_MAJOR="${BASH_REMATCH[1]}"
fi

if [[ -n "$OPT_MAJOR" && "$OPT_MAJOR" -lt 11 && $USE_LEGACY -eq 0 ]]; then
  die "opt version appears to be <$OPT_MAJOR>; new pass manager likely unsupported. Use --legacy."
fi

# ---- verbose dump -----------------------------------------------------------
if [[ $VERBOSE -eq 1 ]]; then
  echo "OS               : $OS"
  echo "opt              : $OPT_BIN ($OPT_VER_STR)"
  echo "src dir          : $SRC_DIR"
  echo "plugin path      : $PLUGIN_PATH"
  echo "pipeline (new PM): $PIPELINE"
  if [[ $USE_LEGACY -eq 1 ]]; then
    echo "legacy pipeline  : $LEGACY_PIPE"
  fi
  if [[ $DISABLE_OUTPUT -eq 1 ]]; then
    echo "output           : disabled (-disable-output)"
  else
    echo "output file      : $OUTFILE"
  fi
  echo "input            : $INPUT"
fi

# ---- run opt ----------------------------------------------------------------
set -x
if [[ $USE_LEGACY -eq 1 ]]; then
  # Legacy pass manager path (example: -hello). You must provide the correct pass flag.
  # Some LLVM builds require -load before the pass flag.
  "$OPT_BIN" -load "$PLUGIN_PATH" $LEGACY_PIPE "$INPUT" ${OUTFILE:+-o "$OUTFILE"}
else
  # New pass manager path
  if [[ $DISABLE_OUTPUT -eq 1 ]]; then
    "$OPT_BIN" -load-pass-plugin "$PLUGIN_PATH" -time-passes -passes="$PIPELINE" -disable-output "$INPUT"
  else
    "$OPT_BIN" -load-pass-plugin "$PLUGIN_PATH" -time-passes -passes="$PIPELINE" "$INPUT" -o "$OUTFILE"
  fi
fi
set +x
