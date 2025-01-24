#include "function_frame.h"

void FunctionFrame::builtin_sin(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	Operand* _v = this->stack->peek();
	this->stack->pop();

	Operand* v = (_v);

	Operand* result = new Operand((float)std::sin(v->get_number_data<float>()));

	this->stack->push(result);

	delete _v;
}

void FunctionFrame::builtin_cos(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	Operand* _v = this->stack->peek();
	this->stack->pop();

	Operand* v = (_v);

	Operand* result = new Operand((float)std::cos(v->get_number_data<float>()));

	this->stack->push(result);

	delete _v;
}

void FunctionFrame::builtin_tan(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	Operand* _v = this->stack->peek();
	this->stack->pop();

	Operand* v = (_v);

	Operand* result = new Operand((float)std::tan(v->get_number_data<float>()));

	this->stack->push(result);

	delete _v;
}

void FunctionFrame::builtin_atan(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	Operand* _v1 = this->stack->peek();
	this->stack->pop();
	Operand* _v2 = this->stack->peek();
	this->stack->pop();

	Operand* v1 = (_v1);
	Operand* v2 = (_v2);

	Operand* result = new Operand((float)std::atan2(v1->get_number_data<float>(), v2->get_number_data<float>()));

	this->stack->push(result);

	delete _v1;
	delete _v2;
}

void FunctionFrame::builtin_abs(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	Operand* _v = this->stack->peek();
	this->stack->pop();

	Operand* v = (_v);

	Operand* result = new Operand(std::abs(v->get_number_data<double>()));

	this->stack->push(result);

	delete _v;
}

void FunctionFrame::builtin_sqrt(Operator* op, CVM* vm, FunctionFrame* caller, Memory* caller_class) {
	Operand* _v = this->stack->peek();
	this->stack->pop();

	Operand* v = (_v);
	float fv = (float)v->get_number_data<float>();

	if (fv < 0) {
		CHESTNUT_THROW_ERROR(L"Failed to execute sqrt function, given value was negative number.", "FAILED_TO_EXECUTE_SQRT", "0x023", op->get_line_number());
	}

	Operand* result = new Operand((float)std::sqrt(fv));

	this->stack->push(result);

	delete _v;
}