#include "sys_file.h"

std::wstring get_absolute_path(std::wstring const& path) {
	std::wstring current_path;
	std::wstring wcurrent_dir = get_current_directory();
	current_path.assign(wcurrent_dir.begin(), wcurrent_dir.end());

	return current_path + L"\\" + path;
}

std::vector<Token*> parse_tokens(std::vector<std::wstring> file) {
	std::vector<Token*> parsed_tokens;
	for (std::wstring _f : file) {
		std::vector<Token*> tkns = extract_tokens(_f);

		for (Token* tkn : tkns) {
			parsed_tokens.push_back(tkn);
		}
	}

	return parsed_tokens;
}

std::vector<std::wstring> get_file(std::wstring& file_path) {

	file_path = get_absolute_path(file_path);

	std::wifstream openFile(file_path.data());
	std::vector<std::wstring> result;

	if (openFile.is_open()) {
		std::wstring line;
		while (getline(openFile, line)) {
			result.push_back(line);
		}

		openFile.close();
	}
	else {
		std::wstring file_name = file_path;
		CHESTNUT_THROW_ERROR(L"Failed to load file " + std::wstring(file_name.begin(), file_name.end()) + L" Please check file name again.",
			"RUNTIME_FAILED_TO_LOAD_FILE", "0x09", -1);
	}

	CHESTNUT_LOG(L"new file : " + std::wstring(file_path.begin(), file_path.end()) + L" loaded.", log_level::log_okay);

	return result;
}

void write_file(std::wstring& file_path, std::wstring content) {
	std::vector<std::wstring> result;
	std::wofstream writeFile(file_path.data());

	if (writeFile.is_open()) {
		writeFile << content;
		writeFile.close();
	}
}

std::wstring get_current_directory() {
	TCHAR buffer[MAX_PATH] = { 0 };
	GetCurrentDirectory(MAX_PATH, buffer);
	return std::wstring(buffer);
}