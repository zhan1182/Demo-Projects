#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "linked_list.h"

#define SUCCESS 1
#define FAILURE 0
#define NEWLINE '\n'
#define TERMINATING_NULL_BYTE '\0'
#define MAXLINE 512
#define READ_MODE "r"
#define WRITE_MODE "w"

int llist_lookup_index (LINKED_LIST* list, char* name);

//
// It returns a new double LINKED_LIST. It allocates it dynamically,
// and initialises the values. The initial list is empty.
//
LINKED_LIST* llist_create () 
{

	LINKED_LIST* list = malloc (sizeof (LINKED_LIST));
	if (list == NULL) 
    {
		return NULL;
	}

	// Create Sentinel node. This node does not store any data
	// but simplifies the list implementation.
	list->head = malloc (sizeof (LINKED_LIST_ENTRY));
	if (list->head == NULL) 
    {
		return NULL;
	}
	
    // Initialise LINKED_LIST elements
	list->nElements = 0;
    
    // Initialise dummy node elements
    (list->head)->name = NULL;
    (list->head)->value = NULL;
    // list->head is beginning and end of double linked list
	(list->head)->next = list->head;
	(list->head)->previous = list->head;
	
	return list;
}

//
// It prints the elements in the list in the form:
//
//===== List =====
//nElements=2
//name="George" value="23 Oak St, West Lafayette, 47906"
//name="Peter" value="27 Oak St, West Lafayette, 47906"
//======== End List =======
//
void llist_print (LINKED_LIST* list)
{
	LINKED_LIST_ENTRY* node;

	printf ("===== List =====\n");
	printf ("nElements=%d\n", list->nElements);

	node = (list->head)->next;
	
    while (node != list->head) // list->head is beginning and end of double linked list
    {
		printf ("name=\"%s\" value=\"%s\"\n", node->name, node->value);
		
        node = node->next;
	}
	
    printf ("======== End List =======\n");
}

//
// Adds one pair name/value to the END of the list. If the name already exists it will
// substitute its value. Otherwise, it will store name/value in a new entry.
// The name/value strings are duplicated with strdup() before adding them to the
// list.
//
int llist_add (LINKED_LIST* list, char* name, char* value) 
{
    LINKED_LIST_ENTRY* node = (list->head)->next;
    
    // See if name already exists in list
    while (node != list->head)
    {
        if (strcmp(node->name, name) == 0)
        {
            free (node->value);
            node->value = strdup (value);
            
            return SUCCESS;
        }
        
        node = node->next;
    }

    // At this point, did not find name in list
	return llist_insert_last (list, name, value);
}

//
// Returns the value that corresponds to the name or NULL if the
// name does not exist in the list.
//
char* llist_lookup (LINKED_LIST* list, char* name) 
{
    LINKED_LIST_ENTRY* node = (list->head)->next;
    
    while (node != list->head)
    {
        if (strcmp(node->name, name) == 0)
        {
            return node->value;
        }
        
        node = node->next;
    }
	
    return NULL;
}

//
// It removes the entry with that name from the list.
// Also the name and value strings will be freed.
//
int llist_remove (LINKED_LIST* list, char* name) 
{
    LINKED_LIST_ENTRY* node = (list->head)->next;
    
    while (node != list->head)
    {
        if (strcmp(node->name, name) == 0)
        {
            // Update list pointers
            (node->previous)->next = node->next;
            (node->next)->previous = node->previous;
            
            free (node->name);
            free (node->value);
            free (node);
            
            // Update nElements
            list->nElements --;
            
            return SUCCESS;
        }
        
        node = node->next;
    }
    
    // At this point, could not find name in list
	return FAILURE;
}

//
// It returns in *name and *value the name and value that correspond to
// the ith entry. It will return 1 if successful, or 0 otherwise.
//
int llist_get_ith (LINKED_LIST* list, int ith, char** name, char** value)
{
    int i = 0; // List index

	LINKED_LIST_ENTRY* node = (list->head)->next;
    
    while (node != list->head)
    {
        if (i == ith)
        {
            *name = node->name;
            *value = node->value;
            
            return SUCCESS;
        }
        
        node = node->next;
        i ++;
    }
    
    // At this point, ith entry was out of bounds
	return FAILURE;
}

