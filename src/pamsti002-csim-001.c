/*
 ============================================================================
 Name        : pamsti002-csim-001.c
 Author      : Olivier Durand
 Version     : r002-001
 Copyright   : (c) 2015
 Description : NFC simulator to interact with python GUI through named pipe
 Ref. file   : ctopyqt001.c
 ============================================================================
*/

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUF 1024

int  main()
{
	int ret;

	ret = system("clear");
	printf("[%d] Initial system clear\n", ret);

	int fdw, fdr, i, selectionMenu = 0;
	char buf[MAX_BUF];
	char* textInitCon = "Hi";
	char* textPipeWriteError = "There was an error writing to pipe\n";
	char* myfifow = "/tmp/ctopyfifo";
	char* myfifor = "/tmp/pytocfifo";

	const char* choix[5][2];

	choix[0][0] = "A1f2G7";
	choix[0][1] = "Olivier Durand";
	choix[1][0] = "weq814";
	choix[1][1] = "Maurice Müller";
	choix[2][0] = "5zjtR2";
	choix[2][1] = "Guillaume Cuérel";
	choix[3][0] = "Hbid13";
	choix[3][1] = "John Doe";
	choix[4][0] = "zuR5c4";
	choix[4][1] = "Julien Haueter";

	/* Create fifos */
	mkfifo(myfifow,0666);
	mkfifo(myfifor,0666);

	/* Send Hi to pipe */
	printf("Writing init string TO pipe: >> %s << ...\n", textInitCon);
	fdw = open(myfifow, O_WRONLY);

	if (fdw < 0) {
		return -1;
	}

    if (write(fdw, textInitCon, sizeof(textInitCon)) != sizeof(textInitCon)) {
    	printf("%s",textPipeWriteError);
        close(fdw);
        return -1;
    }

    close(fdw);

	/* Wait Ok from pipe */
	fdr = open(myfifor, O_RDONLY);
	ret = read(fdr, buf, MAX_BUF);
	printf("[%d] Received init string FROM pipe: >> %s << ...\n", ret, buf);
	close(fdr);

	if (strcmp(buf, "Ok") == 0) {
		printf("Init ok, start pushing ...\n\n");

		/* Print menu */
		printf("Index\tID\tPerson\n");
		printf("-----\t--\t------\n\n");

		/* print selection list */
		for (i = 0; i < 5; i++) {
				printf("%d\t%s\t%s\n",i, choix[i][0], choix[i][1]);
		}

		printf("\n");

		while (selectionMenu != 8) {
			printf("Type index, 8 to quit: ");
			ret = scanf("%d", &selectionMenu);

			if (selectionMenu != 8) {
				printf("[%d] Writing: >> %s << to pipe...\n", ret, choix[selectionMenu][0]);
				fdw = open(myfifow, O_WRONLY);
			    if (write(fdw, choix[selectionMenu][0], strlen(choix[selectionMenu][0])) != strlen(choix[selectionMenu][0])) {
			    	printf("%s",textPipeWriteError);
			    }
				close(fdw);
			} else {
				printf("Writing: >> %s << to pipe...\n", "Bye");
				fdw = open(myfifow, O_WRONLY);
			    if (write(fdw, "Bye", strlen("Bye")) != strlen("Bye")) {
			    	printf("%s",textPipeWriteError);
			    }
				close(fdw);
			}

			fdr = open(myfifor, O_RDONLY);
			ret = read(fdr, buf, MAX_BUF);
			printf("[%d] Received string FROM pipe: >> %s << ...\n", ret, buf);
			close(fdr);

		}

		printf("\nstop pushing ... program terminated!\n\n");

	} else {
		printf("Init failed, abort!\n");
	}

	/* Remove fifos */
	unlink(myfifow);
	unlink(myfifor);

	return 0;
}
