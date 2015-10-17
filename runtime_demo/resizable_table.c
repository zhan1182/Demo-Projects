#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "resizable_table.h"

#define MAXLINE 512
#define SUCCESS 1
#define FAILURE 0
#define NEWLINE '\n'
#define TERMINATING_NULL_BYTE '\0'
#define READ_MODE "r"
#define WRITE_MODE "w"

int rtable_lookup_index (RESIZABLE_TABLE* table, char* name);

//
// It returns a new RESIZABLE_TABLE. It allocates it dynamically,
// and initialises the values. The initial maximum size of the array is 10.
//
RESIZABLE_TABLE* rtable_create() 
{
    // Allocate a RESIZABLE_TABLE
	RESIZABLE_TABLE* table = malloc (sizeof (RESIZABLE_TABLE));
    if (table == NULL) 
    {
		return NULL;
	}
	
	// Initialise the members of RESIZABLE_TABLE
	table->maxElements = INITIAL_SIZE_RESIZABLE_TABLE;
	table->currentElements = 0;
	
    table->array = malloc ((table->maxElements) * sizeof (RESIZABLE_TABLE_ENTRY));
	if ((table->array) == NULL) 
    {
		return NULL;
	}
	
	return table;
}

//
// It prints the elements in the array assuming the value is a string in the form:
//
//======== Table =======
//currentElements=2 maxElements=10
//0: "George"       "23 Oak St, West Lafayette, 47906"
//1: "Peter"        "27 Oak St, West Lafayette, 47906"
//======== End Table =======
//
void rtable_print_str (RESIZABLE_TABLE* table)
{
	int i = 0;

	printf("\n======== Table =======\n");
	printf("currentElements=%d maxElements=%d\n", table->currentElements, table->maxElements);

	for (i = 0; i < (table->currentElements); i ++) 
    {
		printf("%d: \"%s\" \"%s\"\n", i, table->array[i].name, (char*) (table->array[i].value));
	}
    
	printf("======== End Table =======\n");
	return;
}

//
// It prints the elements in the array assuming the value is an int:
//
//======== Table =======
//currentElements=2 maxElements=10
//0: "George"               101
//1: "Peter"                245
//======== End Table =======
//
void rtable_print_int (RESIZABLE_TABLE* table)
{
	int i = 0;

	printf("\n======== Table =======\n");
	printf("currentElements=%d maxElements=%d\n", table->currentElements, table->maxElements);
	
    for (i = 0; i < (table->currentElements); i ++) 
    {
		printf("%d: \"%s\" %ld\n", i, table->array[i].name, (long) (table->array[i].value));
	}
	
    printf("======== End Table =======\n");
	return;
}

/* Allocates a new array (NOT table!) with double the original size, copies over the original elements and frees the old array.
 */
void reallocate (RESIZABLE_TABLE* table)
{
    int i; // Loop index
    
    // Allocate new array
    RESIZABLE_TABLE_ENTRY* newArray = malloc (((table->maxElements) * 2) * sizeof (RESIZABLE_TABLE_ENTRY));
    if (newArray == NULL) 
    {
		return;
	}
    
    // Copy original elements into new array
    for (i = 0; i < (table->currentElements); i ++)
    {
        newArray[i].name = table->array[i].name;
        newArray[i].value = table->array[i].value;
    }
    
	// Free original array
    free (table->array);
    
    // Redirect table->array to point to new array
    table->array = newArray;
    
    // Update maxElements.
    (table->maxElements) *= 2;
    
    // NOTE: Since neither the table structure nor the number of elements currently in the structure have changed, currentElements does not need to be updated.
}

//
// Adds one pair name/value to the table. If the name already exists it will
// substitute its value. Otherwise, it will store name/value in a new entry.
// If the new entry does not fit, it will double the size of the array.
// The name string is duplicated with strdup() before assigning it to the
// table.
//
int rtable_add (RESIZABLE_TABLE* table, char* name, void* value) 
{
	// Find if it is already there and substitute value
    
    int nameIndex = rtable_lookup_index (table, name);
    
    if (nameIndex != -1) // Name and value already exist
    {
        // Assuming preexisting value does not need to be freed
        table->array[nameIndex].value = value;
        
        return !SUCCESS;
    }
    
	// If we are here, it is because the entry was not found.
    return !(rtable_insert_last (table, name, value));
}

//
// Add name and value into table where value is a string (char *)
// Implement on top of rtable_add.
//
int rtable_add_str (RESIZABLE_TABLE* table, char* name, char* str_value)
{
	return rtable_add (table, name, (void*) strdup(str_value));
}

//
// Add name and value into table where value is a long
// Implement on top of rtable_add.
//
int rtable_add_int (RESIZABLE_TABLE* table, char* name, long int_value)
{
	return rtable_add (table, name, (void*) int_value );
}

