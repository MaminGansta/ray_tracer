#pragma once

#define safe_release(ptr) (delete ptr, ptr = nullptr)
#define safe_releaseArr(ptr) (delete[] ptr, ptr = nullptr)

#include <Windows.h>
#include <stdio.h>
#include <cstdint>
#include <cstdio>

#include <vector>
#include <algorithm>


void doutput(const char* format, ...)
{
	char log[128];
	va_list args;
	va_start(args, format);
	vsprintf_s(log, format, args);
	OutputDebugStringA(log);
	va_end(args);
}

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


#define STB_IMAGE_IMPLEMENTATION
#define STBI_WINDOWS_UTF8
#include "stb_image.h"

// globals
HINSTANCE hInst;

#ifndef MAX_THREADS
#define MAX_THREADS 8
#endif

// unity build
#include "thread_pool.cpp"
thread_pool workers(MAX_THREADS);

// gui laoyt
#include "canvas.cpp"
#include "window.cpp"
#include "image.cpp"
#include "draw.cpp"
#include "input.cpp"
#include "timer.cpp"
#include "time.cpp"



void al_init(HINSTANCE hInstance)
{
	hInst = hInstance;
	init_time = high_resolution_clock::now();
}
