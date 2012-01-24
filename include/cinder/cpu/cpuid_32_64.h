#pragma once 

typedef struct cpuid_args_s {
DWORD eax;
DWORD ebx;
DWORD ecx;
DWORD edx;
} CPUID_ARGS; 

extern "C" { 
#ifdef _WIN64 // For 64-bit apps 
	void cpuid64(CPUID_ARGS* p);
#define _CPUID cpuid64
#endif
}