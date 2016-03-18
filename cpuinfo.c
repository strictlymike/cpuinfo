#include <windows.h>
#include <intrin.h>
#include <stdio.h>

#include "ll.h"

#define INVALID_OR_NULL(h)		((NULL == h) || (INVALID_HANDLE_VALUE == h))
#define TIMEOUT_MSEC			(10*1000)

#define DISPLAY(c, v, l, o)		Display(c, v, l, (void*)o, sizeof(o))

#define VMX_BIT					((0x1) << 5)
#define VMX_BIT_INV				(~VMX_BIT)

#define CPUID_REG_EAX			0
#define CPUID_REG_EBX			1
#define CPUID_REG_ECX			2
#define CPUID_REG_EDX			3

int GetNumCpus();
DWORD_PTR CpuMask(int n);
DWORD WINAPI ThreadStart(LPVOID param);
void Dump(int cpu);
void Display(int cpu, int vmx, char *label, void *base, size_t len);
void DisplayHex(unsigned char *base, size_t len);

CRITICAL_SECTION UsingOutputDevice;

int
main(void)
{
	BOOL Ok;
	int i, n;
	HANDLE *Threads = NULL;
	int ret = 1;
	DWORD_PTR ResultantMask;
	DWORD Wait;

	n = GetNumCpus();

	Threads = malloc(sizeof(HANDLE) * n);
	if (Threads == NULL) {
		fprintf(stderr, "malloc failed\n");
		goto exit_main;
	}

	memset(Threads, 0, sizeof(HANDLE) * n);

	InitializeCriticalSection(&UsingOutputDevice);

	for (i=0; i<n; i++) {
		Threads[i] = CreateThread(NULL, 0, ThreadStart, (LPVOID)i, CREATE_SUSPENDED, NULL);
		/* If thread creation fails... */
		if (INVALID_OR_NULL(Threads[i])) {
			fprintf(
				stderr,
				"CreateThread[%d] failed, GLE=%d\n",
				i,
				GetLastError()
			   );

			goto exit_main;
		}

		ResultantMask = SetThreadAffinityMask(Threads[i], CpuMask(i));
		if (0 == ResultantMask) {
			fprintf(
				stderr,
				"SetThreadAffinityMask[%d] failed, GLE=%d\n",
				i,
				GetLastError()
			   );
			goto exit_main;
		}
	}

	printf("\"CPU\", \"VMX\", \"Datum\", \"Value\"\n");

	for (i=0; i<n; i++) {
		if (-1 == ResumeThread(Threads[i])) {
			fprintf(
				stderr,
				"ResumeThread[%d] failed, GLE=%d\n",
				i,
				GetLastError()
			   );
			goto exit_main;
		}
	}

	Wait = WaitForMultipleObjects(n, Threads, TRUE, TIMEOUT_MSEC);
	if ((Wait < WAIT_OBJECT_0) || (Wait >= (WAIT_OBJECT_0+n))) {
		fprintf(
			stderr,
			"WaitForMultipleObjects failed, Wait=%d, GLE=%d\n",
			Wait,
			GetLastError()
		   );
		goto exit_main;
	}

	ret = 0;

exit_main:
	/* Terminate any outstanding threads */
	for (i=0; i<n; i++) {
		if (Threads[i]) {
			TerminateThread(Threads[i], -1);
		}
	}

	if (NULL != Threads) { free(Threads); }

	return ret;
}

DWORD_PTR
CpuMask(int n)
{
	return (DWORD_PTR) (0x1 << n);
}

int
GetNumCpus()
{
	SYSTEM_INFO si;

	GetSystemInfo(&si);

	return si.dwNumberOfProcessors;
}

DWORD
WINAPI
ThreadStart(LPVOID param)
{
	Dump((int)param);
	return 0;
}

void
Dump(int cpu)
{
	/* TIL: Basic Execution Environment: Descriptor table registers - The
	 * global descriptor table register (GDTR) and interrupt descriptor table
	 * register (IDTR) expand to 10 bytes so that they can hold a full 64-bit
	 * base address. */
	BYTE idt[IDTR_LEN];
	BYTE gdt[GDTR_LEN];
	BYTE cr0[CR0_LEN];
	// BYTE ldt[16]; /* LDT stuff is a work in progress */
	int regs[4];
	int vmx;
	int i;
	char lbl[32];

	memset(idt, 0, sizeof(idt));
	memset(gdt, 0, sizeof(gdt));
	// memset(ldt, 0, sizeof(ldt));
	memset(cr0, 0, sizeof(cr0));

	__cpuid(regs, 1);

	vmx = regs[CPUID_REG_ECX] & VMX_BIT;

	__sidt((void *)&idt);
	ll_sgdt((void *)&gdt); /* _sgdt does not compile with cl 16.00.30319.01 */
	ll_smsw((void *)&cr0);
	// ll_sldt((void *)&ldt);

	EnterCriticalSection(&UsingOutputDevice);
	DISPLAY(cpu, vmx, "IDT", idt);
	DISPLAY(cpu, vmx, "GDT", gdt);
	DISPLAY(cpu, vmx, "CR0", cr0);
	// DISPLAY(cpu, vmx, "LDT", ldt);

	for (i=0; i<=0xf; i++)
	{
		__cpuid(regs, i);

		_snprintf(lbl, sizeof(lbl), "cpuid.%02xh.eax", i);
		DISPLAY(cpu, vmx, lbl, &regs[CPUID_REG_EAX]);

		_snprintf(lbl, sizeof(lbl), "cpuid.%02xh.ebx", i);
		DISPLAY(cpu, vmx, lbl, &regs[CPUID_REG_EBX]);

		_snprintf(lbl, sizeof(lbl), "cpuid.%02xh.ecx", i);
		DISPLAY(cpu, vmx, lbl, &regs[CPUID_REG_ECX]);

		_snprintf(lbl, sizeof(lbl), "cpuid.%02xh.edx", i);
		DISPLAY(cpu, vmx, lbl, &regs[CPUID_REG_EDX]);
	}

	for (i=0x80000000; i<=0x80000008; i++)
	{
		__cpuid(regs, i);

		_snprintf(lbl, sizeof(lbl), "cpuid.%02xh.eax", i);
		DISPLAY(cpu, vmx, lbl, &regs[CPUID_REG_EAX]);

		_snprintf(lbl, sizeof(lbl), "cpuid.%02xh.ebx", i);
		DISPLAY(cpu, vmx, lbl, &regs[CPUID_REG_EBX]);

		_snprintf(lbl, sizeof(lbl), "cpuid.%02xh.ecx", i);
		DISPLAY(cpu, vmx, lbl, &regs[CPUID_REG_ECX]);

		_snprintf(lbl, sizeof(lbl), "cpuid.%02xh.edx", i);
		DISPLAY(cpu, vmx, lbl, &regs[CPUID_REG_EDX]);
	}

	LeaveCriticalSection(&UsingOutputDevice);
}

void
Display(int cpu, int vmx, char *label, void *base, size_t len)
{
	char *fmt = NULL;

	printf("\"%d\", \"%s\", \"%s\", ", cpu, vmx? "Yes": "No", label);
	DisplayHex(base, len);
	printf("\n");
}

void
DisplayHex(unsigned char *base, size_t len)
{
	printf("\"0x");
	while (len--)
	{
		printf("%02x", *(base+len));
	}
	printf("\"");
}
