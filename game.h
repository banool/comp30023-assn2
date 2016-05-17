// Daniel Porteous porteousd

#include <sys/socket.h>

#include "instances.h"
#include "logging.h"

#define CODE_LENGTH 4
#define MAX_PLAYERS 20
#define OUTGOING_MSG_LEN 32
#define WELCOME_LENGTH 320
#define ALIVE 1
#define DEAD 0

int cmp_codes(char *guess, char *correct, int *b, int *m);
int create_game(int sock_id, char *ip4, char *correct, StateInfo *state_info);
void *run_instance(void *param);
int game_step(char *msg, char *correct, Instance *instance);
void send_welcome(int sock_id);
char *get_random_code();
