#include <errno.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "struct.h"

doubly_linked_list_t *dll_create(int data_size)
{
	doubly_linked_list_t *list = malloc(sizeof(doubly_linked_list_t));
	if (!list) {
		fprintf(stderr, "Malloc failed in dll_create.\n");
		return NULL;
	}
	list->data_size = data_size;
	list->head = NULL;
	list->size = 0;
	return list;
}

dll_node_t *dll_get_nth_node(doubly_linked_list_t *list, int n)
{
	if (n < 0)
		return NULL;

	if (n >= list->size)
		n = n % list->size;

	if (n == 0)
		return list->head;

	int count = 0;
	dll_node_t *curr = list->head;
	while (curr) {
		if (count == n)
			return curr;
		count++;
		curr = curr->next;
	}
	return NULL;
}

void dll_add_nth_node(doubly_linked_list_t *list, int n, const void *data)
{
	if (n < 0)
		return;
	if (!list->head) {
		dll_node_t *new = malloc(1 * sizeof(dll_node_t));
		if (!new) {
			fprintf(stderr, "Malloc failed in dll_add_nth_node.\n");
			return;
		}
		new->data = malloc(list->data_size);
		if (!new->data) {
			fprintf(stderr, "Malloc failed in dll_add_nth_node.\n");
			return;
		}
		memcpy(new->data, data, list->data_size);
		list->head = new;
		new->next = NULL;
		list->head->prev = NULL;
		list->size++;
		return;
	}

	if (n == 0) {
		dll_node_t *new = malloc(1 * sizeof(dll_node_t));
		if (!new) {
			fprintf(stderr, "Malloc failed in dll_add_nth_node.\n");
			return;
		}
		new->data = malloc(list->data_size);
		memcpy(new->data, data, list->data_size);
		new->next = list->head;
		new->prev = NULL;
		list->head->prev = new;
		list->head = new;
		list->size++;
		return;
	}

	int contor = 0;
	dll_node_t *curr_node = list->head;
	while (curr_node->next) {
		if (contor == n - 1) {
			dll_node_t *new = malloc(1 * sizeof(dll_node_t));
			if (!new) {
				fprintf(stderr, "Malloc failed in dll_add_nth_node.\n");
				return;
			}
			new->data = malloc(list->data_size);
			memcpy(new->data, data, list->data_size);
			new->next = curr_node->next;
			new->prev = curr_node;
			curr_node->next = new;
			(new->next)->prev = new;
			list->size++;
			return;
		}

		// go to next
		contor++;
		curr_node = curr_node->next;
	}

	// contor = number of nodes
	if (n >= contor) {
		// add to the end of the list
		dll_node_t *new = malloc(1 * sizeof(dll_node_t));
		if (!new) {
			fprintf(stderr, "Malloc failed in dll_add_nth_node.\n");
			return;
		}
		new->data = malloc(list->data_size);
		memcpy(new->data, data, list->data_size);
		curr_node->next = new;
		new->prev = curr_node;
		new->next = NULL;
		list->size++;
	}
}

dll_node_t *dll_remove_nth_node(doubly_linked_list_t *list, int n)
{
	if (n < 0)
		return NULL;

	if (n == 0 && list->size == 1) {
		dll_node_t *aux = list->head;
		list->head = NULL;
		list->size--;
		return aux;
	}

	if (n == 0) {
		dll_node_t *aux = list->head;
		list->head = aux->next;
		list->head->prev = NULL;
		list->size--;
		return aux;
	}

	dll_node_t *node = list->head;
	int contor = 0;
	while (node) {
		if (contor == list->size - 2)
			break;
		if (contor == n - 1) {
			// modify connections
			dll_node_t *aux = node->next;
			node->next = aux->next;
			aux->next->prev = node;
			list->size--;
			return aux;
		}

		contor++;
		node = node->next;
	}

	if (n >= contor) {
		// remove last element
		node = list->head;
		contor = 0;
		while (node) {
			if (contor == list->size - 2) {
				// modify connections
				dll_node_t *aux = node->next;
				node->next = NULL;
				list->size--;
				return aux;
			}

			contor++;
			node = node->next;
		}
	}
	return NULL;
}

