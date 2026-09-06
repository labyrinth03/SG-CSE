#include <stdio.h>
#include <string.h>

int pmatch_all(char *string, char *pat, int* failure)
{
/* Knuth, Morris, Pratt string matching algorithm */
    int i = 0, j = 0;                               // intialize i, j
    int lens = strlen(string);                      // save length of string to lens
    int lenp = strlen(pat);                         // save length of pattern to lenp
    while (i < lens && j < lenp) {                  // while i, j is in bound of each string
        if (string[i] == pat[j]) {                  // if ith element of string & jth element of pattern is same
            i++; j++; }                             // plus 1 to i, j
        else if (j == 0) i++;                       // if j is 0, plus one to i 
        else j = failure[j-1] + 1;                  // else, save plus 1 of failure's j-1 element

        if(j == lenp){                              // if string where equal to pattern found
            printf("%d\n", i - lenp);               // print first index of that string.
            i = i - lenp + 1;                       // then set string index(i) to next of that string
            j = j - lenp;                           // save j to 0
            if(j > lenp){                           // if j is greater than length of p
                j = 0;                              // set j to 0
            }
        }

        
    }
    return ( (j == lenp) ? (i - lenp) : -1);        // if there's pattern, return it's first index, or return -1
}

void fail(char *pat, int *failure)
{
/* compute the pattern’s failure function */
    int i, n = strlen(pat);                         // save pattern's length to i, n
    failure[0] = -1;                                // first element is -1
    for (int j = 1; j < n; j++) {                   // for loop to repeat pattern's length times
        i = failure[j-1];                           // save previous failure value to i
        while ((pat[j] != pat[i+1]) && (i >= 0))    // while current pattern value is not same with i+1's and i is positive 
            i = failure[i];                         // save failure[i] to i
        if (pat[j] == pat[i+1])                     // if current pattern value is same with i+1's
            failure[j] = i+1;                       // current failure value is set to i+1
        else failure[j] = -1;                       // if it's different, set current failure value to -1
    }
}

int main(){
    int failure[30];                    // array to save failure
    char string[30];                    // array to save string
    char pat[30];                       // array to save pattern
    scanf("%s", string);                // save string
    scanf("%s", pat);                   // save pattern
    fail(pat, failure);                 // call failure function
    pmatch_all(string, pat, failure);   // call pmatch
    return 0;
}