import sys
import os
import subprocess

# Constants.
FILENAME_OUTPUT_STDOUT = 'stdout.log'
FILENAME_OUTPUT_STDERR = 'stderr.log'
FILENAME_OUTPUT_RETURNCODE = 'returncode.log'

# Usage.
if len(sys.argv) < 3:
    print('Usage: python run.py <program> <time limit in seconds> <arg0> <arg1> ... <argN>')
    exit(1)

# Setup.
program = sys.argv[1] # os.path.abspath(sys.argv[1])
tl_in_seconds = int(sys.argv[2])
args = sys.argv[3:]

# Run process.
try:
    results = subprocess.run(
        [program] + args,
        stdin = subprocess.PIPE,
        stdout = subprocess.PIPE,
        stderr = subprocess.PIPE,
        timeout = tl_in_seconds, 
        universal_newlines = True
    )

    # Output, when no timeout error.
    with open(FILENAME_OUTPUT_STDOUT, 'w') as output_stdout:
        output_stdout.write(results.stdout)
    with open(FILENAME_OUTPUT_STDERR, 'w') as output_stderr:
        output_stderr.write(results.stderr)
    with open(FILENAME_OUTPUT_RETURNCODE, 'w') as output_returncode:
        output_returncode.write(str(results.returncode))

except subprocess.TimeoutExpired:
    print('Timeout expired.')
    exit(255) 
