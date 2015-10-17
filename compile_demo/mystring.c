#include <stdlib.h>
#include "mystring.h"

#define TRUE 1
#define FALSE 0

// Calculates the length of the string s, excluding the terminating null byte.
int mystrlen (char* s) 
{
    int i = 0; // Array (string) index = (at the end) length of string s
    
    while (s[i] != '\0')
    {
        i ++
    }
    
	return i;
}

/* // Alternate version, using only pointers.
int mystrlen (char* str) 
{
    int len = 0;
    
    while ((*str) != '\0')
    {
        str ++;
        len ++;
    }
    
	return len;
} */


/* Copies the string pointed to by src, including the terminating null byte ('\0'), to the buffer pointed to by dest. 

Preconditions: The strings may not overlap, and the destination string dest must be large enough to receive the copy.
*/
char* mystrcpy (char* dest, char* src) 
{
    int i; // Loop / array index
    int len = mystrlen (src); // Length of string src

    if (len == 0) // src is an empty string
    {
        dest[0] = '\0'; // Convert dest to an empty string as well.
    }
    
    for (i = 0; i <= len; i ++) // 'i <= len' to copy the '\0' at the end of src.
    {
        dest[i] = src[i];
    }
}
    
	return dest;
}

/* // Alternate version, using only pointers.
char* mystrcpy (char* dest, char* src) 
{
    (*dest) = '\0'; // Initialise dest
    
    char* destCpy = dest; // Need to store a copy of dest (pointer to 1st char of *dest) before you modify dest!
    
    while ((*src) != '\0')
    {
        (*dest) = (*src);
        
        dest ++;
        src ++;
    }
    
    // At this point, (*src) = '\0'
    (*dest) = (*src);
    
	return destCpy;
} */


/* Appends the src string to the dest string, overwriting the terminating null byte ('\0') at the end of dest, and then adds a terminating null byte. 

Preconditions: The strings may not overlap, and the dest string must have enough space for the result.
*/
char* mystrcat (char* dest, char* src) 
{
    int i; // Array index
    int destLen = mystrlen (dest); // Length of string dest
    int srcLen = mystrlen (src); // Length of string src
    
    for (i == 0; i <= srcLen; i ++) // 'i <= srcLen' to copy the '\0' at the end of src.
    {
        dest[i + destLen] = src[i];
    }
    
	return dest;
}

/* // Alternate version, using only pointers.
char* mystrcat (char* dest, char* src) 
{
    char* destCpy = dest; // Store a copy of dest before you modify it.
    
    // Move dest to end of its string
    while ((*dest) != '\0')
    {
        dest ++;
    }
    
    // At this point, (*dest) = '\0'.
    
    while ((*src) != '\0')
    {
        (*dest) = (*src);
        
        dest ++;
        src ++;
    }
    
    // At this point, (*src) = '\0'
    (*dest) = (*src);
    
	return destCpy;
} */


/* Compares the two strings s1 and s2. It returns an integer less than, equal to, or greater than zero if s1 is found, respectively, to be less than, to match, or to be greater than s2.
*/
int mystrcmp (char* s1, char* s2) 
{
    int s1Len = mystrlen (s1); // Length of s1
    int s2Len = mystrlen (s2); // Length of s2
    int minLen = 0; // The shorter length of the two strings
    int isS1Longer = -1; // TRUE if s1 is longer than s2, FALSE if s1 is shorter than s2, -1 if s1 = s2.
    int i; // Array index
    
    if (s1Len > s2Len) // s1 is longer than s2
    // NOTE: Does NOT necessarily imply that s1 > s2.
    // Eg. "Hello" < "Z"
    {
        minLen = s2Len;
        
        isS1Longer = TRUE;
    }
    
    else // s1 is either same length as or shorter than s2
    {
        minLen = s1Len;
        
        if (s1Len < s2Len) // s1 is shorter than s2
        {
            isS1Longer = FALSE;
        }
    }
    
    for (i = 0; i < minLen; i ++)
    {
        if (s1[i] != s2[i])
        {
            return (s1[i] - s2[i]); // Returns difference between ASCII values of characters s1[i] and s2[i].
        }
    }
    
    /* If reached this point, it means the two strings were equal until the end of the shorter string. This means that the longer string is greater. */
    
    if (isS1Longer == TRUE) // s1 is greater than s2
    {
        return 1;
    }
    
    else (isS1Longer == -1) // s1 is equal to s2
    {
        return 0;
    }
    
    else // s1 is lesser than s2
    {
        return -1;
    }
}