void dll_free(doubly_linked_list_t **pp_list)
{
	if (*pp_list) {
		dll_node_t *aux = (*pp_list)->head;
		while (aux) {
			(*pp_list)->head = aux->next;
			if (aux->data) {
				if (((info_in_nod *)aux->data)->date)
					free(((info_in_nod *)aux->data)->date);
				free(aux->data);
			}
			free(aux);
			aux = (*pp_list)->head;
		}

		free(*pp_list);
		*pp_list = NULL;
	}
}

// INIT_HEAP
void init_heap(heap_t *free_l)
{
	free_l->free_calls = 0;
	free_l->malloc_calls = 0;
	free_l->fragmentations = 0;
	free_l->nr_bytes_allocated = 0;
	free_l->nr_bytes_free = free_l->nr_bytes_per_list * free_l->nr_lists;
	free_l->total_heap_size = free_l->nr_bytes_per_list * free_l->nr_lists;

	// allocate memory for the array lists
	free_l->list = malloc(free_l->nr_lists * sizeof(doubly_linked_list_t *));
	if (!free_l->list) {
		fprintf(stderr, "Malloc failed in init_heap.\n");
		return;
	}

	// create and initialize each list from array
	info_in_nod *info = malloc(sizeof(info_in_nod));
	if (!info) {
		fprintf(stderr, "Malloc failed in init_heap.\n");
		return;
	}
	info->size = 8;
	info->address = free_l->start_address_heap;
	info->date = NULL;

	for (int i = 0; i < free_l->nr_lists; i++) {
		free_l->list[i] = dll_create(sizeof(info_in_nod));

		// initialize each node from list
		int index = 0;
		int nr_nodes = free_l->nr_bytes_per_list / info->size;

		while (index < nr_nodes) {
			dll_add_nth_node(free_l->list[i], index, (info_in_nod *)info);
			info->address = info->address + info->size;
			index++;
		}

		// modify the data for the next list in the array
		info->size = info->size * 2;
	}
	free(info);
}

// returns the position where a node should be added to a list to
// remain sorted by addresses
int find_position_to_put(doubly_linked_list_t *list, size_t addr)
{
	if (!list->head)
		return 0;

	if (addr < ((info_in_nod *)list->head->data)->address)
		return 0;

	int position = 1;
	dll_node_t *node = list->head;
	while (node->next) {
		if (addr > ((info_in_nod *)node->data)->address &&
			addr < ((info_in_nod *)node->next->data)->address) {
			return position;
		}
		position++;
		node = node->next;
	}
	return position;
}

// returns -1 if there is no block in free_list with size, otherwise it
// returns the index where it found one
int check_if_size_exists(heap_t *free_l, int size)
{
	for (int i = 0; i < free_l->nr_lists; i++) {
		if (((info_in_nod *)free_l->list[i]->head->data)->size == size)
			return i;
	}
	return -1;
}

// add a new list in the array of lists
void add_new_list(heap_t *free_l, int size, void *data_to_add)
{
	// find the index where to add the list
	int index_to_add_list = free_l->nr_lists;
	for (int i = 0; i < free_l->nr_lists; i++) {
		if (size < ((info_in_nod *)free_l->list[i]->head->data)->size) {
			index_to_add_list = i;
			break;
		}
	}

	// allocate new memory
	free_l->nr_lists++;
	free_l->list = realloc(free_l->list,
						   free_l->nr_lists * sizeof(doubly_linked_list_t *));
	if (!free_l->list) {
		fprintf(stderr, "Malloc failed in add_new_list.\n");
		return;
	}

	// shift all the terms of the vector with an index further to the right
	// so that to have room for the new list
	free_l->list[free_l->nr_lists - 1] = dll_create(sizeof(info_in_nod));
	for (int i = free_l->nr_lists - 2; i >= index_to_add_list; i--) {
		memcpy(free_l->list[i + 1], free_l->list[i],
			   sizeof(doubly_linked_list_t));
	}

	// create a new list at index_to_add_list
	free(free_l->list[index_to_add_list]);
	free_l->list[index_to_add_list] = dll_create(sizeof(info_in_nod));
	dll_add_nth_node(free_l->list[index_to_add_list], 0,
					 (info_in_nod *)(data_to_add));
}

