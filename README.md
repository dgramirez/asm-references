# ASM-References

**THIS IS NOT A LIBRARY NOR A FRAMEWORK!**  
**THIS IS MEANT TO BE REFERENCED FOR YOUR OWN PROJECTS!**  
**LICENSE: [THE UNLICENSE](https://unlicense.org)**

<p>This repository is designed to show how to integrate Assembly into a C program. This is useful when you want (or must) divorce yourself from the C Standard Library (libc, glibc, etc) and/or C Runtime Library (Microsoft CRT library) but want to be closer to your user's hardware.</p>

<p>CPUID is the first example, being able to grab CPU features (SSE/AVX, AES/SHA, etc.), CPU Cache Sizes (L1, L2 & L3), and whatever more the CPU manufacturers put in their CPUID call. Knowing the CPU's capabilities allow developers to create more informed decisions on how to structure their codebase with no dependency.</p>

## Requirements
### FASM (Both Linux and Windows)
<p>One major requirement is an assembler for the assembly code. The chosen assembler is <a href="https://flatassembler.net/">Flat Assembler</a>. For Linux, check to see if your package manager have fasm and you can install it that way. For Windows and Linux without it in their package manager:</p>
<ul>
	<li><a href="https://flatassembler.net/download.php">Download FASM</a> for your operating system</li>
	<li>Extract the folder and move it to a location accessable to you.</li>
	<ul>
		<li>Windows: I normally use `C:\Program Files\fasm\<version> (Admin User) or `C:\Users\<My Username>\AppData\Local\Programs\fasm\<version>` (Standard User).</li>
		<li>Linux: I normally use `/usr/local/fasm/<version>` (Admin User) or `~/.local/programs/fasm/<version>` (Standard User)</li>
	</ul>
	<li>Add the fasm filepath to your PATH environment variable.</li>
	<li>Test the results typing `fasm` into your terminal (Linux) / Command Prompt (Windows) outside of the extracted directory. You know it works if you see `flat assembler version x.xx.xx` prompt appears.</li>
</ul>

### Zig (zig cc) [Optional for Linux]
<p>Another major requirement is a compiler. For Windows users, MSVC is untested as of this commit, so it may fail if you try to use /NODEFAULTLIB due to not having memcpy and memset. For Linux users, GCC may follow the same fate as MSVC, however if you use Clang it should work. To accomodate this, I ensure that Zig's C compiler works with both Windows and Linux.</p>
<ul>
	<li><a href="https://ziglang.org/download/">Download Zig</a> for your operating system</li>
	<li>Extract the folder and move it to a location accessable to you.</li>
	<ul>
		<li>Windows: I normally use `C:\Program Files\zig\<version> (Admin User) or `C:\Users\<My Username>\AppData\Local\Programs\zig\<version>` (Standard User).</li>
		<li>Linux: I normally use `/usr/local/zig/<version>` (Admin User) or `~/.local/programs/zig/<version>` (Standard User)</li>
	</ul>
	<li>Add the Zig filepath to your PATH environment variable.</li>
	<li>Test the results typing `zig version` into your terminal (Linux) / Command Prompt (Windows) outside of the extracted directory. You know it works if you see `xx.xx.xx` prompt appears.</li>
</ul>

## Compilation
<p>Inside the `/misc` directory, there is a build file that can be ran for Windows (build.bat) and Linux (build.sh).</p>

### Build Files & Compiler Support  
<p>When using a build file, the binaries will be placed in `/out` directory.</p>

##### build.sh (Linux)
<p>The bash file uses the `CC` environment variable to call the compiler. If `CC` is not set, it will attempt to use the cc command, as many Linux distribution's set cc to GCC or Clang. If this is still a problem, you can either modify the build.sh file itself, or you can export cc like this:</p>
<ul>
	<li>export CC=<compiler></li>
</ul>

<p>Examples (Along with supported compilers):</p>
<ul>
	<li>export CC="zig cc"</li>
	<li>export CC=clang</li>
	<li>export CC=cproc</li>
</ul>

<p>Due to not having a memcpy or memset defined, These compilers may fail:</p>
<ul>
	<li>export CC=gcc</li>
	<li>export CC=tcc (Tested, It does fail)</li>
</ul>

#### build.bat (Windows)
<p>As of now, you must have Zig setup in your environment in order to build. If you do have it, it is as simple as double clicking on the batch file to compile it. Since Zig isn't really a stable compiler itself, the latest Zig version used is 0.13.0.</p>

<p>MSVC will definitely be supported later on after the Winter is over (I want my 50 degrees back!)</p>

## The Assembly Reference Notes
<p>To start, These notes are not meant for beginners in the software dev space. You should know how to navigate at least one language effectively, preferrably C as that is what is used here. Another thing is these are just notes that I picked up while doing this small project, as I'm on a journey to have my own reference standard library.</p>

<p>Another thing, is if this is TL (for TL;DR): I made notes with focus on Assembly, linking documentation, providing how assembly is put together with c (using cpuid as the example), pitfalls that may occur, and the compilation process. Want to know more? Go Read.</p>

<p>I normally do single-file compilation. This means I only put one source file to compile the whole program. From there, I do my best to organize the code into its own section.</p>
<ul>
	<li>Combined Source Code : `/src`</li>
	<li>ASM Code: `/src/asm`</li>
	<li>ASM Headers for C: `/include/asmref`</li>
	<li>CPUID Header Files: /include/cpuid</li>
</ul>

#### Prereq: Finding the Documentation
<p>When dealing with Assembly, There is quite a bit of documentation you should have on hand:</p>
<ul>
	<li>The Assembler Documentation: <a href="https://flatassembler.net/docs.php">[FASM Documentation]</a></li>
	<li>x86_64 ABI: <a href="https://gitlab.com/x86-psABIs/x86-64-ABI">[Here]</a></li>
	<li>[CPU Specifics]</li>
	<ul>
		<li>Intel Software Developer's Manual: <a href="https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html">[Here]</a></li>
		<li>AMD Programmer's Manual: <a href="https://www.amd.com/en/search/documentation/hub.html#sortCriteria=%40amd_release_date%20descending&f-amd_document_type=Programmer%20References&f-amd_document_location=AMD.com&f-amd_archive_status=Active">[Here]</a></li>
		<li>ARM Documentation Center: <a href="https://developer.arm.com/documentation">[Here]</a></li>
	</ul>
	<li>[Operating System Specifics]: </li>
	<ul>
		<li>Linux Syscall Table for x86_64: <a href="https://github.com/torvalds/linux/blob/master/arch/x86/entry/syscalls/syscall_64.tbl">[Here]</a></li>
		<li>Calling Conventions for C/C++: <a href="https://en.wikipedia.org/wiki/X86_calling_conventions#x86-64_calling_conventions">[Here]</a></li>
	</ul>
</ul>

#### First: Compiling the FASM Code
`fasm entry-x86_64.fasm`  

<p>Compiling FASM is the easy part. The hard part is writing ASM that will work for both Windows and Linux. A few things to keep in mind is what argument is on what register, what registers is caller-safe vs callee-safe, and where the additional arguments are located. Sometimes its easier to have a separate codepath for Windows and Linux, and that was done using FASM's symbolic variable for cpuid.</p>

<p>Without libc, We are now responsible on executing the exit syscall for Linux (Thankfully, Windows have ExitProcess() in kernel32.lib). This is very easy to figure out if you use the syscall table and call that syscall. This is where `/src/asm/syscall-linux-x86_64.fasm` comes in. **One major note about syscall in general [not for exit], is that it uses r10 as argument 3 instead of rcx!** Another thing to think about is entry point. On Windows, we don't necessarily need to think about this, but on Linux we do if we want to pass arguments into our C program. Inside `/src/entry-x86_64.fasm`, we have a _start label for Linux (not for Windows), which calls main, then calls SYS_EXIT (in its syscall code). That should handle the entrypoint.</p>

<p>Now, how to pass arguments into the C program? <a href="https://nullprogram.com/blog/2023/03/23/">Thanks to Chris Wellons</a> from <a href="https://nullprogram.com">Null Program</a>, He shows how to pass those standard arguments for a main program. To get arguments in Windows, its as easy as calling `GetCommandLineA` and `CommandLineToArgvW` from shell32.lib</p>

<p>The final thing is to write the remaining fasm code. In this project, I've written CPUID for both Windows and Linux. Despite being similar with instructions, the registers is what tripped me up.</p>
<p>For Windows, the calling convention for arguments is:</p>
<ol>
	<li>rcx</li>
	<li>rdx</li>
	<li>r8</li>
	<li>r9</li>
</ol>
<p>For Linux, the calling convention for arguments is:</p>
<ol>
	<li>rdi</li>
	<li>rsi</li>
	<li>rcx</li>
	<li>rdx</li>
</ol>
<p>Now, For CPUID Instruction, we need the registers:</p>
<ol>
	<li>eax (rax)</li>
	<li>ebx (rbx)</li>
	<li>ecx (rcx)</li>
	<li>edx (rdx)</li>
</ol>
<p>So the most common registers required is rcx and rdx, required by all 3, with Windows being argument 0/1, CPUID in argument 2/3 and System V in argument 3/2. I'm sure if I think more (as I was when typing this), it is possible to unify this with platform-specific symbolic variables. Anyways, this is why inside `/src/asm/cpuid-x86_64.fasm` have two different codepaths for Windows and Linux. One last note on this: Since ebx is required for cpuid and both Windows and System V has it as non-volatile (callee-saved), you will have to save ebx (I pushed it) and then load the previous value back (I pop'd it). I encountered a release mode bug in where ebx is accessed and I ended up crashing. Compiling in debug for Clang & Zig did not crash and ran the program as expected. Always check the calling conventions before using a register you're not familiar with!</p>

