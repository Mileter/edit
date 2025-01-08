# Run indent, passing the combined list of files
./indent_all.sh

# Remove old files that have ~ after them
find ../source -type f \( -name "*.cpp~" -o -name "*.c~" -o -name "*.h~" -o -name "*.hpp~" \) -exec rm -f {} \; -exec printf "rm -f \"%s\"\n" {} \;

# Optional bell sound
printf '\a'