// remove a list from free_list (there are no addresses there)
void remove_list(heap_t *free_l, int index)
{
	// free the list from index (the one to remove)
	dll_free(&free_l->list[index]);
	free_l->list[index] = dll_create(sizeof(info_in_nod));

	// shift them all with an index further to the left
	for (int i = index; i < free_l->nr_lists - 1; i++)
		memcpy(free_l->list[i], free_l->list[i + 1],
			   sizeof(doubly_linked_list_t));
	free(free_l->list[free_l->nr_lists - 1]);
	free_l->nr_lists--;
}

// MALLOC
void my_malloc(heap_t *free_l, doubly_linked_list_t **allocated_list,
			   int nr_bytes)
{
	if (!free_l)
		return;

	// search for the first list in which nr_bytes fit
	int diff;
	for (int i = 0; i < free_l->nr_lists; i++) {
		diff = (((info_in_nod *)free_l->list[i]->head->data)->size) - nr_bytes;
		if (diff >= 0) {
			// update nr_bytes
			free_l->nr_bytes_allocated += nr_bytes;
			free_l->nr_bytes_free -= nr_bytes;

			// delete the node to add it to the allocated_list
			dll_node_t *node_rmv = dll_remove_nth_node(free_l->list[i], 0);
			// must be deleted (there was only one element left that I deleted)
			if (!free_l->list[i]->head)
				remove_list(free_l, i);

			// if there is a list whose size is equal to nr_bytes
			if (diff == 0) {
				// add it so that it remains sorted
				int pos = find_position_to_put(*allocated_list,
					((info_in_nod *)node_rmv->data)->address);
				dll_add_nth_node(*allocated_list, pos,
								 (info_in_nod *)node_rmv->data);
				free(node_rmv->data);
				free(node_rmv);
				free_l->malloc_calls++;
				return;
			}

			// if it doesn't exist, I create another list with size = diff
			if (diff != 0) {
				// change the size of the node that I add in allocated_list
				((info_in_nod *)node_rmv->data)->size = nr_bytes;
				int pos2 = find_position_to_put(*allocated_list,
					((info_in_nod *)node_rmv->data)->address);
				dll_add_nth_node(*allocated_list, pos2,
								 (info_in_nod *)node_rmv->data);

				// change the size again to add the rest to the free_list
				((info_in_nod *)node_rmv->data)->size = diff;
				((info_in_nod *)node_rmv->data)->address += (size_t)nr_bytes;

				// look for the size among the existing lists in free_list
				int index_lista = check_if_size_exists(free_l, diff);
				if (index_lista != -1) {
					// add the node to the corresponding list (remains sorted)
					int pos2 = find_position_to_put(free_l->list[index_lista],
						((info_in_nod *)node_rmv->data)->address);
					dll_add_nth_node(free_l->list[index_lista], pos2,
									 (info_in_nod *)node_rmv->data);
				} else {
					// create a new list in the array list
					add_new_list(free_l, diff, (info_in_nod *)(node_rmv->data));
				}
				free_l->fragmentations++;
				free(node_rmv->data);
				free(node_rmv);
			}
			free_l->malloc_calls++;
			return;
		}
	}
	printf("Out of memory\n");
}

// returns -1 if not found, otherwise returns the index
int check_if_address_exists(doubly_linked_list_t *list, size_t address)
{
	dll_node_t *curr = list->head;
	int index = 0;
	while (curr) {
		if (((info_in_nod *)curr->data)->address == address)
			return index;
		index++;
		curr = curr->next;
	}
	return -1;
}

