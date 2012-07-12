void shell(void);
void free_argv(uint32_t argc, char ***argv);
void parse_command(char *command, uint32_t *argc, char ***argv);
void uname(uint32_t argc, char **argv);

#define SHELL_BUF_MAX       256
#define SHELL_ARG_BUF_MAX   256
#define SHELL_PROMPT        "$ "

#define BACKSPACE           "\e[1D\e[0K"
