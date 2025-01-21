@echo off
setlocal

set PRJ_NAME=cpuid
set PRJ_COMPILER=zig cc

set ROOT=
for %%i in ( %~dp0.. ) do set "ROOT=%%~fi"

set PRJ_INCLUDE=%ROOT%\include
set PRJ_SRC=%ROOT%\src
set PRJ_OUT=%ROOT%\out

if not exist "%PRJ_OUT%" ( mkdir "%PRJ_OUT%" )
pushd "%PRJ_SRC%"
	fasm -d PLATFORM=WIN64 entry-x86_64.fasm
	
	if ["%PRJ_COMPILER%"] == ["zig cc"] goto :compile_zigcc
	goto :compile_end

	:compile_zigcc
	set TEMP=
	for /F "tokens=* delims=" %%i in ('where zig') do set "TEMP=%%~dpi"
	set TEMP="%TEMP%\lib\libc\include\any-windows-any"
	echo %TEMP%
	zig cc main.c entry-x86_64.obj -nostdlib -D_ZIG ^
		-Wall -Wno-unused-function -Ofast -g ^
		-target x86_64-windows-gnu /SUBSYSTEM:CONSOLE /ENTRY:_start -e "_start" ^
		-lkernel32 ^
		-I "%PRJ_INCLUDE%" -isystem %TEMP%  -Xclang -sys-header-deps ^
		-o "%PRJ_OUT%\%PRJ_NAME%.exe"
:compile_end
popd

endlocal
