#ifndef USR_SHELL_SHELL_H_INCLUDED
#define USR_SHELL_SHELL_H_INCLUDED

void shell(void);
void help(int argc, char **argv);
void history(int argc, char **argv);

#define SHELL_BUF_MAX       256
#define SHELL_ARG_BUF_MAX   256
#define SHELL_PROMPT        "$ "
#define SHELL_PROMPT_LEN    (2)  // strlen(SHELL_PROMPT)
#define SHELL_HISTORY       (10) // Command history length

#define CLEARLINE           "\e[K"
#define RIGHT               "\e[C"
#define LEFT                "\e[D"

#endif
