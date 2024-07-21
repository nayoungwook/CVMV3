#pragma once

#include <iostream>
#include <string>
#include <locale>
#include <chrono>
#include <Windows.h>

enum class log_state {
	log_okay = 3,
	log_error = 4,
	log_success = 2,
	log_default = 7,
};

void chestnut_log(const std::wstring& content, log_state c);