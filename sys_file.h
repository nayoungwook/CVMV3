#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <ostream>
#include <ctime>
#include <Windows.h>
#include <chrono>
#include "token.h"
#include "error/sys_error.h"

std::vector<std::wstring> get_file(std::wstring& file_path);
void write_file(std::wstring& file_path, std::wstring content);
std::wstring get_current_directory();
std::wstring get_absolute_path(std::wstring const& path);
std::vector<Token*> parse_tokens(std::vector<std::wstring> file);