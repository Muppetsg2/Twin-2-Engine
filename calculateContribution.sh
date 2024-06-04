#!/bin/bash

# Step 1: Navigate to the root directory of your Git project

# Step 2: List all .cpp and .h files in the src directory and save to a temporary file
find ./src -type f \( -name '*.cpp' -o -name '*.h' \) > file_list.txt

# Step 3: Initialize an empty file to store blame results
echo "" > blame_output.txt

# Step 4: Run git blame on each file and append to the blame output file
while IFS= read -r file; do
    git blame "$file" >> blame_output.txt
done < file_list.txt

# Step 5: Use Python to process the blame output and calculate contributions
python << 'EOF'
import re
from collections import defaultdict

# Read blame output
with open('blame_output.txt', 'r') as f:
    blame_output = f.readlines()

# Initialize a dictionary to count lines per author
author_lines = defaultdict(int)

# Regex to extract author from each line of git blame output
blame_line_regex = re.compile(r'^\^?\w+\s\((.*?)\s+\d{4}-\d{2}-\d{2}')

# Count lines for each author
for line in blame_output:
    match = blame_line_regex.match(line)
    if match:
        author = match.group(1).strip()
        author_lines[author] += 1

# Calculate total lines
total_lines = sum(author_lines.values())

# Calculate and print percentage contribution for each author
print(f'Total lines of code: {total_lines}\n')
for author, lines in author_lines.items():
    percentage = (lines / total_lines) * 100
    print(f'{author}: {lines} lines ({percentage:.2f}%)')

EOF

# Cleanup temporary files
rm file_list.txt blame_output.txt
