#!/bin/bash

usage() {
	echo "usage: $0 -l lab [-s subdir] [-f flags] [-n] [-c]"
	echo "This tool creates a copy of xv6-public/, patches it with a selected lab folder and excutes the qemu emulator"
	echo "-l The lab to be built"
	echo "-s The subdir (withitn each lab) to be built. Default=end"
	echo "-f Flags to be passed to the compiler. Default=none"
	echo "-n Avoid the creation of a new copy of xv6-public/. Useful for development. Default=false"
	echo "-c Clean all patched copies of xv6"
	exit 1
}

# set initial values
SUBDIR="end"
NO_INIT=0
CLEAN=0
FLAGS=""

while getopts "s:f:ncl:" o; do
    case "${o}" in
        s|subdir)
            SUBDIR=${OPTARG}
            ;;
        f)
            FLAGS=${OPTARG}
            ;;
        n)
	    NO_INIT=1
	    ;;
	c)
	    CLEAN=1
	    ;;
	l)
	    LAB=${OPTARG}
	    ;;
        *)
            usage
            ;;
    esac
done
shift $((OPTIND-1))

if [ "$CLEAN" -eq 1 ]; then
  rm -rf xv6-public-*
  exit 0
fi

if [ -z "$LAB" ]; then
  usage
  exit 1
fi

DST=xv6-public-$(basename "$LAB")

if [ $NO_INIT -eq 0 ]; then
  rm -rf "$DST"
  cp -r xv6-public "$DST"
  cp -r "$LAB"/"$SUBDIR"/* "$DST"
fi

cd "$DST"
if [ -z "$FLAGS" ]; then
  make qemu-nox
else
  make qemu-nox "$FLAGS"
fi

exit 0
