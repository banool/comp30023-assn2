// Daniel Porteous porteousd

/*
** The actual game?
*/

#include "game.h"

/* 
** Returns id for the thread maybe?
*/
int create_game(int sock_id, char *ip4, Instances *instances) {

    // The thread ID will be set by pthread_create later.
    Instance *new_game = new_instance(instances, sock_id, ip4, (pthread_t)-1);
    //print_instances(instances);

    if (new_game == NULL) {
        // Error message currently hardcoded prepended with 0.
        // TODO this should be more extensible.
        send(sock_id, "0Sorry, max players reached.", 28, 0);
        close(sock_id);
        return -1; // blah do more errno or something?
    }

    // Make it return 1 if this fails maybe.
    // Back in server, make a line like
    // while(create_game())
    // which will try to keep making it until it succeeds.
    pthread_create(&new_game->t, NULL, run_instance, new_game);
    // Care. The thread number that pthread_join is waiting for is not
    // the number that is assigned in pthread_create. In run_instance
    // the real thread number is assigned and this is the one that is used
    // for the pthread_join and the remove_instance.
    //pthread_join(new_game->t, NULL);
    //remove_instance(instances, new_game->t);
    //print_instances(instances);
    //printf("removing %d\n", new_game->t);

    return 0;
}

void *run_instance(Instance *instance)
{
    //instance->t = pthread_self();

    int sock_id = instance->s;
    char *ip4 = instance->ip4;

    char *correct = get_random_code();

    char log_buf[LOG_MSG_LEN];

    sprintf(log_buf, "(0.0.0.0) server secret = %s\n", correct);
    write_log(log_buf);

    // pthread_self() is the same as instance->t

    // TODO this thread number doesnt seem right for some reason?
    // printf("sock id = %d and thread id = %d\n", sock_id, instance->t);

    send_welcome(sock_id);

    // No need for +1 because we know input length therefore no sentinel.
    // TODO revise this position
    char msg[CODE_LENGTH+1]; 
    
    // todo dont forget about this len thing.
    while (recv(sock_id, &msg, CODE_LENGTH, 0))
    {
        msg[CODE_LENGTH] = '\0';
        if (game_step(msg, correct, instance) == 0)
            break;
    }
    // TODO do we ever get here?
    close(sock_id);
}

// Returns 0 when done, otherwise returns 1 to indicate the game isn't done.
int game_step(char *msg, char *correct, Instance *instance) {

    // Repeating these lines inside game_step saves us passing these 
    // variables through.
    int sock_id = instance->s;
    char *ip4 = instance->ip4;
    char log_buf[LOG_MSG_LEN];

    // Buffer for output to be returned to client.
    char outgoing[OUTGOING_MSG_LEN];

    int b = 0;
    int m = 0;

    if (cmp_codes(msg, correct, &b, &m) == 0) {
        if (b == 4) {
            sprintf(log_buf, "(%s)(%d) SUCCESS Game Over\n", ip4, sock_id);
            write_log(log_buf);

            sprintf(outgoing, "%dSuccess! You won in %d turns.", DEAD, instance->turn);
            send(sock_id, outgoing, strlen(outgoing), 0);

            //remove_instance(instance); include below line in this function
            close(sock_id);
            return 0;
        } else if (instance->turn == 10) {
            sprintf(log_buf, "(%s)(%d) FAILURE Game Over\n", ip4, sock_id);
            write_log(log_buf);
            //TODO make all these consistent. so like get rid of strcpy in place of sprintf even if it has no args.
            sprintf(outgoing, "%dSorry, you ran out of turns :(", DEAD);
            send(sock_id, outgoing, strlen(outgoing), 0);

            //remove_instance(instance); include below line in this function
            close(sock_id);
            return 0;
        } else {
            sprintf(log_buf, "(%s)(%d) client guess = %s\n", ip4, sock_id, msg);
            write_log(log_buf);

            sprintf(log_buf, "(0.0.0.0) server hint = [%d,%d]\n", b, m);
            sprintf(outgoing, "%d[%d,%d]", ALIVE, b, m);

            instance->turn += 1;
        }
    } else {
        sprintf(outgoing, "%dInvalid guess, try again.", ALIVE);
    }

    write_log(log_buf);

    send(sock_id, outgoing, strlen(outgoing), 0);
    b = 0;
    m = 0;

    memset(outgoing, '\0', OUTGOING_MSG_LEN);

    return 1;
}

void send_welcome(int sock_id)
{
    char welcome[WELCOME_LENGTH];
    welcome[0] = ALIVE;
    strcat(welcome, "Welcome to Mastermind!\n\n");
    strcat(welcome, "How to play:\n");
    strcat(welcome, "============\n");
    strcat(welcome, "Enter your guess of 4 characters from A to F. Eg. ABBD\n");
    strcat(welcome, "The server will return  in this format: [b,m]\n");
    strcat(welcome, "    b - Num. correct letters in correct position.\n");
    strcat(welcome, "    m - Num. correct letters in wrong position.\n");
    strcat(welcome, "You get 10 guesses. Good luck!\n");
    // Prints last line with two newlines, one here and one from the client.
    strcat(welcome, "\0");

    send(sock_id, welcome, WELCOME_LENGTH, 0);
}

char *get_random_code()
{
    char *code;
    char randomletter;

    code = malloc(sizeof(char) * CODE_LENGTH);

    for (int i=0; i < CODE_LENGTH; i++) {
        code[i] = get_random_letter();
    }

    return code;

}

// Thanks to the second option provided by the correct answer here:
// https://goo.gl/g0KFEM
static inline unsigned get_random_letter() {
   int l;
   srand(time(NULL));
   do { l = rand(); } while (l>=(RAND_MAX/CODE_LENGTH)*CODE_LENGTH);
   return 'A' + (unsigned)(l % CODE_LENGTH);
}

/*
** b: num of correct colours in the correct positions
** m: num of colours that are part of the code but not in the correct positions
** 
** Returns -1 if any of the letters are out of the accepted range. 0 otherwise.
** TODO this is slightly wrong still. i.e. 3,1 on ACBD vs DCBD instead of 3,0.
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
    printf("leftovers %s\n", leftover_letters);

    // Iterate throught leftover_letters, the letters that weren't
    // guessed in the correct position but could still be present.
    for (i = 0; i < CODE_LENGTH; i++) {
        // For each leftover letter, check if it is present in the guess.
        // If so, set it to Z so we don't double up.
        // We break because we don't want to check this letter anymore,
        // a match has been found and we increment m accordingly.
        for (j = 0; j < CODE_LENGTH; j++) {
            if (leftover_letters[i] == guess[j]) {
                leftover_letters[i] = 'Y';
                *m = *m + 1;
                break; // This only breaks the inner loop.
            }
        }
    }

    return 0; // String was valid (A to F) if we get here.
}


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