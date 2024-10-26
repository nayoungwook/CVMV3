#include "gc.h"
#include "../function_frame.h"
#include "../parser.h"

Node::Node(Memory* memory) : memory(memory) {
}

CGC::CGC(std::vector<FunctionFrame*>& stack_area, std::vector<Memory*>& heap_area, std::unordered_map<unsigned int, Operand*>& global_area)
	: stack_area(stack_area), heap_area(heap_area), global_area(global_area) {

}

std::unordered_map<Memory*, Node*> gc_nodes;
extern std::unordered_set<std::wstring> enabled_options;

void disconnectNode(Memory* target, Memory* remove_target_element) {
	std::unordered_map<Memory*, Node*>::iterator gc_node_iterator = gc_nodes.find(target);

	for (int i = 0; i < gc_node_iterator->second->childs.size(); i++) {
		if (gc_node_iterator->second->childs[i]->memory == remove_target_element) {
			gc_node_iterator->second->childs.erase(gc_node_iterator->second->childs.begin() + i);
			break;
		}
	}
}

void CGC::run() {

	bool gc_log_enabled = enabled_options.find(L"debug_view_gc_log") != enabled_options.end();

	if (gc_log_enabled) {
		/*
		std::wstring GARBAGE_TRUCK =
			L"\n       _______________   ___ / -\\___   ___ / -\\___   ___ / -\\___	\n"
			"     / /             ||  |---------|   |---------|   |---------|		\n"
			"    / /              ||   |       |     | | | | |     |   |   |		\n"
			"   / /             __||   |       |     | | | | |     | | | | |		\n"
			"  / /   \\\\        I  ||   |       |     | | | | |     | | | | |		\n"
			" (-------------------||   | | | | |     | | | | |     | | | | |		\n"
			" ||               == ||   |_______|     |_______|     |_______|		\n"
			" ||garbage collector | =============================================	\n"
			" ||          ____    |                                 ____      |		\n"
			"( | o      / ____ \\                                 / ____ \\    |)	\n"
			" ||      / / . . \\ \\                              / / . . \\ \\   |	\n"
			"[ |_____| | .   . | |____________________________  || .   .  | |__]	\n"
			"          | .   . |                                 | .   .  |			\n"
			"           \\_____/                                  \\_____/			\n";
		CHESTNUT_LOG(GARBAGE_TRUCK, log_level::log_okay);
		*/

		CHESTNUT_LOG(L"GC for chestnut is running !!", log_level::log_okay);
	}

	// root memories
	std::vector<Memory*> root_memories;

	// for local variables
	for (FunctionFrame* current_frame : this->stack_area) {

		std::unordered_map<unsigned int, Operand*> local_area = current_frame->local_area;
		std::unordered_map<unsigned int, Operand*>::iterator local_area_iterator = local_area.begin();

		for (; local_area_iterator != local_area.end(); local_area_iterator++) {
			Operand* op = (local_area_iterator->second);

			if (op->get_type() == operand_address) {
				Memory* root_memory = (Memory*)op->data;
				root_memories.push_back(root_memory);
			}
		}
	}

	// for global variables
	std::unordered_map<unsigned int, Operand*>::iterator global_area_iterator = this->global_area.begin();

	for (; global_area_iterator != this->global_area.end(); global_area_iterator++) {
		Operand* op = (global_area_iterator->second);

		if (op->get_type() == operand_address) {
			root_memories.push_back((Memory*)op->data);
		}
	}

	// for additional builtin variables
	root_memories.push_back(this->current_scene);

	if (gc_log_enabled)
		CHESTNUT_LOG(L"Root memories collected", log_level::log_default);

	// start searching with queue
	std::queue<Node*> q;
	std::unordered_set<Memory*> marked_memories;

	for (Memory* root : root_memories) {
		// for unexpected memories like (shader, mouse etc...) just continue
		// if it is unable to find memory in gc_nodes.
		if (gc_nodes.find(root) == gc_nodes.end() || root == nullptr) {
			continue;
		}

		Node* root_node = gc_nodes[root];
		q.push(root_node);
	}

	if (gc_log_enabled)
		CHESTNUT_LOG(L"Marked.", log_level::log_default);

	while (!q.empty()) {
		Node* node = q.front();
		q.pop();

		if (marked_memories.find(node->memory) == marked_memories.end()) {
			marked_memories.insert(node->memory);
		}
		else {
			continue;
		}

		for (Node* child_node : node->childs) {
			if (marked_memories.find(child_node->memory) == marked_memories.end()) {
				q.push(child_node);
			}
		}
	}

	if (gc_log_enabled) {
		CHESTNUT_LOG(L"Root memory count : " + std::to_wstring(root_memories.size()), log_level::log_default);
		CHESTNUT_LOG(L"Marked node count : " + std::to_wstring(marked_memories.size()), log_level::log_default);
		CHESTNUT_LOG(L"Unmarked node count : " + std::to_wstring(this->heap_area.size() - marked_memories.size()), log_level::log_default);
	}

	// delete unmarked nodes.

	unsigned int deleted_count = 0;
	std::queue<Memory*> delete_target_memories;
	for (int i = 0; i < this->heap_area.size(); i++) {
		bool alive = marked_memories.find(this->heap_area[i]) != marked_memories.end();

		if (!alive) {
			delete_target_memories.push(this->heap_area[i]);
			deleted_count++;
		}
	}

	while (!delete_target_memories.empty()) {
		Memory* del_target = delete_target_memories.front();
		delete_target_memories.pop();

		this->heap_area.erase(remove(this->heap_area.begin(), this->heap_area.end(), del_target));

		std::unordered_map<Memory*, Node*>::iterator gc_node_iterator = gc_nodes.begin();

		for (; gc_node_iterator != gc_nodes.end(); gc_node_iterator++) {
			if (gc_node_iterator->first == del_target) {
				delete gc_node_iterator->second;
				gc_nodes.erase(gc_node_iterator);
				break;
			}
		}

		delete del_target;
	}

	if (gc_log_enabled) {
		CHESTNUT_LOG(L"[GC] memory " + std::to_wstring(deleted_count) + L" deleted.", log_level::log_default);
	}
}

void CGC::increase_gc_counter() {
	this->gc_counter++;
}