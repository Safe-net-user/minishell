#define _POSIX_C_SOURCE 200809L
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

/*struct sigaction {
    void     (*sa_handler)(int);   // handler ou SIG_IGN ou SIG_DFL
    sigset_t sa_mask;              // signaux bloqués pendant le handler
    int      sa_flags;             // options
    void     (*sa_restorer)(void); // obsolète, ignore
};*/


struct  sigaction sa;

void    set_signaux_interactif(void)
{
    sigemptyset(&sa.sa_mask);
    sa.__sigaction_handler = handle_sigint(); //fonction a faire
    sigaction(SIGINT, &sa, NULL);
    sa.__sigaction_handler = SIG_IGN; //ignorer en mode interactif
    sigaction(SIGQUIT, &sa, NULL);
}
