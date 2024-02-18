import matplotlib.pyplot as plot
import sys
import re
from itertools import takewhile
from itertools import product
import math

if len(sys.argv) > 1:
    with open(sys.argv[1]) as file:
        lines = [
            line_p
            for line in file
            if (line_p := line.rstrip().lstrip())
        ]
else:
    lines = [
        line_p
        for line in sys.stdin.readlines()
        if (line_p := line.rstrip().lstrip())
    ]

def is_float(f_string):
    if f_string is None:
        return False
    try:
        float(f_string)
        return True
    except ValueError:
        return False

contents = {
    entry: [
        float(entry)
        for entry in takewhile(lambda l: is_float(l), lines[index+1:])
    ]
    for index, entry in enumerate(lines)
    if not is_float(entry)
}

# import json
# print(json.dumps(contents, indent=2))

nrow = max(math.floor(math.sqrt(len(contents.keys()))), 2)
ncol = max(math.ceil(math.sqrt(len(contents.keys()))), 2)

plot.style.use('dark_background')
fig, ax = plot.subplots(nrow, ncol)

fig.suptitle('simc random generator histograms')
fig.tight_layout()

for (r, c), (k, v) in zip(product(range(nrow), range(ncol)), contents.items()):
    ax[r, c].set_title(k)
    ax[r, c].set_xlabel('Value')
    ax[r, c].set_ylabel('Samples')
    ax[r, c].hist(
        v,
        bins=64,
        linewidth=0.5,
        alpha=0.7
    )

plot.show()
