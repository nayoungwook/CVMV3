#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

class Token {
private:
public:
	Token(std::wstring identifier) : identifier(identifier) {}
	std::wstring identifier;
};

Token* pull_token(std::vector<Token*>& tokens);
std::vector<Token*> extract_tokens(std::wstring const& str);
