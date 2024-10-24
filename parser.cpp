#include "parser.h"

Operator* get_operator(std::unordered_map<std::wstring, unsigned int>* label_id, int line_number, std::vector<Token*>& tokens) {
	Token* token = pull_token(tokens);

	Operator* result = nullptr;
	std::vector<Token*> operands;
	operator_type type = op_none;

	if (token->identifier == L"@PUSH_STRING") {
		type = op_push_string;

		operands.push_back(pull_token(tokens)); // content
	}
	else if (token->identifier == L"@PUSH_NUMBER") {
		type = op_push_number;

		operands.push_back(pull_token(tokens)); // content
	}
	else if (token->identifier == L"@PUSH_INTEGER") {
		type = op_push_integer;

		operands.push_back(pull_token(tokens)); // content
	}
	else if (token->identifier == L"@PUSH_FLOAT") {
		type = op_push_float;

		operands.push_back(pull_token(tokens)); // content
	}
	else if (token->identifier == L"@PUSH_BOOL") {
		type = op_push_bool;

		operands.push_back(pull_token(tokens)); // content
	}
	else if (token->identifier == L"@PUSH_NULL") {
		type = op_push_null;
	}
	else if (token->identifier == L"@PUSH_THIS") {
		type = op_push_this;
	}
	else if (token->identifier == L"@STORE_GLOBAL") {
		type = op_store_global;

		operands.push_back(pull_token(tokens)); // id
		pull_token(tokens);
	}
	else if (token->identifier == L"@STORE_CLASS") {
		type = op_store_class;

		operands.push_back(pull_token(tokens)); // id
		operands.push_back(pull_token(tokens)); // ( name )
	}
	else if (token->identifier == L"@STORE_ATTR") {
		type = op_store_attr;

		operands.push_back(pull_token(tokens)); // id
		operands.push_back(pull_token(tokens)); // name
	}
	else if (token->identifier == L"@STORE_LOCAL") {
		type = op_store_local;

		operands.push_back(pull_token(tokens)); // id
		operands.push_back(pull_token(tokens)); // name
	}
	else if (token->identifier == L"@LOAD_GLOBAL") {
		type = op_load_global;

		operands.push_back(pull_token(tokens)); // id
		operands.push_back(pull_token(tokens)); // name
	}
	else if (token->identifier == L"@LOAD_LOCAL") {
		type = op_load_local;

		operands.push_back(pull_token(tokens)); // id
		operands.push_back(pull_token(tokens)); // name
	}
	else if (token->identifier == L"@SUPER_CALL") {
		type = op_super_call;

		operands.push_back(pull_token(tokens)); // parameter_count
	}
	else if (token->identifier == L"@LOAD_CLASS") {
		type = op_load_class;

		operands.push_back(pull_token(tokens)); // id
		pull_token(tokens);
	}
	else if (token->identifier == L"@ADD") {
		type = op_add;
	}
	else if (token->identifier == L"@SUB") {
		type = op_sub;
	}
	else if (token->identifier == L"@MUL") {
		type = op_mul;
	}
	else if (token->identifier == L"@DIV") {
		type = op_div;
	}
	else if (token->identifier == L"@POW") {
		type = op_pow;
	}
	else if (token->identifier == L"@MOD") {
		type = op_mod;
	}
	else if (token->identifier == L"@RET") {
		type = op_ret;
	}
	else if (token->identifier == L"@OR") {
		type = op_or;
	}
	else if (token->identifier == L"@AND") {
		type = op_and;
	}
	else if (token->identifier == L"@GREATER") {
		type = op_greater;
	}
	else if (token->identifier == L"@LESSER") {
		type = op_lesser;
	}
	else if (token->identifier == L"@EQ_GREATER") {
		type = op_eq_greater;
	}
	else if (token->identifier == L"@EQ_LESSER") {
		type = op_eq_lesser;
	}
	else if (token->identifier == L"@EQUAL") {
		type = op_equal;
	}
	else if (token->identifier == L"@NOT_EQUAL") {
		type = op_not_equal;
	}
	else if (token->identifier == L"@INCRE") {
		type = op_inc;
	}
	else if (token->identifier == L"@DECRE") {
		type = op_dec;
	}
	else if (token->identifier == L"@LABEL") {
		type = op_label;

		Token* tkn = pull_token(tokens);
		std::wstring id = tkn->identifier;
		label_id->insert(std::make_pair(id, line_number));

		operands.push_back(tkn); // label
	}
	else if (token->identifier == L"@GOTO") {
		type = op_goto;

		operands.push_back(pull_token(tokens)); // label
	}
	else if (token->identifier == L"@FOR") {
		type = op_for;

		operands.push_back(pull_token(tokens)); // label
	}
	else if (token->identifier == L"@IF") {
		type = op_if;

		operands.push_back(pull_token(tokens)); // label
	}
	else if (token->identifier == L"@LOAD_ATTR") {
		type = op_load_attr;

		operands.push_back(pull_token(tokens)); // id
		operands.push_back(pull_token(tokens)); // name
	}
	else if (token->identifier == L"@ARRAY") {
		type = op_array;

		operands.push_back(pull_token(tokens)); // array size
	}
	else if (token->identifier == L"@ARRAY_GET") {
		type = op_array_get;
	}
	else if (token->identifier == L"@CAST") {
		type = op_cast;
		operands.push_back(pull_token(tokens)); // type
	}
	else if (token->identifier == L"@NEW") {
		type = op_new;

		operands.push_back(pull_token(tokens)); // id
		pull_token(tokens); // ( name )

		operands.push_back(pull_token(tokens)); // constructor parameter count
	}
	else if (token->identifier == L"@VECTOR") {
		type = op_vector;

		operands.push_back(pull_token(tokens)); // vector size
	}
	else if (token->identifier == L"@CALL_BUILTIN" || token->identifier == L"@CALL_GLOBAL" || token->identifier == L"@CALL_ATTR" || token->identifier == L"@CALL_CLASS") {
		if (token->identifier == L"@CALL_BUILTIN")
			type = op_call_builtin;
		else if (token->identifier == L"@CALL_GLOBAL")
			type = op_call_global;
		else if (token->identifier == L"@CALL_ATTR")
			type = op_call_attr;
		else if (token->identifier == L"@CALL_CLASS")
			type = op_call_class;

		operands.push_back(pull_token(tokens)); // id
		pull_token(tokens); // ( name )
		operands.push_back(pull_token(tokens)); // parameter_count
	}
	else if (token->identifier == L"@KEYBOARD") {
		operands.push_back(pull_token(tokens)); // key
		type = op_keybaord;
	}

	std::wstring line_number_str = pull_token(tokens)->identifier;

	int code_line_number = std::stoi(line_number_str);
	result = new Operator(type, operands, code_line_number);

	assert(result != nullptr);

	return result;
}

