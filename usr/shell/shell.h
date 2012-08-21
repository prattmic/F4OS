void shell(void);
void help(int argc, char **argv);

#define SHELL_BUF_MAX       256
#define SHELL_ARG_BUF_MAX   256
#define SHELL_PROMPT        "$ "

#define BACKSPACE           "\e[1D\e[0K"