// FREE
void my_free(heap_t *free_l, doubly_linked_list_t **allocated_list,
			 size_t addr)
{
	if (!free_l)
		return;
	int index = check_if_address_exists(*allocated_list, addr);
	if (index != -1) {
		// remove it from the allocated_list
		dll_node_t *node_removed = dll_remove_nth_node(*allocated_list, index);

		// update the number of allocated bytes
		free_l->nr_bytes_allocated = free_l->nr_bytes_allocated -
			((info_in_nod *)(node_removed->data))->size;

		if (free_l->type == 0) {
			// update the number of free bytes
			free_l->nr_bytes_free = free_l->nr_bytes_free +
									((info_in_nod *)(node_removed->data))->size;

			// add back to the array list
			int index_lista = check_if_size_exists(free_l,
				((info_in_nod *)node_removed->data)->size);
			if (index_lista != -1) {
				// add the node to the corresponding list(sorted by addresses)
				int position = find_position_to_put(free_l->list[index_lista],
					((info_in_nod *)node_removed->data)->address);
				dll_add_nth_node(free_l->list[index_lista], position,
								 (info_in_nod *)node_removed->data);
			} else {
				// create a new list in the array list with a new size
				add_new_list(free_l, ((info_in_nod *)node_removed->data)->size,
							 (info_in_nod *)node_removed->data);
			}
		}
		free_l->free_calls++;
		free(node_removed->data);
		free(node_removed);
	} else {
		printf("Invalid free\n");
	}
}

// DUMP_MEMORY
void dump_memory(heap_t *free_l, doubly_linked_list_t *allocated_list)
{
	if (!free_l)
		return;
	printf("+++++DUMP+++++\n");

	// heap size
	printf("Total memory: %d bytes\n", free_l->total_heap_size);

	// nr_bytes_allocated
	printf("Total allocated memory: %d bytes\n", free_l->nr_bytes_allocated);

	// nr_bytes_free
	printf("Total free memory: %d bytes\n", free_l->nr_bytes_free);

	// number of free blocks
	int free_blocks = 0;
	for (int i = 0; i < free_l->nr_lists; i++)
		free_blocks += free_l->list[i]->size;
	printf("Free blocks: %d\n", free_blocks);

	// number of allocated blocks
	printf("Number of allocated blocks: %d\n", allocated_list->size);

	// malloc calls
	printf("Number of malloc calls: %d\n", free_l->malloc_calls);

	// fragmentations
	printf("Number of fragmentations: %d\n", free_l->fragmentations);

	// free calls
	printf("Number of free calls: %d\n", free_l->free_calls);

	// print each block from free_list
	for (int i = 0; i < free_l->nr_lists; i++) {
		int bytes = ((info_in_nod *)(free_l->list[i]->head->data))->size;
		int blocks = free_l->list[i]->size;
		printf("Blocks with %d bytes - %d free block(s) :", bytes, blocks);
		dll_node_t *curr = free_l->list[i]->head;
		while (curr) {
			printf(" 0x%lx", ((info_in_nod *)(curr->data))->address);
			curr = curr->next;
		}
		printf("\n");
	}

	// print each block from allocated_list
	printf("Allocated blocks :");
	dll_node_t *node = allocated_list->head;
	while (node) {
		printf(" (0x%lx - %d)", ((info_in_nod *)node->data)->address,
			   ((info_in_nod *)node->data)->size);
		node = node->next;
	}

	printf("\n-----DUMP-----\n");
}

// DESTROY_HEAP
void destroy_heap(heap_t *free_l, doubly_linked_list_t **allocated_list)
{
	if (!free_l)
		return;
	for (int i = 0; i < free_l->nr_lists; i++)
		dll_free(&free_l->list[i]);
	if (free_l->list)
		free(free_l->list);

	if (free_l)
		free(free_l);
	free_l = NULL;

	// free the memory from the allocated_list
	dll_free(allocated_list);
}

// returns the node that contains a certain address from the allocated list
dll_node_t *place_me_to_address(doubly_linked_list_t *allocated_list,
								size_t start_addr)
{
	dll_node_t *curr = allocated_list->head;
	while (curr) {
		if (((info_in_nod *)curr->data)->address == start_addr)
			break;
		curr = curr->next;
	}
	return curr;
}