std::unordered_set<std::wstring> enabled_options;

CodeMemory* get_code_memory(CVM* vm, std::vector<Token*>& tokens) {

	Token* token = pull_token(tokens);
	if (token->identifier == L"FUNC" || token->identifier == L"$INITIALIZE" || token->identifier == L"$CONSTRUCTOR") {
		unsigned int id = std::stoi(pull_token(tokens)->identifier);
		std::wstring name = pull_token(tokens)->identifier; // ( name )
		std::wstring access_modifier = pull_token(tokens)->identifier;

		std::vector<std::wstring> params;

		std::wstring return_type = pull_token(tokens)->identifier;

		while (tokens[0]->identifier != L"{") {
			pull_token(tokens); // name
			params.push_back(pull_token(tokens)->identifier);
		}

		pull_token(tokens); // {

		std::vector<Operator*> operators;

		int i = 0;
		while (tokens[0]->identifier != L"}") {
			Operator* _operator = get_operator(vm->label_id, i, tokens);
			if (_operator != nullptr)
				operators.push_back(_operator);
			i++;
		}

		pull_token(tokens); // }

		CodeMemory* result = nullptr;

		if (token->identifier == L"$INITIALIZE")
			result = (CodeMemory*) new CMInitialize(operators, id, return_type, params, access_modifier);
		else  if (token->identifier == L"$CONSTRUCTOR")
			result = (CodeMemory*) new CMConstructor(operators, id, return_type, params, access_modifier);
		else
			result = (CodeMemory*) new CMFunction(name, operators, id, return_type, params, access_modifier);

		((CMFunction*)result)->name = name.substr(1, name.size() - 2);

		return result;
	}
	else if (token->identifier == L"CLASS" || token->identifier == L"SCENE" || token->identifier == L"OBJECT") {
		std::wstring object_type = token->identifier;
		unsigned int id = std::stoi(pull_token(tokens)->identifier);
		std::wstring name = pull_token(tokens)->identifier; // ( name )
		unsigned int parent_id = std::stoi(pull_token(tokens)->identifier);

		pull_token(tokens); // {

		std::unordered_map<unsigned int, CMFunction*>* member_functions = new std::unordered_map<unsigned int, CMFunction*>;

		CMInitialize* initializer = nullptr;
		CMConstructor* constructor = nullptr;

		unsigned int
			init_function_id = -1,
			tick_function_id = -1,
			render_function_id = -1;

		while (tokens[0]->identifier != L"}") {
			std::queue<std::pair<std::wstring, std::wstring>> empty_queue; // there's nothing going to be queued.
			CMFunction* member_function = (CMFunction*)get_code_memory(vm, tokens);

			if (member_function->get_type() == code_constructor) {
				constructor = (CMConstructor*)member_function;
			}
			else if (member_function->get_type() == code_initialize) {
				initializer = (CMInitialize*)member_function;
			}
			else {
				unsigned int id = member_function->get_id();
				std::wstring function_name = member_function->name;

				if (member_function->get_access_modifier() == L"public") {
					if (function_name == L"init")
						init_function_id = id;
					else if (function_name == L"tick")
						tick_function_id = id;
					else if (function_name == L"render")
						render_function_id = id;
				}

				member_functions->insert(std::make_pair(id, member_function));
			}
		}

		pull_token(tokens); // }

		CodeMemory* result = nullptr;

		if (object_type == L"CLASS")
			result = (CodeMemory*) new CMClass(id, parent_id,
				init_function_id, tick_function_id, render_function_id);
		else if (object_type == L"SCENE")
			result = (CodeMemory*) new CMScene(id, parent_id,
				init_function_id, tick_function_id, render_function_id);
		else if (object_type == L"OBJECT")
			result = (CodeMemory*) new CMObject(id, parent_id,
				init_function_id, tick_function_id, render_function_id);

		((CMClass*)result)->member_functions = member_functions;

		((CMClass*)result)->constructor = constructor;
		((CMClass*)result)->initializer = initializer;
		((CMClass*)result)->name = name.substr(1, name.size() - 2);

		// add render function
		if (object_type == L"OBJECT") {
			CMObject* result_object = ((CMObject*)result);
			unsigned int render_function_id = result_object->get_render_function_id();
			std::vector<Operator*> temp1;
			std::vector<std::wstring> temp2;
			CMFunction* render_function = new CMRender(temp1, render_function_id, temp2);
			if (result_object->member_functions->find(render_function_id) != result_object->member_functions->end())
				result_object->member_functions->erase(result_object->member_functions->find(render_function_id));

			result_object->member_functions->insert(std::make_pair(render_function_id, render_function));
		}

		return result;
	}
	else if (token->identifier == L"#LOAD") {
		std::wstring name = pull_token(tokens)->identifier;

		std::wstring path = L"", file_path = pull_token(tokens)->identifier;
		std::wstring current_directory = get_current_directory();
		path.assign(current_directory.begin(), current_directory.end());
		path += (L"\\" + file_path.substr(1, file_path.size() - 2));

		vm->load_queue.push(std::make_pair(name, path));

		int line_number = std::stoi(pull_token(tokens)->identifier); // line number
	}
	else if (token->identifier == L"#FONT") {
		std::wstring name = pull_token(tokens)->identifier;

		std::wstring path = L"", file_path = pull_token(tokens)->identifier;
		std::wstring current_directory = get_current_directory();
		path.assign(current_directory.begin(), current_directory.end());
		path += (L"\\" + file_path.substr(1, file_path.size() - 2));

		vm->font_queue.push(std::make_pair(name, path));

		int line_number = std::stoi(pull_token(tokens)->identifier); // line number
	}
	else if (token->identifier == L"#IMPORT") {
		std::wstring name = pull_token(tokens)->identifier;
		std::wstring file_path = name + L".cir";
		int line_number = std::stoi(pull_token(tokens)->identifier); // line number

		if (vm->imported_files.find(name) != vm->imported_files.end()) {
			return nullptr;
		}

		std::vector<std::wstring> file = get_file(file_path);
		std::vector<Token*> parsed_tokens = parse_tokens(file);

		register_parsed_file(parsed_tokens, vm);

		vm->imported_files.insert(name);
	}
	else if (token->identifier == L"#OPTION") {
		std::wstring option = pull_token(tokens)->identifier;

		enabled_options.insert(option);

		CHESTNUT_LOG(option + L" enabled.", log_level::log_warn);
	}

	return nullptr;
}

