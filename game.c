// Daniel Porteous porteousd

/*
** The actual game?
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CODE_LENGTH 4

int cmp_codes(char *guess, char *correct, int *b, int *m);

int main(int argc, char *argv[])
{
    char hard1[4] = "ABCC"; // guess
    char hard2[4] = "BCDC"; // correct

    int b = 0;
    int m = 0;

    printf("%d\n", cmp_codes(hard1, hard2, &b, &m));
    printf("b: %d // m: %d\n", b, m);

    return 0;
}

/*
** b: num of correct colours in the correct positions
** m: num of colours that are part of the code but not in the correct positions
** 
** Returns -1 if any of the letters are out of the accepted range. 0 otherwise.ls
*/
int cmp_codes(char *guess, char *correct, int *b, int *m)
{
    int i;
    int j;

    // This array represents letters that could be correct, but in the wrong
    // spot. We take away correctly positioned letters on the first pass,
    // leaving behind letters that might be present but in the wrong position.
    char leftover_letters[CODE_LENGTH];
    strncpy(leftover_letters, correct, CODE_LENGTH);

    // First pass checking for chars in correct position.
    for (i = 0; i < CODE_LENGTH; i++) {
        // Chcecking that the characters are in the acceptable range.
        // TODO are these magic numbers???
        if (guess[i] < 'A' || guess[i] > 'F') {
            *b = 0;
            return -1;
        }

        // Setting correctly positioned chars to Z so we don't double up.
        if (guess[i] == correct[i]) {
            leftover_letters[i] = 'Z'; // Should I be using something else? \0?
            *b = *b + 1;
        }
    }

    // Iterate throught leftover_letters, the letters that weren't
    // guessed in the correct position but could still be present.
    for (i = 0; i < CODE_LENGTH; i++) {
        // For each leftover letter, check if it is present in the guess.
        // If so, set it to Z so we don't double up.
        // We break because we don't want to check this letter anymore,
        // a match has been found and we increment m accordingly.
        for (j = 0; j < CODE_LENGTH; j++) {
            if (leftover_letters[i] == guess[j]) {
                leftover_letters[i] = 'Z';
                *m = *m + 1;
                break; // This only breaks the inner loop.
            }
        }
    }

    return 0; // String was valid (A to F) if we get here.
}