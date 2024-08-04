#include "sys_util.h"

std::string to_upper_all(const std::string& str) {
	std::string result;

	for (int i = 0; i < str.size(); i++) {
		result += std::toupper(str[i]);
	}

	return result;
}