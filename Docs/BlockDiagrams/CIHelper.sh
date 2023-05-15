#!/bin/bash
file=$1 # File name excluding extension

# Export diagram to plain XML
drawio --export --format xml --uncompressed "$file.drawio"

# Count how many pages based on <diagram element
count=$(grep -o "<diagram" "$file.xml" | wc -l)

# Export each page as an PNG
# Page index is zero based
for ((i = 0 ; i <= $count-1; i++)); do
  drawio --export --page-index $i --output "$file-$i.png" "$file.drawio"
done

