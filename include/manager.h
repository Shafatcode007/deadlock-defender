#ifndef MANAGER_H
#define MANAGER_H

#include "ipc.h"

/*
 * Print current allocation / need / available table.
 */
void print_state(SharedData *shm);

/*
 * Main manager loop.
 * defender_mode: 1 = apply Banker's Algorithm per request
 *                0 = no checking (vulnerable mode)
 */
void run_manager(SharedData *shm, int defender_mode);

/*
 * Manual input mode: user types worker ID + request vector.
 * Banker's Algorithm applied to each request interactively.
 */
void run_manual_mode(SharedData *shm);

#endif
