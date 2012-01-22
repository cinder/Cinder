; call cpuid with args in eax, ecx
; store eax, ebx, ecx, edx to p

PUBLIC cpuid64

.CODE

ALIGN     8

cpuid64 PROC FRAME

; void cpuid64(CPUID_ARGS* p);

; rcx <= p

sub rsp, 32

.allocstack 32

push    rbx

.pushreg    rbx

.endprolog

mov r8, rcx

mov eax, DWORD PTR [r8+0]

mov ecx, DWORD PTR [r8+8]

cpuid

mov DWORD PTR [r8+0], eax

mov DWORD PTR [r8+4], ebx

mov DWORD PTR [r8+8], ecx

mov DWORD PTR [r8+12], edx

pop      rbx

add      rsp, 32

ret

ALIGN     8

cpuid64 ENDP

;_TEXT ENDS

END