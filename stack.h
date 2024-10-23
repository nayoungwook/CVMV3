#pragma once

#include "operator.h"
#include "operand.h"
#include <vector>
#include <deque>

class Stack {
private:
public:
	std::deque<Operand*> operands;
	Operand* peek();
	void pop();
	void push(Operand* op);
	~Stack();
};