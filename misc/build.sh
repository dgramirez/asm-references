#!/bin/bash
PROJECT_NAME=mr_asm

PROJECT_ROOT="$(dirname $(dirname $(realpath $0))..)"
PROJECT_INCLUDE="$PROJECT_ROOT/include"
PROJECT_SRC="$PROJECT_ROOT/src"
PROJECT_OUT="$PROJECT_ROOT/out"

if [[ "$CC" == "" ]]; then
	echo CC Environment Variable not set.
	echo defaulting to cc

	CC="cc"
fi

mkdir -p "$PROJECT_OUT"
cd "$PROJECT_SRC"
fasm entry-x86_64.fasm -d PLATFORM=LINUX
${CC} main.c entry-x86_64.o -nostdlib    \
	-Wall -Wno-unused-function -Ofast -g \
	-I "$PROJECT_INCLUDE"                \
	-o "$PROJECT_OUT/$PROJECT_NAME"

rm -rf *.o