// check if all consecutive addresses required for writing are valid
int check_seg_fault_in_write(int bytes_available, int nr_bytes,
							 dll_node_t *curr, heap_t *free_l,
							 doubly_linked_list_t *allocated_list)
{
	while (bytes_available < nr_bytes && curr) {
		size_t addr1 = ((info_in_nod *)(curr->data))->address;
		curr = curr->next;
		if (!curr) {
			printf("Segmentation fault (core dumped)\n");
			dump_memory(free_l, allocated_list);
			destroy_heap(free_l, &allocated_list);
			return -1;
		}
		size_t addr2 = ((info_in_nod *)(curr->data))->address;
		if (addr2 != addr1 + ((info_in_nod *)curr->prev->data)->size) {
			printf("Segmentation fault (core dumped)\n");
			dump_memory(free_l, allocated_list);
			destroy_heap(free_l, &allocated_list);
			return -1;
		}
		// add as much as available
		bytes_available += ((info_in_nod *)(curr->prev->data))->size;
	}
	// No seg fault detected
	return 0;
}

// write all the input in a single address (there is room)
void write_in_one_addr(dll_node_t *curr, int nr_bytes, void *data_to_add)
{
	if (((info_in_nod *)curr->data)->date) {
		// if I write something smaller over something bigger
		if ((int)strlen((((info_in_nod *)curr->data)->date)) >= nr_bytes) {
			strncpy((char *)(((info_in_nod *)(curr->data))->date),
					(char *)data_to_add, nr_bytes);
		} else {
			// if I write something bigger over something smaller
			(((info_in_nod *)(curr->data))->date) =
				(char *)realloc(((info_in_nod *)(curr->data))->date,
								(nr_bytes + 1) * sizeof(char));
			memcpy((char *)(((info_in_nod *)(curr->data))->date),
				   (char *)data_to_add, nr_bytes);
			((char *)(((info_in_nod *)(curr->data))->date))[nr_bytes] = '\0';
		}
	} else {
		((info_in_nod *)(curr->data))->date =
			(char *)malloc((nr_bytes + 1) * sizeof(char));
		memcpy(((info_in_nod *)(curr->data))->date, data_to_add, nr_bytes);
		((char *)(((info_in_nod *)(curr->data))->date))[nr_bytes] = '\0';
	}
}

// write only part of the string in an address
void write_partially_in_curr_node(dll_node_t *curr, int nr_bytes,
								  void *data_to_add)
{
	if (((info_in_nod *)curr->data)->date) {
		// if I write something smaller over something bigger
		if ((int)strlen(((info_in_nod *)curr->data)->date) >= nr_bytes) {
			strcpy(((info_in_nod *)(curr->data))->date, (char *)data_to_add);
		} else {
			// if I write something bigger over something smaller
			((info_in_nod *)(curr->data))->date =
				(char *)realloc(((info_in_nod *)(curr->data))->date,
								(nr_bytes + 1) * sizeof(char));
			if (!((info_in_nod *)curr->data)->date) {
				fprintf(stderr, "Malloc failed.\n");
				return;
			}
			memcpy(((info_in_nod *)(curr->data))->date, data_to_add, nr_bytes);
			((char *)(((info_in_nod *)(curr->data))->date))[nr_bytes] = '\0';
		}
	} else {
		((info_in_nod *)(curr->data))->date =
			(char *)malloc((nr_bytes + 1) * sizeof(char));
		if (!((info_in_nod *)curr->data)->date) {
			fprintf(stderr, "Malloc failed.\n");
			return;
		}
		memcpy(((info_in_nod *)(curr->data))->date, data_to_add, nr_bytes);
		((char *)(((info_in_nod *)(curr->data))->date))[nr_bytes] = '\0';
	}
}

