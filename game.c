// Daniel Porteous porteousd

#include "game.h"

// These are defined in server.c
extern int num_wins;
extern pthread_mutex_t lock;

// Put this here so we only need to build the message once.
// The function build_welcome will be called from server.c
char welcome[WELCOME_LENGTH];

// Hardcoded reject message put up here for easy access.
char reject_message[OUTGOING_MSG_LEN] = "0Sorry, max players reached.";

/* 
** Creates a new game. First we create a new instance. If thhis function returns
** NULL, it means we've hit max players so we send a reject message and return
** non-zero to the server so it can log accordingly. After this, we generate a
** (pseduo) random code if none was supplied on initial execution.
** Finally we create a thread and pass it the StateInfo to work with.
*/
int create_game(int sock_id, char *ip4, char *correct, StateInfo *state_info) {

    // The thread ID will be set by pthread_create later.
    Instance *new_game = new_instance(state_info, sock_id, ip4, (pthread_t)-1);

    if (new_game == NULL) {
        // Error message currently hardcoded prepended with 0.
        // TODO this should be more extensible.
        send(sock_id, reject_message, OUTGOING_MSG_LEN, 0);
        close(sock_id);
        return -1; // blah do more errno or something?
    }

    if (correct == NULL) {
        new_game->code = get_random_code();
    } else {
        new_game->code = correct;
    }

    // Write initial information to log upon client connection.
    char log_buf[LOG_MSG_LEN];
    sprintf(log_buf, "(%s)(%d) Client connected.\n", ip4, sock_id);
    write_log(log_buf);

    sprintf(log_buf, "(0.0.0.0) Server secret = \"%s\".\n", new_game->code);
    write_log(log_buf);

    // Create the new thread and pass in the StateInfo.
    // We will relocate the appropriate instance inside the thread.
    pthread_create(&new_game->t, NULL, run_instance, state_info);

    return 0;
}

/*
** This is the process spawned by pthread_create, into which we pass StateInfo.
** Using pthread_self, we relocate the appropriate instance and set up some
** variables for the game, log some stuff to log, send a welcome message, etc.
** We then enter the main game loop, waiting for messages from the client
** which we then handle with game_step. Once the game ends, we remove the
** appropriate instance, close the socket and print to log before finally
** exiting the thread.
*/
void *run_instance(void *param)
{
    /*
    ** Because we can only pass one thing into this function, we pass the
    ** thing with the greatest amount of useful info, the Instances struct. 
    ** We then find  again the target Instance by using the thread_id, 
    ** which we can find out using pthread_self() passed to get_instance().
    */
    StateInfo *state_info = (StateInfo*)param;
    Instance *instance = get_instance(state_info, pthread_self());
    char *correct = instance->code;

    int sock_id = instance->s;
    char *ip4 = instance->ip4;

    char log_buf[LOG_MSG_LEN];

    send_welcome(sock_id);

    // Add our own sentinel for printing purposes.
    char msg[CODE_LENGTH+1];
    
    // Main game loop. We don't have to worry about the len that recv returns
    // because we know we will get 4 chars and these are all that concern us.
    while (recv(sock_id, &msg, CODE_LENGTH, 0))
    {
        msg[CODE_LENGTH] = '\0';
        if (game_step(msg, correct, instance) == 0)
            break;
    }

    remove_instance(state_info, instance->t);
    close(sock_id);
    
    sprintf(log_buf, "(%s)(%d) Client disconnected.\n", ip4, sock_id);
    write_log(log_buf);

    pthread_exit(NULL);
}

