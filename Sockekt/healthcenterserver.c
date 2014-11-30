#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <errno.h>
#include <netdb.h>

#define SHMSZ 1024

int main(int argc, char *argv[]) {
	FILE *fp;
	FILE *fpava;
	char c, tmp;
	int shmid;
	key_t key;
	char *shm;
	int rows = 6; // The number of rows of the 2D array for flag
	int columns = 1; // The number of columns of the 2D array for flag
	int row, column;
	int* flag;
	char my_string[20][50];
	char availabilities[20][50];
	char name[20][50];
	char password[20][50];
	char nameSplit[20];
	char passwordSplit[20];
	char ava[1024][1024];
	char sendAva[1024][1024];
	char docAndPort[100][100];
	key = 1234;
	int PORT = 21629;
	int flagMatch = 0;
	int size = sizeof(name) / sizeof(name[0]);
	int m1 = 0; //for check password and username
	if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
		perror("shmget");
		return 1;
	}
	/*
	 * Now we attach the segment to our data space.
	 */

	flag = (int *) shmat(shmid, NULL, 0);
	for (row = 0; row < 6; row++) {
		flag[row] = 1;

	}

	fp = fopen("user.txt", "r");
	//already open the file and read from it.
	int i1 = 0;
	for (i1 = 0; i1 < 2; i1++) {

		fgets(my_string[i1], 50, fp);
		strtok(my_string[i1], "\n");
	}

	fclose(fp);

	/**
	 *Read name and password from file
	 * Separate the line and save separately
	 */
	char * pch;
	int i2 = 0;
	int j = 0;
	int k1 = 0;
	for (k1 = 0; k1 < 2; k1++) {

		int count = 1;
		pch = strtok(my_string[k1], " ,.-");
		while (pch != NULL) {
			if (count % 2 != 0) {
				strcpy(name[i2++], pch);

				pch = strtok(NULL, " ,.-");
				count++;
			} else {
				strcpy(password[j++], pch);
				//strtok(password[j], "\n");
				pch = strtok(NULL, " ,.-");
			}

		}

	}
	/**
	 *Read available,store the whole line in one place.
	 *
	 */
	int a = 0;
	fpava = fopen("availabilities.txt", "r");
	char available[1024][1024];
	char line[100];
	char dataBase[1024][1024];
	while (fgets(line, sizeof line, fpava) != NULL) {
		strcpy(available[a], line);
		strcpy(dataBase[a], line);
		a++;
	}

	char * pch2;
	char section[1024][1024];
	int col = 0;
	int i3 = 0;
	for (i3 = 0; i3 < 6; i3++) {
		pch2 = strtok(available[i3], " ,.-");
		while (pch2 != NULL) {
			strcpy(section[col], pch2);
			pch2 = strtok(NULL, " ,.-");
			col++;
		}
	}
