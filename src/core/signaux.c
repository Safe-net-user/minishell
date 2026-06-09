#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void    handle_sigint(int sig)
{
    (void)sig;
    write(1, "\n", 1);
    // réafficher le prompt si besoin
}

void    set_signaux_interactif(void)
{
    signal(SIGINT, handle_sigint);
    signal(SIGQUIT, SIG_IGN);
}
