#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

class Token {
private:
public:
	Token(std::string identifier) : identifier(identifier) {}
	std::string identifier;
};

Token* pull_token(std::vector<Token*>& tokens);
std::vector<Token*> extract_tokens(std::string const& str);