//
// It removes the ith entry from the list.
// Also the name/value strings are freed.
//
int llist_remove_ith (LINKED_LIST* list, int ith)
{
	int i = 0; // List index

	LINKED_LIST_ENTRY* node = (list->head)->next; // Ensures you never remove dummy head node.
    
    while (node != list->head)
    {
        if (i == ith)
        {
            // Update list pointers
            (node->previous)->next = node->next;
            (node->next)->previous = node->previous;
            
            free (node->name);
            free (node->value);
            free (node);
            
            // Update nElements
            list->nElements --;
            
            return SUCCESS;
        }
        
        node = node->next;
        i ++;
    }
    
    // At this point, ith entry was out of bounds
	return FAILURE;
}

//
// It returns the number of elements in the list.
//
int llist_number_elements (LINKED_LIST* list) 
{
	return list->nElements;
}

//
// It saves the list in a file called file_name. The format of the
// file is as follows:
//
// name1\n
// value1\n
// \n
// name2\n
// value2\n
// ...
//
// Notice that there is an empty line between each name/value pair.
//
int llist_save (LINKED_LIST* list, char* file_name)
{
    FILE* fout = fopen (file_name, WRITE_MODE);
    if (fout == NULL) // fopen failed
    {
        return FAILURE;
    }
    
    LINKED_LIST_ENTRY* node = (list->head)->next;
    
    while (node != list->head)
    {
        fprintf (fout, "%s\n", node->name);
        fprintf (fout, "%s\n\n", node->value);
        
        node = node->next;
    }

	fclose (fout);
    
	return SUCCESS;
}

/* Removes newline character from the end of input strings. */
void sanitise (char* str)
{
    int len = strlen (str);
    
    if (str[len - 1] == NEWLINE)
    {
        str[len - 1] = TERMINATING_NULL_BYTE;
    }
}

//
// It reads the list from the file_name indicated. If the list already has entries, it will
// clear the entries.
//
int llist_read (LINKED_LIST* list, char* file_name)
{
    int i; // List index
	char line[MAXLINE + 1]; // Temporary buffer to store name/value read in from file
    char* name = NULL; // Temporarily store input name
    char* value = NULL; // Temporarily store input value 
    
    FILE* fin = fopen (file_name, READ_MODE);
    if (fin == NULL) // fopen failed
    {
        return FAILURE;
    }
    
    if (list->nElements > 0) // List already has elements
    {
        for (i = 0; i < list->nElements; i ++)
        {
            llist_remove_ith (list, i);
        }
    }
    
    while (fgets(line, MAXLINE + 1, fin) != NULL) // Read in name
    {
        // Remove newline character at the end of input string
        sanitise (line);
        
        name = strdup (line);
        
        // Read in value
        if (fgets(line, MAXLINE + 1, fin) == NULL)
        {
            fclose (fin);
            return FAILURE;
        }
        
        // Remove newline character at the end of input string
        sanitise (line);
        
        value = strdup (line);
        
        // Read in empty line separating name/value pairs
        if (fgets(line, MAXLINE + 1, fin) == NULL)
        {
            fclose (fin);
            return FAILURE;
        }
        
        if (llist_add (list, name, value) == FAILURE)
        {
            return FAILURE;
        }        
    }
    
    fclose (fin);
    return SUCCESS;
}

// Causes qsort to sort names in ascending order
int nameSortAsc (const void* namePtr1, const void* namePtr2)
{
    char* name1 = *((char**) namePtr1);
    char* name2 = *((char**) namePtr2);
    
    return strcmp (name1, name2);
}

// Causes qsort to sort names in descending order
int nameSortDesc (const void* namePtr1, const void* namePtr2)
{
    return nameSortAsc (namePtr2, namePtr1);
}

