#include "sys_file.h"

std::string get_absolute_path(std::string const& path) {
	std::string current_path;
	std::wstring wcurrent_dir = get_current_directory();
	current_path.assign(wcurrent_dir.begin(), wcurrent_dir.end());

	return current_path + "\\" + path;
}

std::vector<Token*> parse_tokens(std::vector<std::string> file) {
	std::vector<Token*> parsed_tokens;
	for (std::string _f : file) {
		std::vector<Token*> tkns = extract_tokens(_f);

		for (Token* tkn : tkns) {
			parsed_tokens.push_back(tkn);
		}
	}
	
	return parsed_tokens;
}

std::vector<std::string> get_file(std::string& file_path) {

	file_path = get_absolute_path(file_path);

	std::ifstream openFile(file_path.data());
	std::vector<std::string> result;

	if (openFile.is_open()) {
		std::string line;
		while (getline(openFile, line)) {
			result.push_back(line);
		}

		openFile.close();
	}
	else {
		std::string file_name = file_path;
		CHESTNUT_THROW_ERROR(L"Failed to load file " + std::wstring(file_name.begin(), file_name.end()) + L" Please check file name again.",
			"RUNTIME_FAILED_TO_LOAD_FILE", "0x09", -1);
	}

	CHESTNUT_LOG(L"new file : " + std::wstring(file_path.begin(), file_path.end()) + L" loaded.", log_level::log_okay);

	return result;
}

void write_file(std::string& file_path, std::string content) {
	std::vector<std::string> result;
	std::ofstream writeFile(file_path.data());

	if (writeFile.is_open()) {
		writeFile << content;
		writeFile.close();
	}
}

std::wstring get_current_directory() {
	TCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
	return std::wstring(buffer).substr(0, pos);
}