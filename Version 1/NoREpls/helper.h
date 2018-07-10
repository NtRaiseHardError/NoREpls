#pragma once
#ifndef __HELPER_H__
#define __HELPER_H__

#include <iostream>

#include "anti.h"
#include "resource.h"
#include "str_obfuscator.hpp"

#define STROBF(x) ((cryptor::create(x).decrypt()))

// global strings declaration

BOOL FileExists(LPCWSTR szPath);
void MyWideCharToMultiByte(LPSTR& lpOutput, LPWSTR lpwInput, SIZE_T nInLen);
void MyMultiByteToWideChar(LPWSTR& lpwOutput, LPSTR lpInput, SIZE_T nInLen);
void MyLoadString(UINT uID, LPWSTR& lpBuffer);
void MyUnloadString(LPWSTR& lpBuffer);

struct _init {
	_init() {
		// check integrity of string tables v2.1
		StringTableCheck();
		// check debugging windows v2.2
		CheckDebuggerWindows();
	}
};

#endif // !__HELPER_H__