//
// It sorts the list according to the name. The parameter ascending determines if the
// order is ascending (1) or descending(0).
//
void llist_sort (LINKED_LIST* list, int ascending)
{
    if ((ascending != 0) && (ascending != 1)) // Invalid input!
    {
        return;
    }
    
    int i = 0; // Array index
    
    char* names[list->nElements]; // Store only the names.
    
    LINKED_LIST_ENTRY* node = (list->head)->next;
    
    // Create new list to store entries of original list, in sorted order.
    LINKED_LIST* sortedList = llist_create ();
    
    while (node != list->head)
    {
        names[i] = node->name;
        
        node = node->next;
        i ++;
    }
    
    if (ascending == 1)
    {
        qsort (names, list->nElements, sizeof (char*), nameSortAsc);
    }
    
    else
    {
        qsort (names, list->nElements, sizeof (char*), nameSortDesc);
    }
    
    // Array 'names' is now sorted.
    
    // Transfer entries from original list to new list, in sorted order
    for (i = 0; i < list->nElements; i ++)
    {
        // Reset original node pointer to point to first node (after dummy head node) in original list
        node = (list->head)->next;
        
        while (node != list->head)
        {
            if (node->name == names[i]) // Comparing addresses
            {
                // Decouple node from original list, without actually freeing any memory
                (node->previous)->next = node->next;
                (node->next)->previous = node->previous;
                
                // Attach node to (end of) new list, without actually allocating any memory
                ((sortedList->head)->previous)->next = node;
                node->previous = (sortedList->head)->previous;
                ((sortedList->head)->previous) = node;
                node->next = sortedList->head;
                
                break;
            }
            
            node = node->next; // Advance to next node in original list
        }
    }
    
    // Completely free old list->head. list still exists, so list->nElements is unaffected.
    free (list->head);
    
    /* Caller has only given us a COPY of LINKED_LIST pointer 'list', and expects that pointer to still be valid after this function is done. Thus, cannot free 'list' and cannot redirect 'list' to point to 'sortedList'. Instead, redirect 'list->head' to point to 'sortedList->head' and free 'sortedList'. */
    
    list->head = sortedList->head;
    free (sortedList);
}

//
// It removes the first entry in the list.
// All entries are moved down one position.
// It also frees memory allocated for name and value.
//
int llist_remove_first (LINKED_LIST* list) 
{
	return llist_remove_ith (list, 0);
}

//
// It removes the last entry in the list
// It also frees memory allocated for name and value.
//
int llist_remove_last (LINKED_LIST* list) 
{
	if (list->nElements > 0)
    {
        return llist_remove_ith (list, list->nElements - 1);
    }
    
    // If reached this point, no elements to remove
    return FAILURE;
}

//
// Insert a name/value pair at the beginning of the list.
// There is no check if the name already exists. The entry is added
// at the beginning of the list.
//
int llist_insert_first (LINKED_LIST* list, char* name, char* value)
{
    LINKED_LIST_ENTRY* node = malloc (sizeof (LINKED_LIST_ENTRY));
    if (node == NULL)
    {
        return FAILURE;
    }
    
    node->name = strdup (name);
    node->value = strdup (value);
    
    // Update list pointers
    node->next = (list->head)->next;
    node->previous = (list->head);
    (node->next)->previous = node;
    (node->previous)->next = node;
    
    // Update nElements
    list->nElements ++;
    
	return 0;
}

//
// Insert a name/value pair at the end of the list.
// There is no check if the name already exists. The entry is added
// at the end of the list.
//
int llist_insert_last (LINKED_LIST* list, char* name, char* value) 
{
    // Allocate memory for a new node.
    LINKED_LIST_ENTRY* node = malloc (sizeof (LINKED_LIST_ENTRY));
    if (node == NULL)
    {
        return FAILURE;
    }
    
    node->name = strdup (name);
    node->value = strdup (value);
    
    // Update list pointers
    ((list->head)->previous)->next = node;
    node->previous = (list->head)->previous;
    ((list->head)->previous) = node;
    node->next = list->head;
    
    // Update nElements only if actually adding a new element.
    list->nElements ++;
    
    return SUCCESS;
}