// WRITE
int my_write(heap_t *free_l, doubly_linked_list_t *allocated_list,
			 size_t start_addr, void *data_to_add, int data_size,
			 int nr_bytes)
{
	if (!free_l)
		return 0;
	if (check_if_address_exists(allocated_list, start_addr) != -1) {
		if (nr_bytes >= data_size)
			nr_bytes = data_size;

		// reach the node with the address where I can start writing
		dll_node_t *curr = place_me_to_address(allocated_list, start_addr);

		// initialize with how many bytes I find at start_address
		int bytes_available = ((info_in_nod *)(curr->data))->size;

		// if space available in one address, allocate directly
		if (bytes_available >= nr_bytes) {
			write_in_one_addr(curr, nr_bytes, data_to_add);
			return 0;
		}

		// the case when I write to more addresses
		if (check_seg_fault_in_write(bytes_available, nr_bytes, curr,
									 free_l, allocated_list) == -1)	{
			return -1;
		}

		// if it's reached here, all addresses are valid
		curr = place_me_to_address(allocated_list, start_addr);
		while (nr_bytes >= ((info_in_nod *)(curr->data))->size)	{
			int length = ((info_in_nod *)(curr->data))->size;
			if (((info_in_nod *)curr->data)->date) {
				// fill everything I have available
				((info_in_nod *)(curr->data))->date =
					(char *)realloc(((info_in_nod *)(curr->data))->date,
					(length + 1) * sizeof(char));
				if (!((info_in_nod *)curr->data)->date) {
					fprintf(stderr, "Malloc failed.\n");
					return 1;
				}
			} else {
				((info_in_nod *)(curr->data))->date =
					(char *)malloc((length + 1) * sizeof(char));
				if (!((info_in_nod *)curr->data)->date) {
					fprintf(stderr, "Malloc failed.\n");
					return 1;
				}
			}
			memcpy(((info_in_nod *)(curr->data))->date, data_to_add, length);
			((char *)(((info_in_nod *)(curr->data))->date))[length] = '\0';

			// modify the string (delete the first size elements that I wrote)
			int bytes_to_copy = strlen(data_to_add) - length + 1;
			memmove(data_to_add, data_to_add + length, bytes_to_copy);

			// update how many bytes left to add
			nr_bytes -= length;
			curr = curr->next;
		}
		// add those remaining bytes to the node I'm in now
		write_partially_in_curr_node(curr, nr_bytes, data_to_add);
	} else {
		printf("Segmentation fault (core dumped)\n");
		dump_memory(free_l, allocated_list);
		destroy_heap(free_l, &allocated_list);
		return -1;
	}
	return 0;
}

// check if all consecutive addresses required for reading are valid
int check_seg_fault_in_read(int bytes_written, int nr_bytes, dll_node_t *curr,
							heap_t *free_l,
							doubly_linked_list_t *allocated_list)
{
	while (bytes_written < nr_bytes && curr) {
		size_t addr1 = ((info_in_nod *)(curr->data))->address;
		curr = curr->next;
		if (!curr) {
			printf("Segmentation fault (core dumped)\n");
			dump_memory(free_l, allocated_list);
			destroy_heap(free_l, &allocated_list);
			return -1;
		}
		size_t addr2 = ((info_in_nod *)(curr->data))->address;
		if (addr2 != addr1 + ((info_in_nod *)curr->prev->data)->size) {
			printf("Segmentation fault (core dumped)\n");
			dump_memory(free_l, allocated_list);
			destroy_heap(free_l, &allocated_list);
			return -1;
		}
		// add as much as found
		bytes_written += strlen(((info_in_nod *)(curr->prev->data))->date);
	}
	// No segmentation fault detected
	return 0;
}

