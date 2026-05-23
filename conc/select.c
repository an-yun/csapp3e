/* $begin select */
#include "csapp.h"
#include "echo.h"
int command(void);

int main(int argc, char **argv) 
{
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    fd_set read_set, ready_set;

    if (argc != 2) {
	fprintf(stderr, "usage: %s <port>\n", argv[0]);
	exit(0);
    }
    listenfd = Open_listenfd(argv[1]);  //line:conc:select:openlistenfd

    FD_ZERO(&read_set);              /* Clear read set */ //line:conc:select:clearreadset
    FD_SET(STDIN_FILENO, &read_set); /* Add stdin to read set */ //line:conc:select:addstdin
    FD_SET(listenfd, &read_set);     /* Add listenfd to read set */ //line:conc:select:addlistenfd

    while (1) {
	ready_set = read_set;
	Select(listenfd+1, &ready_set, NULL, NULL, NULL); //line:conc:select:select
	if (FD_ISSET(STDIN_FILENO, &ready_set)) {
		//line:conc:select:stdinready
		if (!command()) {
			return 0;
		}
	}
	if (FD_ISSET(listenfd, &ready_set)) { //line:conc:select:listenfdready
            clientlen = sizeof(struct sockaddr_storage); 
	    connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
	    echo(connfd); /* Echo client input for each line */
		Close(connfd);
	}
    }
}

int command(void) {
    char buf[MAXLINE];
    if (!Fgets(buf, MAXLINE, stdin))
	exit(0); /* EOF */
    printf("%s", buf); /* Process the input command */
	if (!strcmp(buf, "q\n")) {
		return 0;
	} else {
		return 1;
	}
}
/* $end select */


