#include "sys_util.h"

std::wstring to_upper_all(const std::wstring& str) {
	std::wstring result;

	for (int i = 0; i < str.size(); i++) {
		result += std::toupper(str[i]);
	}

	return result;
}