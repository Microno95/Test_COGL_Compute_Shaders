// Test.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <cogl/cogl.h>
#include <exception>

#include <cstdio>  /* defines FILENAME_MAX */

#include <direct.h>
#define GetCurrentDir _getcwd

std::string GetCurrentWorkingDir() {
	char cCurrentPath[FILENAME_MAX];

	if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
	{
		throw std::runtime_error("Current Dir makes no sense");
	}

	cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */

	return std::string(cCurrentPath);
}

// TODO: Reference additional headers your program requires here.
