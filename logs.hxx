#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <string>
#include <Psapi.h>
#include <fstream>
#include <encryption/sk.hxx>

namespace logs
{
	extern std::ofstream logs;

	void init_log(LPSTR logname);
	void log(LPSTR message, LPSTR branding, bool cout);
}