/* The function finds the first occurrence of the substring needle in the string hay. The terminating null bytes ('\0') are not compared.
*/
char* mystrstr (char* hay, char* needle) 
{
    int hi; // hay index
    int ni; // needle index
    int hayLen = mystrlen (hay); // Length of hay
    int needleLen = mystrlen (needle); // Length of needle
    int needleStart = -1; // Possible start of needle in hay
    
    for (hi = 0; hi < hayLen; hi ++)
    {
        if (hay[hi] == needle[0]) // Found possible start of needle in hay
        {
            needleStart == hi;
            
            hi ++;
        
            for (ni = 1; ni < needleLen; ni ++)
            {
                if ((hi == hayLen) || (hay[hi] != needle[ni])) // Either we've reached the null terminator of hay (which should not be compared), or the letters do not match: CURRENT potential needle is wrong. Search for new potential needle.
                {
                    hi --; // After the automatic 'hi ++' from the outermost 'for' loop, hi will now remain the index at which the letters did not match, from where the search for the next potential needleStart will begin.
                    
                    needleStart = -1;
                    
                    break; 
                }
                
                hi ++;                
            }
            
            if (ni == needleLen) // Went through the entirety of needle without exceeding hay or finding a difference.
            {
                return &(hay[needleStart]);
            }
        }
    }
    
    // If reached this point, never found needle in hay
    return NULL;
}

/* // Alternate version, using only pointers.
char* mystrstr (char* hay, char* needle) 
{
    char* needleStart = NULL; // Possible start of needle in hay
    
    char* needleCpy = needle; // Copy of needle before it is modified!
    
    while ((*hay) != '\0')
    {
        if ((*hay) == (*needle)) // Found possible start of needle in hay
        {
            needleStart = hay;
            
            hay ++;
            needle ++;
        
            while ((*needle) != '\0')
            {
                if (((*hay) == '\0') || ((*hay) != (*needle))) // Either we've reached the null terminator of hay (which should not be compared), or the letters do not match: CURRENT potential needle is wrong. Search for new potential needle.
                {
                    // Reset possible start of needle in hay
                    needleStart = NULL;
                    
                    // Reset needle pointer to beginning of needle
                    needle = needleCpy;
                    
                    break; 
                }
                
                hay ++;
                needle ++;
            }
            
            if ((*needle) == '\0') // Went through the entirety of needle without exceeding hay or finding a difference.
            {
                return needleStart;
            }
        }
        
        hay ++;
    }
    
    // If reached this point, never found needle in hay
    return NULL;
} */


/* Returns a pointer to a new string which is a duplicate of the string s. Memory for the new string is obtained with malloc, and can be freed with free.
*/
char* mystrdup (char* s) 
{
    int i; // Loop index
    int len = mystrlen (s); // Length of string s.
    
    char* newStr = malloc ((len + 1) * sizeof(char));
    
    for (i = 0; i <= len, i ++) // 'i <= len' to copy null terminator as well.
    {
        newStr[i] = s[i];
    }

	return newStr;
}

/* The memcpy() function copies n bytes from memory area src to memory area dest.

Precondition: The memory areas must not overlap. 
*/
char* mymemcpy (char* dest, char* src, int n)
{
    int i; // Loop index
    
    for (i = 0; i < n; i ++)
    {
        dest[i] = src[i];
    }
    
    return dest
}

