#include "sys_logger.h"

void CHESTNUT_LOG(const std::wstring& content, log_level c) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	const auto now = std::chrono::system_clock::now();
	const std::time_t t_c = std::chrono::system_clock::to_time_t(now);

	SetConsoleTextAttribute(hConsole, (int)c);
	char* time_str;
	char t[256];
	ctime_s(t, sizeof(t), &t_c);
	strtok_s(t, "\n", &time_str);
	std::cout << "[" << time_str << "] | CHESTNUT : ";
	std::wcout << content << '\n';
	SetConsoleTextAttribute(hConsole, 07);
}