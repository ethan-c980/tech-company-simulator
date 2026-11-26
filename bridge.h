
#ifndef SHARED_H
#define SHARED_H

typedef struct {
    int id;
    float score;
} Player;

void print_player(Player p);

extern int global_counter;

#endif
