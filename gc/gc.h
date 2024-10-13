#pragma once
#include "../sys_logger.h"
#include "../memory.h"

#include <iostream>
#include <stack>
#include <unordered_map>
#include <queue>
#include <unordered_set>

#define GC_THRESHOLD 50

class FunctionFrame;
class Memory;

class Node {
public:
	std::vector<Node*> childs;
	Memory* memory = nullptr;

	Node(Memory* memory);
};

class CGC {
private:
	std::vector<FunctionFrame*>& stack_area;
	std::vector<Memory*>& heap_area;
	std::unordered_map<unsigned int, Operand*>& global_area;
public:
	int gc_counter = 0;
	Memory* current_scene = nullptr;
	CGC(std::vector<FunctionFrame*>& stack_area, std::vector<Memory*>& heap, std::unordered_map<unsigned int, Operand*>& global_area);
	void increase_gc_counter();

	void run();
};