// READ
int my_read(heap_t *free_l, doubly_linked_list_t *allocated_list,
			size_t start_addr, int nr_bytes)
{
	if (!free_l)
		return 0;
	if (check_if_address_exists(allocated_list, start_addr) != -1) {
		// reach the node with the target address to begin reading
		dll_node_t *curr = place_me_to_address(allocated_list, start_addr);

		// initialize with how many written bytes I find at start_address
		int bytes_written = 0;
		if (((info_in_nod *)curr->data)->date) {
			bytes_written = strlen(((info_in_nod *)(curr->data))->date);
		} else {
			printf("Segmentation fault (core dumped)\n");
			dump_memory(free_l, allocated_list);
			destroy_heap(free_l, &allocated_list);
			return -1;
		}

		// if I have all the bytes in a single address, I only print from it
		if (bytes_written >= nr_bytes) {
			char *to_print = malloc((nr_bytes + 1) * sizeof(char));
			strncpy(to_print, ((info_in_nod *)curr->data)->date, nr_bytes);
			to_print[nr_bytes] = '\0';
			printf("%s\n", to_print);
			free(to_print);
			return 0;
		}

		// check if all addresses are valid
		if (check_seg_fault_in_read(bytes_written, nr_bytes, curr,
									free_l, allocated_list) == -1) {
			return -1;
		}

		// reach the node with the address where I can start printing
		curr = place_me_to_address(allocated_list, start_addr);

		// create the string to print
		int bytes_printed = 0;
		char *to_print = malloc((nr_bytes + 1) * sizeof(char));
		strcpy(to_print, "");
		while (bytes_printed + strlen(((info_in_nod *)curr->data)->date) <
			   (unsigned long)nr_bytes) {
			strcat(to_print, ((info_in_nod *)curr->data)->date);
			bytes_printed += strlen(((info_in_nod *)curr->data)->date);
			curr = curr->next;
		}

		int rest_to_print = nr_bytes - bytes_printed;
		strncat(to_print, ((info_in_nod *)curr->data)->date, rest_to_print);
		to_print[nr_bytes] = '\0';

		printf("%s\n", to_print);
		free(to_print);
	} else {
		printf("Segmentation fault (core dumped)\n");
		dump_memory(free_l, allocated_list);
		destroy_heap(free_l, &allocated_list);
		return -1;
	}
	return 0;
}

int main(void)
{
	heap_t *free_list = malloc(1 * sizeof(heap_t));
	if (!free_list) {
		fprintf(stderr, "Malloc failed.\n");
		return -1;
	}
	doubly_linked_list_t *allocated_list = dll_create(sizeof(info_in_nod));
	int destroy_done = 0;

	char *function_name = malloc(50 * sizeof(char));
	if (!function_name) {
		fprintf(stderr, "Malloc failed.\n");
		return -1;
	}
	while (destroy_done == 0) {
		scanf("%s", function_name);
		if (strcmp(function_name, "INIT_HEAP") == 0) {
			scanf("%lx%d%d%d", &free_list->start_address_heap,
				  &free_list->nr_lists, &free_list->nr_bytes_per_list,
				  &free_list->type);
			init_heap(free_list);
		}

		if (strcmp(function_name, "MALLOC") == 0) {
			int nr_bytes;
			scanf("%d", &nr_bytes);
			my_malloc(free_list, &allocated_list, nr_bytes);
		}

		if (strcmp(function_name, "FREE") == 0) {
			size_t addr;
			scanf("%lx", &addr);
			my_free(free_list, &allocated_list, addr);
		}

		if (strcmp(function_name, "READ") == 0) {
			size_t address;
			int nr_bytes;
			scanf("%lx%d", &address, &nr_bytes);
			destroy_done =
				my_read(free_list, allocated_list, address, nr_bytes);
		}

		if (strcmp(function_name, "WRITE") == 0) {
			size_t start_addr;
			char *data = malloc(200 * sizeof(char));
			int nr_bytes;
			scanf("%lx \"%[^\"]\" %d", &start_addr, data, &nr_bytes);
			data[strlen(data)] = '\0';
			destroy_done = my_write(free_list, allocated_list, start_addr, data,
									strlen(data), nr_bytes);
			free(data);
		}

		if (strcmp(function_name, "DUMP_MEMORY") == 0)
			dump_memory(free_list, allocated_list);

		if (strcmp(function_name, "DESTROY_HEAP") == 0) {
			destroy_heap(free_list, &allocated_list);
			break;
		}
	}
	free(function_name);
	return 0;
}
