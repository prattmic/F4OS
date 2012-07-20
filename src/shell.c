#include "types.h"
#include "string.h"
#include "task.h"
#include "mem.h"
#include "buddy.h"
#include "usart.h"
#include "shell.h"

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

        if (!argc) {
        }
        else if (argc < 0) {
            printf("%s: could not parse input\r\n", command);
        }
        else if (!strncmp(argv[0], "uname", SHELL_BUF_MAX+1)) {
            uname(argc, argv);
        }
        else {
            printf("%s: command not found\r\n", command);
        }

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