/**
 * Send only first three columns back to patients.
 *
 */
	/**
	 *
	 * Row 1
	 */strcat(ava[0], section[0]);
	strcat(ava[0], " ");
	strcat(ava[0], section[1]);
	strcat(ava[0], " ");
	strcat(ava[0], section[2]);

	/**
	 * Row 2
	 */
	strcat(ava[1], section[5]);
	strcat(ava[1], " ");
	strcat(ava[1], section[6]);
	strcat(ava[1], " ");
	strcat(ava[1], section[7]);
	/**
	 * Row 3
	 */
	strcat(ava[2], section[10]);
	strcat(ava[2], " ");
	strcat(ava[2], section[11]);
	strcat(ava[2], " ");
	strcat(ava[2], section[12]);
	/*
	 * Row4
	 *
	 */
	strcat(ava[3], section[15]);
	strcat(ava[3], " ");
	strcat(ava[3], section[16]);
	strcat(ava[3], " ");
	strcat(ava[3], section[17]);
	/**
	 * Row5
	 */
	strcat(ava[4], section[20]);
	strcat(ava[4], " ");
	strcat(ava[4], section[21]);
	strcat(ava[4], " ");
	strcat(ava[4], section[22]);
	/**
	 * Row6
	 */
	strcat(ava[5], section[25]);
	strcat(ava[5], " ");
	strcat(ava[5], section[26]);
	strcat(ava[5], " ");
	strcat(ava[5], section[27]);

	strcat(docAndPort[0], section[3]);
	strcat(docAndPort[0], " ");
	strcat(docAndPort[0], section[4]);
	strcat(docAndPort[1], section[8]);
	strcat(docAndPort[1], " ");
	strcat(docAndPort[1], section[9]);
	strcat(docAndPort[1], " ");
	strcat(docAndPort[2], section[13]);
	strcat(docAndPort[2], " ");
	strcat(docAndPort[2], section[14]);
	strcat(docAndPort[2], " ");

	strcat(docAndPort[3], section[18]);
	strcat(docAndPort[3], " ");
	strcat(docAndPort[3], section[19]);
	strcat(docAndPort[3], " ");

	strcat(docAndPort[4], section[23]);
	strcat(docAndPort[4], " ");
	strcat(docAndPort[4], section[24]);
	strcat(docAndPort[4], " ");

	strcat(docAndPort[5], section[28]);
	strcat(docAndPort[5], " ");
	strcat(docAndPort[5], section[29]);
	strcat(docAndPort[5], " ");

	int listenfd = 0, qlength = 10, connfd = 0, t = 5;
	int n;
	struct sockaddr_in serv;
	struct sockaddr_in dest;
	socklen_t socksize = sizeof(struct sockaddr_in);
	char sendBuff[1024];
	pid_t pid;
	int m = 1;
	char* IPAddress;
	listenfd = socket(AF_INET, SOCK_STREAM, 0); //socket for listening to connection requests

	memset(&serv, '0', sizeof(serv));//reused code;
	serv.sin_family = AF_INET;//reused code;
	serv.sin_addr.s_addr = htonl(INADDR_ANY);//reused code;
	serv.sin_port = htons(PORT);
	struct hostent *lh = gethostbyname("localhost");

	IPAddress = inet_ntoa(*(struct in_addr *) *lh->h_addr_list);
	bind(listenfd, (struct sockaddr *) &serv, sizeof(serv)); //binding the socket to a port

	printf("The Health Center Server has port number %d and IP address %s.\n",
			PORT,IPAddress);
	printf("\n");

	listen(listenfd, 2);
	while (1) {
		connfd = accept(listenfd, (struct sockaddr *) &dest, &socksize); //another socket for sending and receiving on the previously built socket
		//reused code;
		printf("\n");

		pid = fork();
		if (pid < 0) {
			perror("ERROR on fork");
			exit(1);
		}
		if (pid != 0) {

			close(connfd);

		}
		else {//If fork succeed, run the following codes;
			int clientPort = ntohs(dest.sin_port);
			char * splitRt;
			int i = 0;
			int j = 0;
			int count = 1;
			int sendFirst[30] = { clientPort };
			send(connfd, sendFirst, 1024, 0);
			n = recv(connfd, sendBuff, 1024, 0);
			sendBuff[n] = '\0'; //mark the end
			char receName[200][200];
			char recePass[200][200];
			//split name and password and save;
			splitRt = strtok(sendBuff, " ,.-");
			while (splitRt != NULL) {
				if (count % 2 != 0) {
					strcpy(receName[i++], splitRt);
					splitRt = strtok(NULL, " ,.-");
					count++;
				} else {
					strcpy(recePass[j++], splitRt);
					strtok(recePass[j], "\n");
					splitRt = strtok(NULL, " ,.-");
				}

			}
			printf(
					"The Health Center Server has received request from a patient with username %s and password %s.\n",
					receName[0], recePass[0]);
			printf("\n");
/*
 * compare the message received from patients to check whether their information is correct, and send back status messages.
 */
			for (m1 = 0; m1 < size; m1++) {
				if (strcmp(receName[0], name[m1]) == 0
						&& strcmp(recePass[0], password[m1]) == 0) {
					printf(
							"Phase 1: The Health Center Server sends the response success to patient with username %s.\n",
							receName[0]);
					printf("\n");

					send(connfd, "success", strlen("success"), 0);
					flagMatch = 1;
					break;
				} else {
					flagMatch = 0;
				}

			}
			if (!flagMatch) {
				printf(
						"Phase 1: The Health Center Server sends the response failure to patient with username %s.\n",
						receName[0]);
				printf("\n");

				send(connfd, "failure", strlen("failure"), 0);
			}
			char receAvailable[1024];
			n = recv(connfd, receAvailable, 1024, 0);//reused code;
			if (strcmp(receAvailable, "available") == 0) {
				printf(
						"Phase 2: The Health Center Server, receives a request for available time slots from patients with port number %d and IP address %s.\n",
						ntohs(dest.sin_port), inet_ntoa(dest.sin_addr));
				printf("\n");

				int i4 = 0;
				for (i4 = 0; i4 < sizeof(ava) / sizeof(ava[0]); i4++) {

					if (flag[i4] == 1) {
						strcat(sendAva[0], ava[i4]);
						strcat(sendAva[0], "\n");
					} else {
					}

				}
				printf(
						"Phase 2: The Health Center Server sends available time slot to patient with username %s.\n",
						receName[0]);
				printf("\n");

				send(connfd, sendAva[0], strlen(sendAva[0]), 0);//reused code;
				int timeIndex = 0;
				char selection[200];
				recv(connfd, selection, 1024, 0);

				char * splitRt2;
				char timeString[100];
				char selecString[100];
				int count2 = 1;
				//split name and password and save;
				splitRt2 = strtok(selection, " ,.-");
				while (splitRt2 != NULL) {
					if (count % 2 != 0) {
						strcpy(selecString, splitRt2);
						splitRt2 = strtok(NULL, " ,.-");
						count2++;
					} else {
						strcpy(timeString, splitRt2);
						splitRt2 = strtok(NULL, " ,.-");
					}

				}

				timeIndex = atoi(timeString);

				printf(
						"Phase 2: The Health Center Server receives a request for appointment %d from patient with port number %d and username %s.\n",
						timeIndex, ntohs(dest.sin_port), receName[0]);
				printf("\n");

				switch (timeIndex) {
				case 1:
					if (flag[0] == 1) {//To let number 1 detonate "not reserved", and let number 0 detonate "reserved"
						flag[0] = 0;
						printf(
								"Phase 2: The Health Center Server confirms the following appointment %d to patient with username %s.\n",
								timeIndex, receName[0]);
						printf("\n");

						send(connfd, docAndPort[0], strlen(docAndPort[0]), 0);
					} else {
						printf(
								"Phase 2: The Health Center Server rejects the following appointment %d to patient with username %s.\n",
								timeIndex, receName[0]);
						printf("\n");

						send(connfd, "notavailable", strlen("notavailable"), 0);
					}
					break;
				case 2:

					if (flag[1] == 1) {//To let number 1 detonate "not reserved", and let number 0 detonate "reserved"
						flag[1] = 0;
						printf(
								"Phase 2: The Health Center Server confirms the following appointment %d to patient with username %s.\n",
								timeIndex, receName[0]);
						printf("\n");

						send(connfd, docAndPort[1], strlen(docAndPort[1]), 0);
					} else {
						printf(
								"Phase 2: The Health Center Server rejects the following appointment %d to patient with username %s.\n",
								timeIndex, receName[0]);
						printf("\n");

						send(connfd, "notavailable", strlen("notavailable"), 0);
					}

					break;
				case 3:
					if (flag[2] == 1) {//To let number 1 detonate "not reserved", and let number 0 detonate "reserved"
						flag[2] = 0;
						printf(
								"Phase 2: The Health Center Server confirms the following appointment %d to patient with username %s.\n",
								timeIndex, receName[0]);
						printf("\n");

						send(connfd, docAndPort[2], strlen(docAndPort[2]), 0);
					} else {
						printf(
								"Phase 2: The Health Center Server rejects the following appointment %d to patient with username %s.\n",
								timeIndex, receName[0]);
						printf("\n");

						send(connfd, "notavailable", strlen("notavailable"), 0);
					}

					break;
				case 4:
					if (flag[3] == 1) {//To let number 1 detonate "not reserved", and let number 0 detonate "reserved"
						flag[3] = 0;
						printf(
								"Phase 2: The Health Center Server confirms the following appointment %d to patient with username %s.\n",
								timeIndex, receName[0]);
						printf("\n");

						send(connfd, docAndPort[3], strlen(docAndPort[3]), 0);
					} else {
						printf(
								"Phase 2: The Health Center Server rejects the following appointment %d to patient with username %s.\n",
								timeIndex, receName[0]);
						printf("\n");

						send(connfd, "notavailable", strlen("notavailable"), 0);
					}
					break;
				case 5:
					if (flag[4] == 1) {//To let number 1 detonate "not reserved", and let number 0 detonate "reserved"
						flag[4] = 0;
						printf(
								"Phase 2: The Health Center Server confirms the following appointment %d to patient with username %s.\n",
								timeIndex, receName[0]);
						printf("\n");

						send(connfd, docAndPort[4], strlen(docAndPort[4]), 0);
					} else {
						printf(
								"Phase 2: The Health Center Server rejects the following appointment %d to patient with username %s.\n",
								timeIndex, receName[0]);
						printf("\n");

						send(connfd, "notavailable", strlen("notavailable"), 0);
					}
					break;
				case 6:

					if (flag[5] == 1) {//To let number 1 detonate "not reserved", and let number 0 detonate "reserved"
						flag[5] = 0;
						printf(
								"Phase 2: The Health Center Server confirms the following appointment %d to patient with username %s.\n",
								timeIndex, receName[0]);
						printf("\n");

						send(connfd, docAndPort[5], strlen(docAndPort[5]), 0);
					} else {
						printf(
								"Phase 2: The Health Center Server rejects the following appointment %d to patient with username %s.\n",
								timeIndex, receName[0]);
						printf("\n");

						send(connfd, "notavailable", strlen("notavailable"), 0);
					}
					break;
				default:
					printf("Invalid time index\n");
				}

			}

			close(listenfd);
			exit(0);
			return 0;

		}

	}
	return 0;
}

