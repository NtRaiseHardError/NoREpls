#pragma once
#ifndef __ANTI_H__
#define __ANTI_H__

#include <Windows.h>

#define DIE ::ExitProcess(0)
// anti v2.2
#define DIE_ASM __asm push 0 __asm call ExitProcess
//anti 2.3
//#define DIE_HARD_ASM __asm _emit 0xE9

// anti v2.2
#define ANTIDIS_LINEAR __asm {					\
					__asm	xor		eax, eax	\
					__asm	_emit	0x75		\
					__asm	_emit	0x07		\
					__asm	_emit	0x74		\
					__asm	_emit	0x06		\
					__asm	jmp		$ + 10		\
					__asm	_emit	0x6A		\
					}

#define CheckFuncInt3(x) __asm {									\
						__asm	pushad								\
						__asm	mov			eax,			x		\
						__asm	mov			cl,				0xCC	\
						__asm	cmp			BYTE PTR [eax],	cl		\
						__asm	_emit		0x75					\
						__asm	_emit 		0x01					\
						DIE_ASM										\
						__asm	popad								\
						}

#ifndef _DEBUG
#define ISDBGPRSNT __asm {								\
				__asm	pushad							\
				__asm	mov		eax,	fs:[0x30]		\
				__asm	movzx	eax,	[eax + 0x2]		\
				__asm	_emit	0x74					\
				__asm	_emit	0x01					\
				DIE_ASM									\
				__asm	popad							\
				}
#else
#define ISDBGPRSNT __asm nop
#endif // !_DEBUG

#define DSTRYFUNCBOUND __asm {					\
					__asm	call	$ + 5		\
					__asm	pop		eax			\
					__asm	add		eax,	6	\
					__asm	_emit	0x50		\
					__asm	ret					\
					}

extern LPWSTR lpDbgWindow;

BOOL CALLBACK EnumResNameProc(HMODULE hModule, LPCWSTR lpszType, LPCWSTR lpszName, LONG_PTR lParam);
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
void CheckDebuggerWindows(void);

__forceinline bool StringTableCheck() {
	::EnumResourceNames(NULL, MAKEINTRESOURCE(RT_STRING), (ENUMRESNAMEPROC)EnumResNameProc, 0);
	return true;
}

// anti
struct _anti {
	_anti() {
#ifndef _DEBUG
		// IsDebuggerPresent v2.1
		ANTIDIS_LINEAR
		ISDBGPRSNT
#endif // !_DEBUG
	}
};

#endif // !__ANTI_H__
