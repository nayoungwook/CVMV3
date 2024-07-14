#include "memory.h"

CMClass* Memory::get_cm_class() {
	return cm_class;
}

Memory::Memory(CMClass* cm_class) : cm_class(cm_class) {

}