/*
** This is the main game loop, containing most of the game logic.
** Returns 0 when done, otherwise returns 1 to indicate the game isn't done.
** Essentially, we check whether the code was valid and if so, check whether
** it was correct. If so, they win and exit accordingly. Otherwise send the
** client the appropriate feedback.
*/
int game_step(char *msg, char *correct, Instance *instance) {

    // Repeating these lines inside game_step saves us passing these 
    // variables through.
    int sock_id = instance->s;
    char *ip4 = instance->ip4;
    char log_buf[LOG_MSG_LEN];

    // Buffer for output to be returned to client.
    char outgoing[OUTGOING_MSG_LEN];

    // b represents correct position and colour.
    // m represents incorrect position but correct colour.
    int b = 0;
    int m = 0;

    // cmp_codes returns 0 when the guess was a valid code.
    // We check the b variable to determine if it was correct.
    if (cmp_codes(msg, correct, &b, &m) == 0) {
        sprintf(log_buf, "(0.0.0.0) Server hint = [%d,%d].\n", b, m);
        write_log(log_buf);

        sprintf(log_buf, "(%s)(%d) Client guess = \"%s\".\n", ip4, 
            sock_id, msg);
        write_log(log_buf);

        sprintf(outgoing, "%c[%d,%d]", ALIVE, b, m);

        // This means that the guess was correct.
        if (b == 4) {
            sprintf(log_buf, "(%s)(%d) SUCCESS Game Over.\n", ip4, sock_id);
            write_log(log_buf);

            sprintf(outgoing, "%cSuccess! You won in %d turns.", DEAD, 
                instance->turn);
            send(sock_id, outgoing, strlen(outgoing), 0);

            pthread_mutex_lock(&lock);
            num_wins += 1;
            pthread_mutex_unlock(&lock);
            return 0;
        }
        // This means that the guess was incorrect and we have reached turn 10. 
        else if (instance->turn == 10) {
            sprintf(log_buf, "(%s)(%d) FAILURE Game Over.\n", ip4, sock_id);
            write_log(log_buf);
            
            sprintf(outgoing, "%cSorry, you ran out of turns :(", DEAD);
            send(sock_id, outgoing, strlen(outgoing), 0);

            return 0;
        }
        // Getting here means the guess was incorrect but they have more turns.
        instance->turn += 1;    
    }

    // The guess was invalid, tell the client to try again. 
    else {
        sprintf(log_buf, "(%s)(%d) INVALID Client guess invalid.\n", ip4, 
            sock_id);
        sprintf(outgoing, "%cInvalid guess, try again.", ALIVE);
    }

    send(sock_id, outgoing, strlen(outgoing), 0);
    b = 0;
    m = 0;

    memset(outgoing, '\0', OUTGOING_MSG_LEN);

    return 1;
}

// Creates the welcome message to send to the client.
// We build it line by line for readability's sake.
// Only called once from inside server, as it only needs to be created once.
void build_welcome()
{
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
}

// Sends the above welcome message through the given socket.
void send_welcome(int sock_id)
{
    send(sock_id, welcome, WELCOME_LENGTH, 0);
}

// Generates a pretty decent pseudo random code.
char *get_random_code()
{
    char *code;

    code = malloc(sizeof(char) * CODE_LENGTH + 1);

    srand(time(NULL));
    for (int i=0; i < CODE_LENGTH; i++) {
        code[i] = 'A' + (rand() % 4);
    }

    code[CODE_LENGTH] = '\0';
    return code;

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

    // Making duplicates to work with.
    char guess_d[CODE_LENGTH];
    strncpy(guess_d, guess, CODE_LENGTH);

    char correct_d[CODE_LENGTH];
    strncpy(correct_d, correct, CODE_LENGTH);

    // This loop checks for validity in the guess and then correctly
    // positioned and coloured pins.
    for (i = 0; i < CODE_LENGTH; i++) {
        // Checking that the characters are in the acceptable range.
        // Are these magic numbers? Perhaps not, they're intrinsic to the game.
        if (guess_d[i] < 'A' || guess_d[i] > 'F') {
            *b = 0;
            return -1;
        }

        // Setting correctly positioned chars to 0 or 1 so we don't double up.
        if (guess_d[i] == correct_d[i]) {
            correct_d[i] = 0;
            guess_d[i] = 1;
            *b = *b + 1;
        }
    }

    // This loop checks for incorrectly positioned but correctly coloured pins.
    for (i = 0; i < CODE_LENGTH; i++) {
        for (j = 0; j < CODE_LENGTH; j++) {
            if (guess_d[i] == correct_d[j]) {
                correct_d[j] = 0;
                *m = *m + 1;
                break;
            }
        }
    }

    return 0;
}
