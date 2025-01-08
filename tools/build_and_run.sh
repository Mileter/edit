# build_and_run.sh does the following two's abilities
# build.sh builds ../source directory (clears the screen)
# run.sh -- runs the finished executable
# this file is in the public domain

# output build file
outputfile="../edit.exe"
outputparam="-o"

# build utility
clexe=g++

# flags for the compiler (change as you will)
cflags="$(pkgconf ncursesw --cflags)" 
# flags for the linker (change as you will)
lflags="$(pkgconf ncursesw --libs --static)"

# no need to update anything bellow this line

echo build.sh

flags="$cflags $lflags $@ $outputparam $outputfile "

# find each c++ and c file run g++
echo BUILDING ...

# Find all source/header files
source=$(find ../source -type f \( -iname "*.cpp" -o -iname "*.c" \))

# List of files (improved handling of spaces)
printf "%s\n" $source

if $clexe $source $flags; then
	echo run.sh
	start $outputfile
fi
printf '\a'
