#include "operator.h"

operator_type Operator::get_type() const {
	return type;
}

int Operator::get_line_number() const {
	return line_number;
}

Operator::Operator(operator_type type, std::vector<Token*>& operands, std::vector<int>& numeric_operands, int line_number)
	: type(type), operands(operands), numeric_operands(numeric_operands), line_number(line_number) {
}