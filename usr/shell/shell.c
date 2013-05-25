#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "shell.h"

/* Shell commands */
#include "blink.h"
#include "top.h"
#include "uname.h"
#include "ipctest.h"
#include "accel.h"
#include "ghetto_gyro.h"
#include "lowpass.h"
#include "rd_test.h"
#include "px4_mag.h"
#include "px4_baro.h"
#include "px4_accel_gyro.h"

struct command {
    char *name;
    void (*fptr)(int, char **);
};

const struct command valid_commands[] = {{"help",   &help},
                                         {"top",    &top},
                                         {"uname",  &uname},
                                         {"ipctest", &ipctest},
                                         {"rd_test", &rd_test},
#ifdef CONFIG_HAVE_LED
                                         {"blink",  &blink},
#endif
#if defined(CONFIG_HAVE_SPI) && defined(CONFIG_STM32_BOARD_DISCOVERY)
                                         {"accel", &accel},
#endif
#if defined(CONFIG_HAVE_SPI) && defined(CONFIG_STM32_BOARD_PX4)
                                         {"px4_accel_gyro", &px4_accel_gyro},
#endif
#if defined(CONFIG_HAVE_I2C) && defined(CONFIG_STM32_BOARD_DISCOVERY)
                                         {"ghetto_gyro", &ghetto_gyro},
                                         {"lowpass", &lowpass_test},
#endif
#if defined(CONFIG_HAVE_I2C) && defined(CONFIG_STM32_BOARD_PX4)
                                         {"px4_mag", &px4_mag},
                                         {"px4_baro", &px4_baro},
#endif
};
#define NUM_COMMANDS    (sizeof(valid_commands)/sizeof(valid_commands[0]))

static void free_argv(int argc, char ***argv);
static void parse_command(char *command, int *argc, char ***argv);
static void run_command(char *command, int argc, char **argv);

void shell(void) {
    int n = -1;
    int argc;
    char **argv;

    char *command = malloc(SHELL_BUF_MAX+1);
    if (!command) {
        printf("Shell unable to allocate buffer for command.  You are on your own.  Goodbye.\r\n");
        return;
    }

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

        printf("%s", SHELL_PROMPT);
        n = -1;
    }

    free(command);
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

    *argv = malloc(4 * *argc);
    if (!*argv) {
        printf("Memory error\r\n");
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
            printf("Memory error\r\n");
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
