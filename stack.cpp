#include "stack.h"

void Stack::pop() {
	if (this->operands.size() == 0) {
		std::cout << "Stack is empty" << std::endl;
		exit(EXIT_FAILURE);
	}

	this->operands.pop_front();
}

Stack::~Stack() {
	while (!this->operands.empty()) {
		delete this->operands.front();
		this->operands.pop_front();
	}
}

void Stack::push(Operand* op) {
	this->operands.push_front(op);
}

Operand* Stack::peek() {
	if (this->operands.size() == 0) {
		std::cout << "Stack is empty" << std::endl;
		exit(EXIT_FAILURE);
	}

	return this->operands[0];
}