#!/bin/bash

# Script to check for dead links within all files in a repository

# Configuration
CHECK_EXTERNAL=1  # Set to 0 if you only want to check internal links
TIMEOUT=5         # Timeout in seconds for external link checking
USER_AGENT="Mozilla/5.0 Link Checker"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

echo "=== Dead Link Checker ==="
echo "Searching for links in the repository..."

# Find all markdown, mdx, h, and c files
FILES=$(find . -type f \( -name "*.md" -o -name "*.mdx" -o -name "*.c" -o -name "*.h" \) -not -path "*/\.*" -not -path "*/node_modules/*")

# Counters for statistics
total_links=0
dead_links=0
skipped_links=0

# Function to check if a link is alive
check_link() {
    local link=$1
    local file=$2
    
    # Handle internal links (starting with #, ./, or /)
    if [[ "$link" =~ ^# || "$link" =~ ^\./ || "$link" =~ ^/ ]]; then
        # This is a simplified check for internal links
        # A more sophisticated version would validate if the internal link target exists
        return 0
    fi
    
    # Skip certain protocols
    if [[ "$link" =~ ^(mailto:|tel:) ]]; then
        ((skipped_links++))
        return 0
    fi
    
    # Check external links if enabled
    if [ "$CHECK_EXTERNAL" -eq 1 ] && [[ "$link" =~ ^https?:// ]]; then
        if curl --user-agent "$USER_AGENT" --location --head --silent --fail --max-time $TIMEOUT "$link" > /dev/null 2>&1; then
            return 0
        else
            return 1
        fi
    fi
    
    return 0
}

# Process each file
for file in $FILES; do
    echo -e "${YELLOW}Checking $file...${NC}"
    
    # Extract links based on file extension
    if [[ "$file" == *.md ]]; then
        # Markdown links [text](url)
        links=$(grep -o -E '\[.*?\]\(.*?\)' "$file" | sed -E 's/\[.*?\]\((.*?)(\s+".*?")?\)/\1/g')
    elif [[ "$file" == *.html ]]; then
        # HTML links href="url"
        links=$(grep -o -E 'href="[^"]*"' "$file" | sed -E 's/href="([^"]*)"/\1/g')
    elif [[ "$file" == *.adoc ]]; then
        # AsciiDoc links
        links=$(grep -o -E '(https?://[^ \t\n\r\)\]]+|link:[^\[]+\[[^\]]+\])' "$file" | sed -E 's/link:([^\[]+)\[.*/\1/g')
    else
        # Plain text URLs
        links=$(grep -o -E 'https?://[^ \t\n\r\)\]]+' "$file")
    fi
    
    # Check each link
    echo "$links" | while read -r link; do
        if [ -z "$link" ]; then
            continue
        fi
        
        ((total_links++))
        
        if check_link "$link" "$file"; then
            echo -e "  ${GREEN}✓${NC} $link"
        else
            ((dead_links++))
            echo -e "  ${RED}✗${NC} $link in $file"
        fi
    done
done

# Print summary
echo "=== Summary ==="
echo "Total links checked: $total_links"
echo "Dead links found: $dead_links"
echo "Skipped links: $skipped_links"

if [ "$dead_links" -gt 0 ]; then
    echo -e "${RED}Found $dead_links dead links!${NC}"
    exit 1
else
    echo -e "${GREEN}All links are valid!${NC}"
    exit 0
fi
