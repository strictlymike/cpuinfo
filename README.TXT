CPUINFO

0.0 Contents
================================================================================
	1.0 Overview
	2.0 Build and Test
	3.0 TODO

1.0 Overview
================================================================================
This tool gathers the following CPU information from each CPU:
	* CR0 - Contents of the CR0 register via SMSW instruction
	* GDTR - Contents of the GDT Register via SGDT instruction
	* IDTR - Contents of the IDT Register via SIDT instruction
	* CPUID.0h-0Fh
	* CPUID.80000000h-80000008h

CPUINFO displays information on the console in comma-separated variable (CSV)
form.

2.0 Build and Test
================================================================================

To build 64-bit:
	1. Open a Windows 7 x64 debug build environment
	2. Change to this directory
	3. Type "nmake"

To build 32-bit (Not yet available):
	1. Write llx86.asm
	2. Follow instructions similar to above

To test:
	Run cpuinfo_x64.exe

3.0 TODO
================================================================================
	Fix SLDT (LDTR) check
	Implement 32-bit