<p>This should be everything on the fasm side of things, next up is setting up and compiling the C side of things and linking it up with the fasm.</p>

#### Second: Compiling C Code
`cc main.c entry-x86_64.o`  
`zig cc main.c entry-x86_64.obj`  

<p>After compiling the FASM code, we now have to get our c source code linked up. We will need the declarations for the labels written in fasm. Here is where I can answer the elephant in the room: Why FASM, why not NASM? I initially tried NASM but I couldn't get the labels to be recognzied when compiling for Windows. I'm sure it has to be possible, but it means digging deeper and FASM worked out of the box.</p>

<p>First is entry point! I used main as a convention. Since I got argc, argv and envp, I can do a normal main function. I don't need to worry about the </p>

<p>With Windows, we don't really need to worry about "syscalls", as Kernel32.lib normally take care of everything we would normally need (ExitProcess, VirtualAlloc/VirtualFree, CreateFile/ReadFile/WriteFile,CloseFile.) MSDN is your best friend.</p>

<p>However, its not the same for Linux. You must use the syscall instruction (interrupt 0x40 for x86/i386) to use the kernel's system calls. Of course, we did this already in the fasm part, so to link to those labels, the file `/include/asmref/syscall-linux-x86_64.h` has the declarations for all the syscalls we need. We can now use exit, mmap/munmap, and write. If we need to open/read/write/close file, it is as easy as adding our definition to our asm, then adding our declaration to the header.</p>

