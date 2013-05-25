void shell(void);
void help(int argc, char **argv);
void history(int argc, char **argv);

#define SHELL_BUF_MAX       256
#define SHELL_ARG_BUF_MAX   256
#define SHELL_PROMPT        "$ "
#define SHELL_HISTORY       (10) // Command history length

#define BACKSPACE           "\e[1D\e[0K"
