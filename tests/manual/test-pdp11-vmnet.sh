#!/opt/local/bin/bash
set -euo pipefail

#
# Manual PDP-11 vmnet attach test
# ===============================
#
# Purpose
# -------
# This script is a small manual smoke test for the PDP-11 DELQA/DEQNA
# Ethernet controller (`XQ`) using SIMH's host networking transports,
# especially the macOS `vmnet.framework` support.
#
# It exists to answer a narrow question:
#   "Can the built `pdp11` binary actually attach `XQ` to the requested
#    host transport on this machine?"
#
# What it does
# ------------
# The script starts `BIN/pdp11` non-interactively and feeds it a tiny SCP
# command sequence:
#   1. `attach xq <target>`
#   2. `show xq`
#   3. `show ethernet`
#   4. `exit`
#
# The full transcript is written to a log file so attach failures and device
# state can be inspected after the run.
#
# Typical uses
# ------------
#   sudo tools/test-pdp11-vmnet.sh
#   sudo tools/test-pdp11-vmnet.sh vmnet:shared
#   sudo tools/test-pdp11-vmnet.sh vmnet:en0
#   tools/test-pdp11-vmnet.sh udp:10000:127.0.0.1:10001
#
# Why `sudo` is often needed
# --------------------------
# On a normal local source build, the `vmnet.framework` transports usually
# require elevated privileges or an appropriately entitled/notarized build.
# So `vmnet:host`, `vmnet:shared`, and bridged `vmnet:<device>` should usually
# be tested with `sudo`.
#
# Scope
# -----
# This is a manual host-side diagnostic script, not a regression test.  It is
# useful for verifying:
#   - that vmnet support was compiled in
#   - that `SHOW ETHERNET` advertises vmnet transports
#   - that `XQ` can attach to `vmnet:host`, `vmnet:shared`, or
#     `vmnet:<device>` on the current macOS host
#
# It is not intended to verify guest networking end-to-end.
#
repo_root=$(
  cd "$(dirname "$0")/.." >/dev/null 2>&1
  pwd
)

sim_bin="${SIMH_PDP11_BIN:-$repo_root/BIN/pdp11}"
attach_target="${1:-vmnet:host}"
transcript="${2:-$repo_root/tmp/pdp11-vmnet-test.log}"

if [[ ! -x "$sim_bin" ]]; then
  printf 'pdp11 binary not found or not executable: %s\n' "$sim_bin" >&2
  printf 'Build it first, for example:\n' >&2
  printf '  make -j8 BIN/pdp11\n' >&2
  exit 1
fi

mkdir -p "$(dirname "$transcript")"

if [[ "$attach_target" == vmnet:* && "${EUID}" -ne 0 ]]; then
  printf 'Warning: vmnet usually requires root on a normal source build.\n' >&2
  printf 'You may want to rerun this as:\n' >&2
  printf '  sudo %q %q\n' "$0" "$attach_target" >&2
fi

printf 'Using simulator: %s\n' "$sim_bin"
printf 'Attach target:   %s\n' "$attach_target"
printf 'Transcript:      %s\n' "$transcript"

cat <<EOF | "$sim_bin" | tee "$transcript"
attach xq $attach_target
show xq
show ethernet
exit
EOF

printf '\nKey lines:\n'
grep -E '(%SIM-|attached to |not attached|ETH devices:|vmnet:|udp:)' \
  "$transcript" || true