<p>Why not just make a syscall function or macro and use that? Preference. I rather see "sys_mmap(a0, a1, a2, a3, a4, a5)" instead of "syscall(a0, a1, a2, a3, a4, a5, SYS_MMAP), or even macro'd as syscall(SYS_MMAP, a0, a1, a2, a3, a4, a5). Its all preference, both are ok as long as the arguments are aligned and you minimize your mov or pushq/popq to one register (rcx -> r10).</p>

<p>From there, just write C code. Thats what I did for the first test: Printing out the argument count, arguments and environment variables. Easy Peasy. Now for CPUID.</p>

<p>for CPUID, you will absolutely need the documentation for the processor you use. I have this working for x86_64, not ARM (as of yet). Its going to be your standard "how am I going to architect this" and then going from the ground up. The only asm part of this is the actual cpuid call, besides that its structs, unions, and cpuid calls. **Of course, you do need to validate that the CPU being used even have the cpuid instruction prior to calling it**, which that has been done in fasm (has_cpuid). Besides that, its all C stuff.</p>

<p>Hopefully, Someone is able to get a grasp on this, it is quite a bit to comprehend and a lot of reading documentation. Once you get semi-familiar with it then its just using documentation where needed.</p>

#### Third: Compiler Options - Optimization or Debug w/ No Standard Library
Linux-Debug: `cc main.c entry-x86_64.o -O0 -g -nostdlib`  
Linux-Release: `cc main.c entry-x86_64.o -Ofast -nostdlib`  
Windows-Debug: `zig cc main.c entry-x86_64.obj -O0 -g -nostdlib -D_ZIG %ZIG_DIR%\lib\libc\include\any-windows-any /SUBSYSTEM:CONSOLE -e "_start" -lkernel32`  
Windows-Release: `zig cc main.c entry-x86_64.obj -Ofast -nostdlib -D_ZIG -isystem %ZIG_DIR%\lib\libc\include\any-windows-any /SUBSYSTEM:CONSOLE -e "_start" -lkernel32`  

