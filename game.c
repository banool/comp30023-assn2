// Daniel Porteous porteousd

/*
** The actual game?
*/

#include "game.h"


/*
int main(int argc, char *argv[])
{
    char hard1[4] = "ABCC"; // guess
    char hard2[4] = "BCDC"; // correct

    int b = 0;
    int m = 0;

    printf("%d\n", cmp_codes(hard1, hard2, &b, &m));
    printf("b: %d // m: %d\n", b, m);

    Threads *threads = create_threads_struct(MAX_PLAYERS);

    return 0;
}*/


/* 
** Returns id for the thread maybe?
*/
int create_game(int sock_id, Threads *threads) {
    
    int next_index = get_next_index(threads);

    if (next_index < 0) {
        send(sock_id, "Sorry, max players reached.", 28, 0);
        close(sock_id);
        return -1; // blah do more errno or something?
    }

    // Make it return 1 if this fails.
    // Back in server, make a line like
    // while(create_game())
    // which will try to keep making it until it succeeds.
    pthread_create(&threads->t[next_index], NULL, 
        work_function, sock_id);
    return 0;

}

void *work_function(int sock_id)
{

    send_welcome(sock_id);

    char correct[4] = "ABCD";
    char outgoing[30];

    int b = 0;
    int m = 0;

    // No need for +1 because we know input length therefore no sentinel.
    char msg[CODE_LENGTH]; 
    
    // todo dont forget about this len thing.
    while (recv(sock_id,&msg,CODE_LENGTH,0))
    {
        printf("%d: %s message size: %d\n", sock_id, msg, sizeof(msg));

        if (cmp_codes(msg, correct, &b, &m) < 0) {
            strncpy(outgoing, "Invalid guess, try again.", 7);
            outgoing[8] = '\0';
        } else {
            snprintf(outgoing, sizeof(outgoing), "[%d,%d]", b, m);
        }
        printf("outgoing: %s\n", outgoing);
        send(sock_id, outgoing, 8, 0);
        b = 0;
        m = 0;
    }
    close (sock_id);
}

void send_welcome(int sock_id) {
    char welcome[320];
    strcat(welcome, "Welcome to Mastermind!\n\n");
    // Prints last line with two newlines, one here and one from the client.
    strcat(welcome, "How to play:\n");
    strcat(welcome, "============\n");
    strcat(welcome, "Enter your guess of 4 characters from A to F. Eg. ABBD\n");
    strcat(welcome, "The server will return  in this format: [b,m]\n");
    strcat(welcome, "    b - Num. correct letters in correct position.\n");
    strcat(welcome, "    m - Num. correct letters in wrong position.\n");
    strcat(welcome, "You get 10 guesses. Good luck!\n");

    send(sock_id, welcome, 320, 0);
}

/*
** b: num of correct colours in the correct positions
** m: num of colours that are part of the code but not in the correct positions
** 
** Returns -1 if any of the letters are out of the accepted range. 0 otherwise.
*/
int cmp_codes(char *guess, char *correct, int *b, int *m)
{
    int i;
    int j;

    printf("A: %d, F: %d\n", 'A', 'F');

    // This array represents letters that could be correct, but in the wrong
    // spot. We take away correctly positioned letters on the first pass,
    // leaving behind letters that might be present but in the wrong position.
    char leftover_letters[CODE_LENGTH];
    strncpy(leftover_letters, correct, CODE_LENGTH);
    // First pass checking for chars in correct position.
    for (i = 0; i < CODE_LENGTH; i++) {
        // Chcecking that the characters are in the acceptable range.
        // TODO are these magic numbers???
        printf("char: %d\n", guess[i]);
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