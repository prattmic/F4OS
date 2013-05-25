#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "shell.h"
#include "app.h"

extern command_t _valid_commands;
extern command_t _end_commands;

command_t *valid_commands = (command_t *)&_valid_commands;

#define NUM_COMMANDS ((int)(&_end_commands - &_valid_commands))

static char *cmd_hist[SHELL_HISTORY]; // Command history buffer
static int   cmd_index;               // Next buffer index to fill

static void free_argv(int argc, char ***argv);
static void parse_command(char *command, int *argc, char ***argv);
static void run_command(char *command, int argc, char **argv);

void shell(void) {
    int n = -1;
    int argc;
    char **argv;
    char *command;

    // Allocate enough command buffers for history
    for (int i = 0; i < SHELL_HISTORY; i++) {
        cmd_hist[i] = malloc(SHELL_BUF_MAX+1);
        if (cmd_hist[i] == NULL) {
            printf("Shell unable to allocate buffer for command %d. You are on"
                    "your own. Goodbye.\r\n", i);
            return;
        }
    }
    cmd_index = 0;
    command = cmd_hist[cmd_index];

    /* Wait for stdin/stdout - getc returns a negative error when it is not connected. */
    while (getc() < 0);

    printf("%s", SHELL_PROMPT);

    while (1) {
        while ((command[++n] = getc()) != '\n' && command[n] != '\r' && n < (SHELL_BUF_MAX-1)) {
            if (command[n] == '\b' || command[n] == 0x7F) {
                if (n) {
                    puts(BACKSPACE);
                    n = n - 2;
                }
                else {
                    n--;
                }
            }
            else {
                putc(command[n]);
            }
        }

        if (command[n] == '\n' || command[n] == '\r') {
            command[n] = '\0';
        }
        else {
            command[n+1] = '\0';
        }

        printf("\r\n");

        if (n >= (SHELL_BUF_MAX-1)) {
            printf("Line too long, max length is %d.\r\n%s", SHELL_BUF_MAX, SHELL_PROMPT);
            n = -1;
            continue;
        }

        parse_command(command, &argc, &argv);
        if (argc < 0) {
            /* Something bad happened, argv has already been freed */
            printf("%s: could not parse input\r\n", command);
        }
        else {
            run_command(command, argc, argv);
            free_argv(argc, &argv);
        }

        // Insert command into history, unless blank
        if (argc > 0) {
            cmd_index = (cmd_index + 1) % SHELL_HISTORY;
            command = cmd_hist[cmd_index];
        }

        printf("%s", SHELL_PROMPT);
        n = -1;
    }

    for (int i = 0; i < SHELL_HISTORY; i++) {
        free(cmd_hist[i]);
    }
}

void free_argv(int argc, char ***argv) {
    while (argc) {
        free((*argv)[argc-1]);
        argc--;
    }
    free(*argv);
}

void parse_command(char *command, int *argc, char ***argv) {
    char *begin = command;
    uint32_t n = 0;
    *argc = 0;

    /* skip leading whitespace */
    while (*command == ' ' || *command == '\t') {
        if (!*command) {
            break;
        }
        command++;
    }

    begin = command;

    while (*command) {
        *argc += 1;

        while (*command != ' ' && *command != '\t') {
            if (!*command || n >= SHELL_ARG_BUF_MAX) {
                break;
            }
            command++;
        }

        /* Skip whitespace */
        while (*command == ' ' || *command == '\t') {
            if (!*command) {
                break;
            }
            command++;
        }
    }

    *argv = malloc(sizeof(char*) * *argc);
    if (!*argv) {
        printf("Memory error: unable to allocate space for argv table\r\n");
        return;
    }

    command = begin;
    *argc = 0;

    while (*command) {
        uint32_t i = 0;
        n = 0;
        *argc += 1;
        (*argv)[*argc-1] = malloc(SHELL_ARG_BUF_MAX);

        if ((*argv)[*argc-1] == NULL) {
            printf("Memory error: unable to allocate space for arguement\r\n");
            (*argc)--;
            while (*argc) {
                free((*argv)[*argc-1]);
                (*argc)--;
            }
            free(*argv);
            *argc = -1;
            argv = NULL;
            return;
        }

        while (*command != ' ' && *command != '\t') {
            if (!*command || n >= (SHELL_ARG_BUF_MAX-1)) {
                break;
            }
            (*argv)[*argc-1][i++] = *command++;
        }
        (*argv)[*argc-1][i] = '\0';

        /* Skip whitespace */
        while (*command == ' ' || *command == '\t') {
            if (!*command) {
                break;
            }
            command++;
        }
    }
}

void run_command(char *command, int argc, char **argv) {
    if (!argc) {
        return;
    }

    for (int i = 0; i < NUM_COMMANDS; i++) {
        if (!strncmp(argv[0], valid_commands[i].name, SHELL_BUF_MAX+1)) {
            valid_commands[i].fptr(argc, argv);
            return;
        }
    }

    printf("%s: command not found\r\n", argv[0]);
}

void help(int argc, char **argv) {
    puts("Available commands:\r\n");

    for (uint32_t i = 0; i < NUM_COMMANDS; i++) {
        printf("%s\r\n", valid_commands[i].name);
    }
}
DEFINE_APP(help)

void history(int argc, char **argv) {
    int count = 0; // printed counter, skip blank lines

    printf("History:\r\n");
    // iterate through history list, printing nonempty items
    for (int i = 1; i < SHELL_HISTORY + 1; i++) {
        int j = (cmd_index + i) % SHELL_HISTORY;
        if (cmd_hist[j][0] == '\0')
            continue;
        printf("\t%d: %s\r\n", count, cmd_hist[j]);
        count++;
    }
}
DEFINE_APP(history)
