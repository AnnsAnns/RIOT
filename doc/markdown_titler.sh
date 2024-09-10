---
title: magic.sh
description: magic.sh
---
#!/bin/bash

# Directory to search files (defaults to current directory if not specified)
DIR=${1:-.}

# Loop through each file in the directory
for file in "$DIR"/*; do
  if [[ -f $file ]]; then
    FILE_NAME=$(basename "$file")

    # Insert the front matter at the beginning of the file
    {
      echo "---"
      echo "title: $FILE_NAME"
      echo "description: $FILE_NAME"
      echo "---"
      cat "$file"
    } > temp_file && mv temp_file "$file"
    
    echo "Processed: $file"
  fi
done
