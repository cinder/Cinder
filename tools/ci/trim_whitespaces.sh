#!/bin/bash

# Default values
DEFAULT_FOLDERS=("src/cinder" "include/cinder" "test" "samples")
DEFAULT_EXTENSIONS=("*.cpp" "*.h" "*.mm" "*.java" "*.txt" "*.rc" "*.glsl" "*.frag" "*.vert" "*.comp")
DEFAULT_CWD="./"

# Function to display help
display_help() {
  echo "Usage: $0 [options]"
  echo "Options:"
  echo "  --path PATH         Relative path to the folders (default: './' for CWD)"
  echo "  --folders FOLDERS   Comma-separated list of folders (default: 'src,include,tests')"
  echo "  --extensions EXTS   Comma-separated list of extensions (default: '*.cpp,*.h,*.hpp,*.txt')"
  echo "  --preview           Enable preview mode (list files without making changes)"
  echo "  --help              Display this help message"
  exit 0
}

# Parse named arguments
while [[ "$#" -gt 0 ]]; do
  case "$1" in
    --path)
      RELATIVE_PATH="$2"
      shift 2
      ;;
    --folders)
      IFS=',' read -r -a FOLDERS <<< "$2"
      shift 2
      ;;
    --extensions)
      IFS=',' read -r -a EXTENSIONS <<< "$2"
      shift 2
      ;;
    --preview)
      PREVIEW_MODE=true
      shift
      ;;
    --help)
      display_help
      ;;
    *)
      echo "Unknown argument: $1"
      display_help
      ;;
  esac
done

# Set default values if arguments are not provided
RELATIVE_PATH="${RELATIVE_PATH:-$DEFAULT_CWD}"
FOLDERS=("${FOLDERS[@]:-${DEFAULT_FOLDERS[@]}}")
EXTENSIONS=("${EXTENSIONS[@]:-${DEFAULT_EXTENSIONS[@]}}")
PREVIEW_MODE="${PREVIEW_MODE:-false}"

# Prepend relative path to each folder (except for "current")
if [[ "$RELATIVE_PATH" != "$DEFAULT_CWD" ]]; then
  FOLDERS=("${FOLDERS[@]/#/$RELATIVE_PATH/}")
fi

# Function to process files
process_files() {
  if [[ "$PREVIEW_MODE" == true ]]; then
    echo "Preview mode: Listing files that would be processed..."
    find "${FOLDERS[@]}" -type f \( $(printf -- "-name %s -o " "${EXTENSIONS[@]}") -false \) \
      -exec echo "Processing: {}" \;
  else
    echo "Action mode: Removing trailing whitespace..."
    find "${FOLDERS[@]}" -type f \( $(printf -- "-name %s -o " "${EXTENSIONS[@]}") -false \) \
      -exec sed -i 's/[[:space:]]*$//' {} \;
  fi
}

process_files