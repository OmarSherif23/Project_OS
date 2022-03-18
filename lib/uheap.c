#include <inc/lib.h>
// malloc()
//	This function use FIRST FIT strategy to allocate space in heap
//  with the given size and return void pointer to the start of the allocated space

//	To do this, we need to switch to the kernel, allocate the required space
//	in Page File then switch back to the user again.
//
//	We can use sys_allocateMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls allocateMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the allocateMem function is empty, make sure to implement it.

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//
struct USER_HEAP_IFORMATION {
	uint32 best_addresses_to_fit;
	int adresses_size;
} pages_info[((USER_HEAP_MAX - USER_HEAP_START)/ PAGE_SIZE)];
bool busy_frame[(USER_HEAP_MAX - USER_HEAP_START) / PAGE_SIZE] = { 0 };
#define NUM_OF_HEAP_PAGES (USER_HEAP_MAX - USER_HEAP_START) / PAGE_SIZE
int num_of_allocated_addresses = 0;
void* Best_fit_strategy(uint32 size) {
	size = ROUNDUP(size, PAGE_SIZE) / PAGE_SIZE;
	int num_of_free_frames = 0, first_index = 0, final_index = 0;
	uint32 best_fit = 1000000;
	bool flag = 0;
	for (int i = 0; i < NUM_OF_HEAP_PAGES; i++) {
		if (busy_frame[i] == 0 && busy_frame[i - 1] == 1)
			first_index = i;
		if (busy_frame[i] == 0)
			num_of_free_frames++;
		else if (busy_frame[i] == 1) {
			if (num_of_free_frames < best_fit && num_of_free_frames >= size) {
				final_index = first_index;
				best_fit = num_of_free_frames;
				flag = 1;
			}
			num_of_free_frames = 0;
		}
		if (num_of_free_frames < best_fit && num_of_free_frames == size && flag == 0) {
			int flag1 = 0;
			for (int j = i; j < NUM_OF_HEAP_PAGES; j++) {
				if (busy_frame[j] == 1) {
					flag1 = 1;
					break;
				}
			}
			if (flag1 == 0) {
				final_index = first_index;
				best_fit = num_of_free_frames;
				flag = 1;
				break;
			}
		}
	}
	if (flag == 0)
		return NULL;
	for (int i = 0; i < size; i++)
		busy_frame[i + final_index] = 1;
	pages_info[num_of_allocated_addresses].adresses_size = size;
	pages_info[num_of_allocated_addresses].best_addresses_to_fit = (final_index * PAGE_SIZE) + USER_HEAP_START;
	return (void*) pages_info[num_of_allocated_addresses].best_addresses_to_fit;
}
void* malloc(uint32 size) {
	uint32* returned_address;
	returned_address = Best_fit_strategy(size);
	if (returned_address == NULL)
		return NULL;
	sys_allocateMem((uint32) returned_address, size);
	num_of_allocated_addresses++;
	return (void*) returned_address;
}
// free():
//	This function frees the allocation of the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from page file and main memory then switch back to the user again.
//
//	We can use sys_freeMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls freeMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the freeMem function is empty, make sure to implement it.
void free(void* virtual_address) {
	//TODO: [PROJECT 2021 - [2] User Heap] free() [User Side]
	// Write your code here, remove the panic and write your code
	//panic("free() is not implemented yet...!!");
	//you should get the size of the given allocation using its address
	//refer to the project presentation and documentation for details
	uint32 sz;
	for (int i = 0; i < num_of_allocated_addresses; i++) {
		if (pages_info[i].best_addresses_to_fit == (uint32) virtual_address) {
			sz = pages_info[i].adresses_size * PAGE_SIZE;
			sys_freeMem((uint32) virtual_address, sz);
			pages_info[i].best_addresses_to_fit =
					pages_info[num_of_allocated_addresses - 1].best_addresses_to_fit;
			pages_info[i].adresses_size =
					pages_info[num_of_allocated_addresses - 1].adresses_size;
			num_of_allocated_addresses--;
			break;
		}
	}
	uint32 mx_size = ((uint32) virtual_address - USER_HEAP_START) / PAGE_SIZE;
	sz = ROUNDUP(sz, PAGE_SIZE) / PAGE_SIZE;
	for (int i = 0; i < sz; i++) {
		busy_frame[i + mx_size] = 0;
	}

}
//==================================================================================//
//================================ OTHER FUNCTIONS =================================//
//==================================================================================//

void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable) {
	panic("this function is not required...!!");
	return 0;
}

void* sget(int32 ownerEnvID, char *sharedVarName) {
	panic("this function is not required...!!");
	return 0;
}

void sfree(void* virtual_address) {
	panic("this function is not required...!!");
}

void *realloc(void *virtual_address, uint32 new_size) {
	panic("this function is not required...!!");
	return 0;
}

void expand(uint32 newSize) {
	panic("this function is not required...!!");
}
void shrink(uint32 newSize) {
	panic("this function is not required...!!");
}

void freeHeap(void* virtual_address) {
	panic("this function is not required...!!");
}
