#pragma once

#include "operator.h"
#include "operand.h"
#include <vector>
#include <deque>

class Stack {
private:
	std::deque<Operand*> operands;
public:
	Operand* peek();
	void pop();
	void push(Operand* op);
};