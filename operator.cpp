#include "operator.h"

operator_type Operator::get_type() const {
	return type;
}

std::vector<Token*> Operator::get_operands() const {
	return operands;
}

int Operator::get_line_number() const {
	return line_number;
}

Operator::Operator(operator_type type, std::vector<Token*>& operands, int line_number)
	: type(type), operands(operands), line_number(line_number) {
}