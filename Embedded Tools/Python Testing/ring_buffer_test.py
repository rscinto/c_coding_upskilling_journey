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

result = subprocess.run(
    [r"C:\Users\rocco\CLionProjects\relearn_ch1\cmake-build-debug\relearn_ch1.exe"],
    input=commands,
    text=True,
    capture_output=True
)

print(result.stdout)