<p>Linux is straight forward:</p>
<ul>
	<li>-O[x]: Uses the requested optimize flag</li>
	<li>-g: Retains source-level debugging information [Normally DWARF]</li>
	<li>-nostdlib: strip away the standard library's start files and libraries.</li>
</ul>

<p>Windows on the other hand, was a bit more complicated. Lets go with the more straight-forward stuff first:</p>
<ul>
	<li>-O[x]: Uses the requested optimize flag</li>
	<li>-g: Retains source-level debugging information [Using PDBs]</li>
	<li>-nostdlib: strip away the standard library's start files and libraries [And System Includes!].</li>
	<li>/SUBSYSTEM:CONSOLE[:] This sets the subsystem for windows.</li>
	<li>-e "_start": This is the entry point. Unlike Linux, I use _start here.[*]</li>
	<li>-lkernel32: This links the Kernel32.lib to the project (Needed for ExitProcess)</li>
</ul>

<p>Now the complicated part. I omitted -isystem part in the above because there is a big problem. -nostdlib, for some reason, removes the system include path for the windows headers (Unlike /NODEFAULTLIB, which retains them.) This means that we will have to re-attach the system includes. This also presents another problem: How can we reliably get the correct headers on different windows machines? Inside build.bat, I have this snippet:</p>

<code>set TEMP=
for /F "tokens=* delims=" %%i in ('where zig') do set "TEMP=%%~dpi"
set TEMP="%TEMP%\lib\libc\include\any-windows-any"</code>

<p>`where zig` is the key command here, getting the path of the executable. With batch tricks, we can just get the directory path for our zig compiler. We can now append where the windows header files are located. Finally, we can append that value to the -isystem option to include that path into the system include path and have my windows header back.</p>

#### Conclusion
<p>Overall, putting all of this together allows me to make this simplified CPUID printer that can be compiled on Windows and Linux, using only what the OS provides. Although this is meant for me to keep on record, I do hope it helps someone who wants to make this journey. I will also reference Chris Wellon's <a href="https://nullprogram.com/blog/2023/03/23/">Practical libc-free threading on Linux</a> blog post to continue this adventure, this time focusing on multi-threading for linux. Thankfully, Windows does make this easier also, with invoking `CreateThread, *CriticalSection, *SRWLock (Vista+), and others within Kernel32.lib`</p>
