# Find all source/header files

find ../source -type f \( -name "*.cpp" -o -name "*.c" -o -name "*.h" -o -name "*.hpp" \) \
  -exec printf "indent -bap -bli0 -i8 -l120 -ncs -npcs -npsl -fca -lc79 -fc1 -ts8 \"%s\"\n" {} \; \
  -exec indent -bap -bli0 -i8 -l120 -ncs -npcs -npsl -fca -lc79 -fc1 -ts8 {} \;