//
// Returns the value that corresponds to the name or NULL if the
// name does not exist in the table.
//
void* rtable_lookup (RESIZABLE_TABLE* table, char* name) 
{
    int i; // Loop index
   
    for (i = 0; i < (table->currentElements); i ++)
    {
        if ((strcmp (table->array[i].name, name)) == 0)
        {
            return (void*) table->array[i].value;
        }
    }

    // If reached this point, name does not exist in the table.
    return NULL;
}

/* Returns the index that corresponds to the name or -1 if the
name does not exist in the table. */
int rtable_lookup_index (RESIZABLE_TABLE* table, char* name) 
{
    int i; // Loop index
   
    for (i = 0; i <= (table->currentElements); i ++)
    {
        if ((strcmp (table->array[i].name, name)) == 0)
        {
            return i;
        }
    }

    // If reached this point, name does not exist in the table.
    return -1;
}

//
// It removes the entry with that name from the table. The entries after the entry
// removed will shift upwards. Also the name and value strings will be freed.
//
int rtable_remove (RESIZABLE_TABLE* table, char* name) 
{
	int nameIndex = rtable_lookup_index (table, name);
    
    if (nameIndex != -1) // Name and value already exist
    {
        if ((rtable_remove_ith (table, nameIndex)) == SUCCESS)
        {
            return SUCCESS;
        }
        
        return FAILURE;
    }
    
    // If reached this point, name does not exist in the table -- cannot remove it.
    return FAILURE;
}

//
// It returns in *name and *value the name and value that correspond to
// the ith entry. It will return 1 if successful, or 0 otherwise.
//
int rtable_get_ith (RESIZABLE_TABLE* table, int ith, char** name, void** value)
{
    if (ith >= (table->maxElements)) // Index is out of bounds
    {
        return FAILURE;
    }
    
    *name = table->array[ith].name;
    *value = table->array[ith].value;
    
	return SUCCESS;
}

//
// It removes the ith entry from the table. The entries after the entry removed are
// moved upwards to use the empty space. Also the name/value strings are freed.
//
int rtable_remove_ith (RESIZABLE_TABLE* table, int ith)
{
    int i; // Loop index
    
    if (ith >= (table->currentElements)) // Index does not refer to a valid entry
    {
        return FAILURE;
    }

    // Free preexisting name and value
    free (table->array[ith].name);
    free (table->array[ith].value);
    
    // Shift subsequent entries upwards.
    for (i = ith; i <= ((table->currentElements) - 1); i ++)
    {
        table->array[i].name = table->array[i+1].name;
        table->array[i].value = table->array[i+1].value;
    }
    
    // Update currentElements -- this way, caller won't attempt to access duplicate last entry.
    (table->currentElements) --;
    
	return SUCCESS;
}

//
// It returns the number of elements in the table.
//
int rtable_number_elements (RESIZABLE_TABLE* table)
{
	return table->currentElements;
}

//
// It returns the maximum number of elements in the table
//
int rtable_max_elements (RESIZABLE_TABLE* table)
{
	return table->maxElements;
}

