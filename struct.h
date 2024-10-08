#include <errno.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// node
typedef struct dll_node_t {
	void *data;
	struct dll_node_t *prev, *next;
} dll_node_t;

// doubly linked list
typedef struct {
	dll_node_t *head;
	int data_size;
	int size;
} doubly_linked_list_t;

// info found in each node from every list
typedef struct {
	int size;
	size_t address;
	void *date;
} info_in_nod;

// Base structure containing the array list
typedef struct {
	size_t start_address_heap; // heap base
	int nr_lists;
	int nr_bytes_per_list; // the size of a list from the array
	int type;
	int free_calls;
	int malloc_calls;
	int fragmentations;
	int nr_bytes_allocated;
	int nr_bytes_free;
	int total_heap_size;
	doubly_linked_list_t **list;
} heap_t;
