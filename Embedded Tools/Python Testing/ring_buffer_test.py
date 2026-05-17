from pathlib import Path
import subprocess

commands = """\
push A
push B
push C
push A
push B
push C
push A
push B
push C
pop
pop
pop
pop
"""

project_root = Path(__file__).resolve().parent.parent

exe_path = project_root / "cmake-build-debug" / "relearn_ch1.exe"

result = subprocess.run(
    [str(exe_path)],
    input=commands,
    text=True,
    capture_output=True
)

print(result.stdout)
print(result.stderr)