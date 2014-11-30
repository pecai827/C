#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <arpa/inet.h>
#include <stdbool.h>

#define SHMSZ     1024
#define BUFLEN 512  //Max length of buffer
/* on shared memory */
int main() {
	FILE *fp;
	char my_string[1024];
	char my_stringLocal[1024];

	char available[] = "available";
	int sockfd = 0, n = 0, m = 1;
	char recvBuff[1024];
	char sendBuff[1024];
	struct sockaddr_in serv_addr;
	struct sockaddr_in clit_addr;
	char* IPAddress;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	int shmid;
	key_t key;
	char *shm;
	int* flag;
	int PORT = 21629;
	int UDPPORT = 0;
	key = 1234;
	char portNum[50][50];
	fp = fopen("patient2.txt", "r");
	//already open the file and read from it.
	fgets(my_string, 100, fp);
	/* Close stream; */
	fclose(fp);
	strtok(my_string, "\n"); //Delete the /n in the end
	size_t destination_size = sizeof(my_stringLocal);
	strncpy(my_stringLocal, my_string, destination_size);
	my_stringLocal[destination_size - 1] = '\0'; //Mark the end of the line

	char userName[20][50];
	char userPass[20][50];
	//split name and password and save separately;
	char * splitRt;
	int count = 1;
	splitRt = strtok(my_stringLocal, " ,.-");
	while (splitRt != NULL) {
		if (count % 2 != 0) {
			strcpy(userName[0], splitRt);
			splitRt = strtok(NULL, " ,.-");
			count++;
		} else {
			strcpy(userPass[0], splitRt);
			strtok(userPass[0], "\n");
			splitRt = strtok(NULL, " ,.-");
		}

	}

	if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {//reused code, to open a shared memory.
		perror("shmget");
		return 1;
	}

	flag = (int *) shmat(shmid, NULL, 0);//To bind the array to the shared memory

	memset(recvBuff, '0', sizeof(recvBuff));
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Error : Could not create socket \n");
		return 1;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;//reused code;
	serv_addr.sin_port = htons(PORT);//reused code;
	struct hostent *lh = gethostbyname("localhost");//reused code;

	IPAddress = inet_ntoa(*(struct in_addr *) *lh->h_addr_list);

	if (inet_pton(AF_INET, IPAddress, &serv_addr.sin_addr) <= 0) {//reused code;
		printf("\n inet_pton error occured\n");
		return 1;
	}

	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))
			< 0) {//reused code;
		printf("\n Error : Connect Failed \n");
		return 1;
	}
	int portTCP = ntohs(clit_addr.sin_port);
	connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));//reused code;

	int receive[20];
	n = recv(sockfd, receive, 1024, 0);
	printf("Phase 1: Patient 2 has TCP port number %d  and IP address %s.\n",
			*receive, IPAddress);
	printf("\n");
	n = send(sockfd, my_string, strlen(my_string), 0);
	printf(
			"Phase 1: Authentication request from Patient 2 with username %s and password %s has been sent to the Health Center Server.\n",
			userName[0], userPass[0]);
	printf("\n");

	char feedBack[1024];
	n = recv(sockfd, feedBack, 1024, 0);
	printf("Phase 1: Patient 2 authentication result: %s.\n", feedBack);
	printf("\n");

	char recvAva[200];
	char port[200];
	if (strcmp(feedBack, "success") == 0) {//Compare the message returned by server.

		printf("Phase 1: End of Phase 1 for Patient2. \n");
		printf("\n");

		n = send(sockfd, available, strlen(available), 0);

		n = recv(sockfd, recvAva, 200, 0);
		recvAva[n] = '\0';
		printf(
				"Phase 2: The following appointments are available for Patient 2: \n");
		printf("\n");

		printf("%s\n", recvAva);	//Print out time slots
		int lineNum = 0;

		char revIndex[50];
		int i1 = 0;
		for (i1 = 0; i1 < sizeof(recvAva) / sizeof(recvAva[0]); i1++) {//Calculate to find the total line number
			if (recvAva[i1] == '\n') {
				lineNum++;
			}
		}
		int i2 = 0;
		for (i2 = 0; i2 < lineNum; i2++) {//Get time index;
			revIndex[i2] = recvAva[i2 * 11];
			strtok(&revIndex[i2], "\n");

		}
		size_t revIndex_size = sizeof(revIndex);
		revIndex[revIndex_size] = '\0';

		bool UDP = false;
		bool indexFound = false;
		int indexChosen;
		while (!indexFound) {
			printf(
					"Please enter the preferred appointment index and press enter: \n");

			scanf("%d", &indexChosen);
			int i3 = 0;
			for (i3 = 0; i3 < sizeof(revIndex) / sizeof(revIndex[0]); i3++) {//Traverse to check whether the index the user input exist.
				if ((revIndex[i3] - '0') == indexChosen) {
					indexFound = true;
					break;
				}
			}
			if (!indexFound) {
				printf(
						"The index number you input is not shown, reinput again.\n");
				printf("\n");

			}

		}
		char c = (char) (((int) '0') + indexChosen);
		char selection[200] = "selection";
		strcat(selection, " ");
		strcat(selection, &c);
		size_t selectionSize = sizeof(selection);
		selection[selectionSize] = '\0'; //Mark the end of the line;
		send(sockfd, selection, sizeof(selection), 0);
		n = recv(sockfd, port, 200, 0);
		port[n] = '\0'; //mark the end
		if (strcmp(port, "notavailable") == 0) {
			printf(
					"Phase 2: The requested appointment from Patient 2 is no available. Exiting...\n");
			printf("\n");

			close(sockfd);
		}

		/**
		 * Get port number
		 */
		else {

			char *pch;

			int col = 0;
			pch = strtok(port, " ,.-");
			while (pch != NULL) {
				strcpy(portNum[col], pch);
				pch = strtok(NULL, " ,.-");
				col++;
			}
			strtok(portNum[1], "\n");

			printf(
					"Phase 2: The requested appointment is available and reserved to Patient2. The assigned doctor port number is %s.\n",
					portNum[1]);
			printf("\n");

			UDPPORT = atoi(portNum[1]);

			UDP = true;
		}
		/*
		 * Begin UDP communication
		 *
		 */
		if (UDP) {

			struct sockaddr_in si_other;
			int sUDP;
			socklen_t slen = sizeof(si_other);
			char buf[BUFLEN];
			char message[BUFLEN];
			FILE *fpUDP;
			char my_stringUDP[1024];
			fpUDP = fopen("patient2insurance.txt", "r");
			//already open the file and read from it.
			fgets(my_stringUDP, 100, fpUDP);
			/* Close stream; */
			fclose(fpUDP);
			strtok(my_stringUDP, "\n");
			strcat(my_stringUDP, " ");
			strcat(my_stringUDP, userName[0]);
			strtok(my_stringUDP, "\n");
			size_t sendSize = sizeof(my_stringUDP);
			my_stringUDP[sendSize] = '\0'; //Mark the end of the line

			if ((sUDP = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {//reused code;
			}
			memset((char * ) &si_other, 0, sizeof(si_other));
			si_other.sin_family = AF_INET;
			si_other.sin_port = htons(UDPPORT);

			if (inet_aton(IPAddress, &si_other.sin_addr) == 0) {//reused code;
				fprintf(stderr, "inet_aton() failed\n");
				exit(1);
			}
			struct hostent *lh = gethostbyname("localhost");
			char *UDPHost;
			UDPHost = inet_ntoa(*(struct in_addr *) *lh->h_addr_list);

			char insuranceSplit[100];
			char nameSplict[100];
			char * pchUDP;

			sendto(sUDP, my_stringUDP, strlen(my_stringUDP), 0,
					(struct sockaddr *) &si_other, slen);
			memset(buf, '\0', BUFLEN);
			strtok(my_stringUDP, " ,.-");

			//try to receive some data, this is a blocking call
			if (recvfrom(sUDP, buf, BUFLEN, 0, (struct sockaddr *) &si_other,
					&slen) == -1) {
			}
			char onlyPrice[100][100];
			char onlyPort[100][100];

			//split name and password and save;
			char * splitF;
			int countF = 1;
			splitF = strtok(buf, " ,.-");
			while (splitF != NULL) {
				if (countF % 2 != 0) {
					strcpy(onlyPrice[0], splitF);
					splitF = strtok(NULL, " ,.-");
					countF++;
				} else {
					strcpy(onlyPort[0], splitF);
					strtok(onlyPort[0], "\n");
					splitF = strtok(NULL, " ,.-");
				}

			}


			printf(
					"In Phase 3: Patient 2 has a dynamic UDP port number %s and IP address %s.\n",
					onlyPort[0], IPAddress);
			printf("\n");
			printf(
					"Phase 3: The cost estimation request from Patient 2 with insurance plan %s has been sent to the doctor with port number %d and IP address %s.\n",
					my_stringUDP, UDPPORT, UDPHost);
			printf("\n");
			printf(
					"Phase 3: Patient 2 receives $%s estimation cost from doctor with port number %d and name %s.\n",
					onlyPrice[0], UDPPORT, portNum[0]);
			printf("\n");

			printf("Phase 3: End of Phase 3 for Patient 2.\n");
			printf("\n");

		}
	} else {
		close(sockfd);
		return 0;
	}
	close(sockfd);

	return 0;
}

