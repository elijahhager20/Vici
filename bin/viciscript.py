import subprocess
import os
import sys

VICI_EXEC = os.path.join(os.path.dirname(os.path.abspath(__file__)), "viciw.exe")
VICI_CURR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "user_repos", ".vicicurr")

def get_current_repo():
    try:
        with open(VICI_CURR, "r") as f:
            repo = f.readline().strip()
            return repo
    except Exception:
        return ""

def run_vici_command(args):
    """run a vici command and print its output"""
    try:
        result = subprocess.run([VICI_EXEC] + args, capture_output=True, text=True)
        print(result.stdout, end="")
        if result.stderr:
            print(result.stderr, end="", file=sys.stderr)
    except Exception as e:
        print(f"Error running Vici: {e}", file=sys.stderr)

def main():
    print("ViciScript DevOps REPL. Type 'exit' to quit.")
    try:
        while True:
            curr_repo = get_current_repo()
            prompt = f"@{curr_repo}: " if curr_repo else "viciscript> "
            cmd = input(prompt).strip()
            if not cmd:
                continue
            if cmd.lower() == "exit":
                break
            args = cmd.split()
            run_vici_command(args)
    except (EOFError, KeyboardInterrupt):
        print()
    finally:
        try:
            with open(VICI_CURR, "w") as f:
                f.write("")
        except Exception:
            pass

if __name__ == "__main__":
    main()