Token* pull_token(std::vector<Token*>& tokens) {
	Token* result = tokens.front();
	tokens.erase(tokens.begin());
	return result;
}

special_literal check_special_literal(const std::wstring& str, int i) {
	if (str.size() > i + 1) {
		if (str[i + 1] == 'n') {
			return special_literal::line_break;
		}
	}
	return special_literal::none;
}

std::vector<Token*> extract_tokens(std::wstring const& str) {

	std::wstring tkn = L"";
	std::vector<Token*> result;
	bool string_literal = false;

	for (int i = 0; i < str.length(); i++) {
		// wstring literal
		if (str[i] == '\"') {
			if (!string_literal) {
				string_literal = true;
			}
			else {
				string_literal = false;
				tkn += str[i];
				result.push_back(new Token(tkn));
				tkn = L"";
				i++;
				continue;
			}
		}

		// cut token
		if (isspace(str[i]) && !string_literal) {
			if (!(tkn == L" " || tkn == L"")) {
				result.push_back(new Token(tkn));
			}
			tkn = L"";
			continue;
		}

		if (string_literal && str[i] == '\\') {
			special_literal result = check_special_literal(str, i);

			if (result == special_literal::line_break)
				tkn += L"\n";

			if (result != special_literal::none) {
				i++;
				continue;
			}
		}

		tkn += str[i];

		if (!string_literal && str[i] == '\n') {
			result.push_back(new Token(tkn));
			tkn = L"";
			continue;
		}
	}

	if (tkn != L"" && !string_literal)
		result.push_back(new Token(tkn));

	/*
	for (int i = 0; i < result.size(); i++) {
		std::cout << result[i]->identifier << std::endl;
	}
	*/

	return result;
}