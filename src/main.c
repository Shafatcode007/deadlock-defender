#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "ipc.h"
#include "worker.h"
#include "manager.h"

static void print_banner(void) {
    printf("╔══════════════════════════════════════════════╗\n");
    printf("║      DEADLOCK DEFENDER — IPC ENGINE          ║\n");
    printf("║  Language : C  |  IPC: shmget + semaphores  ║\n");
    printf("╚══════════════════════════════════════════════╝\n\n");
}

static int show_menu(void) {
    int choice;
    printf("Select mode:\n");
    printf("  1) Vulnerable Mode  (no deadlock prevention)\n");
    printf("  2) Defender Mode    (Banker's Algorithm ON)\n");
    printf("  3) Manual Input Mode (interactive requests)\n");
    printf("Choice: ");
    if (scanf("%d", &choice) != 1) return -1;
    return choice;
}

int main(int argc, char *argv[]) {
    (void)argc; (void)argv; /* unused — menu replaces CLI flags */

    print_banner();

    int choice = show_menu();
    while (getchar() != '\n'); /* flush leftover newline from stdin */
    if (choice < 1 || choice > 3) {
        printf("Invalid choice. Exiting.\n");
        return 1;
    }

    /* initialise shared memory and semaphores */
    SharedData *shm = ipc_init();

    if (choice == 3) {
        /* manual mode: no worker processes — user drives all requests */
        run_manual_mode(shm);
        ipc_cleanup(shm);
        printf("\n[Main] Program exited cleanly.\n");
        return 0;
    }

    int defender_mode = (choice == 2) ? 1 : 0;

    printf("\n[Main] Starting %s mode...\n\n",
           defender_mode ? "DEFENDER" : "VULNERABLE");

    pid_t pids[NUM_WORKERS];

    /* fork one child per worker */
    for (int i = 0; i < NUM_WORKERS; i++) {
        pids[i] = fork();
        if (pids[i] < 0) {
            perror("fork");
            exit(1);
        }
        if (pids[i] == 0) {
            /* child: become worker, never returns */
            run_worker(i + 1, shm, defender_mode);
        }
    }

    /* parent becomes the manager */
    run_manager(shm, defender_mode);

    /* wait for all worker children to exit */
    for (int i = 0; i < NUM_WORKERS; i++)
        waitpid(pids[i], NULL, 0);

    /* detach shared memory and destroy semaphores */
    ipc_cleanup(shm);

    printf("\n[Main] Program exited cleanly.\n");
    return 0;
}
