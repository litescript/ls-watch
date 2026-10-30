#!/usr/bin/env bash
set -euo pipefail

# ls-watch development dependency installer.
# Usage:
#   ./scripts/install-deps.sh
#   ./scripts/install-deps.sh --dry-run

PACKAGES=(
  base-devel
  arm-none-eabi-gcc
  arm-none-eabi-binutils
  arm-none-eabi-newlib
  cmake
  ninja
  sdl2
  git
)

DRY_RUN=0

case "${1:-}" in
"")
  ;;
--dry-run)
  DRY_RUN=1
  ;;
*)
  echo "Usage: $0 [--dry-run]" >&2
  exit 2
  ;;
esac

if [[ "$(uname -s)" != "Linux" ]]; then
  echo "Error: only Linux is supported currently." >&2
  exit 1
fi

if [[ ! -r /etc/os-release ]]; then
  echo "Error: /etc/os-release not found." >&2
  exit 1
fi

# shellcheck disable=SC1091
source /etc/os-release

case " ${ID:-} ${ID_LIKE:-} " in
*" arch "*)
  ;;
*)
  echo "Unsupported distribution: ${ID:-unknown}" >&2
  echo "Currently supported: Arch-based Linux." >&2
  exit 1
  ;;
esac

if ! command -v pacman >/dev/null 2>&1; then
  echo "Error: pacman not found." >&2
  exit 1
fi

echo "ls-watch dependency installer"
echo "Distribution: ${PRETTY_NAME:-$ID}"
echo "Package manager: pacman"
echo

if ((DRY_RUN)); then
  echo "Dry run. Would execute:"
  printf 'sudo pacman -S --needed'
  printf ' %q' "${PACKAGES[@]}"
  printf '\n'
  exit 0
fi

echo "Installing missing development packages..."
sudo pacman -S --needed "${PACKAGES[@]}"

echo
echo "Dependencies installed."