//
// It saves the table in a file called file_name with values as char *. The format of the
// file is as follows:
//
// name1\n
// str1\n
// \n
// name2\n
// str2\n
// ...
//
// Notice that there is an empty line between each name/value pair.
//
int rtable_save_str (RESIZABLE_TABLE* table, char* file_name)
{
    int i; // Loop index
    
    FILE* fout = fopen (file_name, WRITE_MODE);
    if (fout == NULL) // fopen failed
    {
        return FAILURE;
    }
    
    for (i = 0; i < (table->currentElements); i ++)
    {
        fprintf (fout, "%s\n", table->array[i].name);
        fprintf (fout, "%s\n\n", (char*) (table->array[i].value));
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
// It reads the table from the file_name indicated, assuming that the values are
// char *. If the table already has entries, it will clear the entries.
//
int rtable_read_str (RESIZABLE_TABLE* table, char* file_name)
{
    int i; // Array index
    char line[MAXLINE + 1]; // Temporary buffer to store name/value read in from file
    
    FILE* fin = fopen (file_name, READ_MODE);
    if (fin == NULL) // fopen failed
    {
        return FAILURE;
    }
    
    if ((table->currentElements) > 0) // Table already has elements
    {
        for (i = 0; i < (table->currentElements); i ++)
        {
            rtable_remove_ith (table, i);
        }
    }
    
    // Reset array index
    i = 0;
    
    while (fgets(line, MAXLINE + 1, fin) != NULL) // Read in name
    {
        // Remove newline character at the end of input string
        sanitise (line);
    
        table->array[i].name = strdup (line);
        
        /* fgets preferred over fscanf since fscanf stops at whitespace unless expressly anticipated in the format string, unlike fgets, which will be an issue when reading in values with possible spaces. */
        
        // Read in value
        if (fgets(line, MAXLINE + 1, fin) == NULL)
        {
            fclose (fin);
            return FAILURE;
        }
        
        // Remove newline character at the end of input string
        sanitise (line);
        
        table->array[i].value = (void*) strdup (line);
        
        // Read in empty line separating name/value pairs
        if (fgets(line, MAXLINE + 1, fin) == NULL)
        {
            fclose (fin);
            return FAILURE;
        }
        
        /* At this point, name, value and empty line separator were successfully read in. Only at this point do we update currentElements. Thus, even if stored garbage name and/or value, currentElements won't update to reflect it and caller won't attempt to access it. */
        (table->currentElements) ++;
        
        // Make sure that there is enough space
    
        if ((table->currentElements) == (table->maxElements)) // Run out of space
        {
            // Allocate more memory
            reallocate (table);
        }
        
        i ++;
    }
    
    fclose (fin);
	return SUCCESS;
}

//
// It saves the table in a file called file_name with values as int. The format of the
// file is as follows:
//
// name1\n
// int1\n
// \n
// name2\n
// int2\n
// ...
//
// Notice that there is an empty line between each name/value pair.
//
int rtable_save_int (RESIZABLE_TABLE* table, char* file_name) 
{
    int i; // Loop index
    
    FILE* fout = fopen (file_name, WRITE_MODE);
    if (fout == NULL) // fopen failed
    {
        return FAILURE;
    }
    
    for (i = 0; i < (table->currentElements); i ++)
    {
        fprintf (fout, "%s\n", table->array[i].name);
        fprintf (fout, "%ld\n\n", (long) (table->array[i].value));
    }
    
    fclose (fout);
    
	return SUCCESS;
}

//
// It reads the table from the file_name indicated assuming that the values are
// int. If the table already has entries, it will clear the entries.
//
int rtable_read_int (RESIZABLE_TABLE* table, char* file_name) 
{
	int i; // Array index
    char line[MAXLINE + 1]; // Temporary buffer to store name read in from file
    long value; // Temporarily stores value read in from file
    
    FILE* fin = fopen (file_name, READ_MODE);
    if (fin == NULL) // fopen failed
    {
        return FAILURE;
    }
    
    if ((table->currentElements) > 0) // Table already has elements
    {
        for (i = 0; i < (table->currentElements); i ++)
        {
            // Assuming preexisting values are strings.
            rtable_remove_ith (table, i);
        }
    }
    
    // Reset array index
    i = 0;
    
    while (fgets(line, MAXLINE + 1, fin) != NULL) // Read in name
    {
        // Remove newline character at the end of input string
        sanitise (line);
    
        table->array[i].name = strdup (line);
        
        /* fgets preferred over fscanf since fscanf stops at whitespace unless expressly anticipated in the format string, unlike fgets, which will be an issue when reading in values with possible spaces. */
        
        // Read in value. \n at end of fscanf format string matches ALL whitespace, including extra \n between name/value pairs.
        if (fscanf(fin, "%ld\n", &value) != 1)
        {
            fclose (fin);
            return FAILURE;
        }
        
        table->array[i].value = (void*) value;
        
        /* At this point, name, value and empty line separator were successfully read in. Only at this point do we update currentElements. Thus, even if stored garbage name and/or value, currentElements won't update to reflect it and caller won't attempt to access it. */
        (table->currentElements) ++;
        
        // Make sure that there is enough space
    
        if ((table->currentElements) == (table->maxElements)) // Run out of space
        {
            // Allocate more memory
            reallocate (table);
        }
        
        i ++;
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
// It sorts the array according to the name. The parameter 'ascending' determines if the
// order is ascending (1) or descending(0).
//
void rtable_sort (RESIZABLE_TABLE* table, int ascending)
{
    if ((ascending != 0) && (ascending != 1)) // Invalid input!
    {
        return;
    }
    
    int i; // Array index
    
    // Create an array consisting of only the names stored in the array and an array consisting of only the values stored in the array, in their original order. 
    
    char* names[table->currentElements];
    void* values[table->currentElements];
    
    for (i = 0; i < table->currentElements; i ++)
    {
        names[i] = table->array[i].name; // Copy only the address
        values[i] = table->array[i].value; // Copy only the address, if value is a string
    }
    
    if (ascending == 1)
    {
        qsort (names, table->currentElements, sizeof (char*), nameSortAsc);
    }
    
    else
    {
        qsort (names, table->currentElements, sizeof (char*), nameSortDesc);
    }
    
    // Array 'names' is now sorted.
    
    // First transfer corresponding values from 'values' to 'table->array'
    for (i = 0; i < table->currentElements; i ++)
    {
        
        table->array[i].value = values[rtable_lookup_index (table, names[i])]; // Copy only the address, if value is a string
        
        // Cannot transfer names till all values are transferred, to avoid affecting the results of rtable_lookup_index.
    }
    
    // Transfer sorted names from 'names' to 'table->array'
    for (i = 0; i < table->currentElements; i ++)
    {   
        table->array[i].name = names[i]; // Copy only the address
    }
}

// Causes qsort to sort integer values in ascending order
int longValSortAsc (const void* valPtr1, const void* valPtr2)
{
    long val1 = *((long*) valPtr1);
    long val2 = *((long*) valPtr2);
    
    return (val1 - val2);
}

// Causes qsort to sort integer values in descending order
int longValSortDesc (const void* valPtr1, const void* valPtr2)
{
    return longValSortAsc (valPtr2, valPtr1);
}

//
// It sorts the array according to the value. The parameter 'ascending' determines if the
// order is ascending (1) or descending(0).
//
void rtable_sort_by_intval (RESIZABLE_TABLE* table, int ascending)
{
    if ((ascending != 0) && (ascending != 1))
    {
        // Invalid input!
        return;
    }
    
    int i, j; // Array indices
    
    // Create an array consisting of only the names stored in the array and an array consisting of only the values stored in the array, in their original order. 
    
    char* names[table->currentElements];
    void* values[table->currentElements];
    
    for (i = 0; i < table->currentElements; i ++)
    {
        names[i] = table->array[i].name; // Copy only the address
        values[i] = table->array[i].value; // Copy only the address, if value is a string
    }
    
    if (ascending == 1)
    {
        qsort (values, table->currentElements, sizeof (long), longValSortAsc);
    }
    
    else
    {
        qsort (values, table->currentElements, sizeof (long), longValSortDesc);
    }
    
    // Array 'values' is now sorted.
    
    // First transfer corresponding names
    for (i = 0; i < table->currentElements; i ++)
    {
        for (j = 0; j < table->currentElements; j ++)
        {
            if (values[i] == table->array[j].value)
            {
                table->array[i].name = names[j];
                
                /* In case multiple names have the same value (if value is not a string), need to change value so that it will not be matched again. To reduce chances of accidentally matching something else, replace it with something that will probably NOT match any of the other values -- here, I replace value with address of the corresponding name. */
                table->array[j].value = table->array[j].name;
                
                // Do NOT keep looking after you have found a match
                break;
            }
        }
        
        // Cannot transfer values till all names are transferred, to avoid affecting the results of value lookup.
    }
    
    // Transfer sorted values from 'values' into 'table->array'
    for (i = 0; i < table->currentElements; i ++)
    {   
        table->array[i].value = values[i]; // Copy only the address
    }
}

//
// It removes the first entry in the table.
// All entries are moved down one position.
// It also frees memory allocated for name and value.
//
int rtable_remove_first (RESIZABLE_TABLE* table) 
{
	return rtable_remove_ith (table, 0);
}

//
// It removes the last entry in the table.
// It also frees memory allocated for name and value.
//
int rtable_remove_last (RESIZABLE_TABLE* table) 
{

    if (table->currentElements > 0)
    {
        return rtable_remove_ith (table, table->currentElements - 1);
    }
	
    // If reached this point, no elements to remove
    return FAILURE;
    
}

//
// Insert a name/value pair at the beginning of the table.
// The entries are moved one position downwards.
// There is no check if the name already exists. The entry is added
// at the beginning of the table.
//
int rtable_insert_first (RESIZABLE_TABLE* table, char* name, void* value) 
{
    int i; // Loop index

    // Make sure that there is enough space
    
    if ((table->currentElements) == (table->maxElements)) // Run out of space
    {
        // Allocate more memory
        reallocate (table);
    }
    
    // Shift all entries, after the first, downwards.
    for (i = (table->currentElements); i >= 0 ; i --)
    {
        table->array[i].name = table->array[i-1].name;
        table->array[i].value = table->array[i-1].value;
    }
    
    // We need to use strdup to create a copy of the name but not value.
	table->array[0].name = name;
    table->array[0].value = value;
    
    // Update currentElements
    (table->currentElements) ++;
    
    return SUCCESS;
}

//
// Insert a name/value pair at the end of the table.
// There is no check if the name already exists. The entry is added
// at the end of the table.

int rtable_insert_last (RESIZABLE_TABLE* table, char* name, void* value)
{
    // Make sure that there is enough space
    
    if ((table->currentElements) == (table->maxElements)) // Run out of space
    {
        // Allocate more memory
        reallocate (table);
    }

    /* Add name and value to a new entry. We need to use strdup to create a copy 
    of the name but not value. Assuming preexisting name and value do not need to be freed. */
	//table->array[table->currentElements].name = strdup (name);
	table->array[table->currentElements].name = name;
    table->array[table->currentElements].value = value;
    
    // Update currentElements
    (table->currentElements) ++;
    
	return SUCCESS;
}