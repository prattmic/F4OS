#include "types.h"
#include "string.h"
#include "task.h"
#include "mem.h"
#include "semaphore.h"
#include "buddy.h"
#include "usart.h"
#include "top.h"
#include "shell.h"

struct command {
    char *name;
    void (*fptr)(uint32_t, char **);
};

const struct command valid_commands[] = {{"uname",  &uname},
                                         {"top",    &top}};
#define NUM_COMMANDS    (sizeof(valid_commands)/sizeof(valid_commands[0]))

static void free_argv(uint32_t argc, char ***argv);
static void parse_command(char *command, uint32_t *argc, char ***argv);
static void run_command(char *command, uint32_t argc, char **argv);

void shell(void) {
    char *command = malloc(SHELL_BUF_MAX+1);
    int n = -1;
    uint32_t argc;
    char **argv;

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

        run_command(command, argc, argv);

        free_argv(argc, &argv);
        printf("%s", SHELL_PROMPT);
        n = -1;
    }

    free(command);
}

void free_argv(uint32_t argc, char ***argv) {
    while (argc) {
        free((*argv)[argc-1]);
        argc--;
    }
    free(*argv);
}

void parse_command(char *command, uint32_t *argc, char ***argv) {
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

    *argv = malloc(4 * *argc);
    command = begin;
    *argc = 0;

    while (*command) {
        uint32_t i = 0;
        n = 0;
        *argc += 1;
        (*argv)[*argc-1] = malloc(SHELL_ARG_BUF_MAX);

        if (*argv[*argc-1] == NULL) {
            printf("Memory error\r\n");
            (*argc)--;
            while (*argc) {
                free(*argv[*argc-1]);
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

void run_command(char *command, uint32_t argc, char **argv) {
    if (!argc) {
        return;
    }

    if (argc < 0) {
        printf("%s: could not parse input\r\n", command);
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

void uname(uint32_t argc, char **argv) {
    if (argc > 1) {
        if (!strncmp(argv[1], "-a", SHELL_ARG_BUF_MAX)) {
            printf("F40S rev %d %s\r\n", BUILD_REV, BUILD_TIME);
        }
        else if (!strncmp(argv[1], "-r", SHELL_ARG_BUF_MAX)) {
            printf("rev %d\r\n", BUILD_REV);
        }
        else {
            printf("%s: unrecognized option '%s'\r\n", argv[0], argv[1]);
        }
    }
    else {
        printf("F4OS\r\n");
    }
}
