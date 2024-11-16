# build.sh builds ../source directory (clears the screen)
# this file is in the public domain

# output build file
outputfile="../edit.exe"
outputparam="-o"

# build utility
clexe=g++

# flags for the compiler (change as you will)
cflags="" 
# flags for the linker (change as you will)
lflags="-lpdcurses -static"

# no need to update anything bellow this line

echo build.sh

flags="$cflags $lflags $@ $outputparam $outputfile "

# find each c++ and c file run g++
echo BUILDING ...

# Find all source/header files
source=$(find ../source -type f \( -iname "*.cpp" -o -iname "*.c" \))

# List of files (improved handling of spaces)
printf "%s\n" $source

$clexe $source $flags

printf '\a'