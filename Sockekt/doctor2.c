#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<unistd.h>
#include <netdb.h>
#define BUFLEN 512  //Max length of buffer
#define PORT 42629   //The port on which to listen for incoming data

void die(char *s) {
	perror(s);
	exit(1);
}

int main(void) {
	FILE *fp;
	char my_string[50][50];
	char insurance[50][50];
	char price[20][50];
	struct sockaddr_in si_me, si_other;
	socklen_t slen = sizeof(si_other);
	char * IPAddress;
	int s, recv_len;
	char buf[BUFLEN];

	fp = fopen("doc1.txt", "r");
	//already open the file and read from it.
	int i1 = 0;
	for (i1 = 0; i1 < 3; i1++) {

		fgets(my_string[i1], 50, fp);
		strtok(my_string[i1], "\n");
	}
	fclose(fp);

	char * pch;
	int row = 0;
	int col = 0;
	int k1 = 0;
	for (k1 = 0; k1 < 3; k1++) {

		int count = 1;
		pch = strtok(my_string[k1], " ,.-");
		while (pch != NULL) {
			if (count % 2 != 0) {
				strcpy(insurance[row++], pch);

				pch = strtok(NULL, " ,.-");
				count++;
			} else {
				strcpy(price[col++], pch);
				pch = strtok(NULL, " ,.-");
			}

		}
		int i2 = 0;
		for (i2 = 0; i2 < 3; i2++) {
			strtok(insurance[i2], "\n");
		}

	}

	//create a UDP socket
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		die("socket");
	}

	// zero out the structure
	memset((char * ) &si_me, 0, sizeof(si_me));

	si_me.sin_family = AF_INET;//reused code;
	si_me.sin_port = htons(PORT);//reused code;

	si_me.sin_addr.s_addr = htonl(INADDR_ANY);

	//bind socket to port
	if (bind(s, (struct sockaddr*) &si_me, sizeof(si_me)) == -1) {//reused code;
	}

	struct hostent *lh = gethostbyname("localhost");
	char *UDPHost;
	UDPHost = inet_ntoa(*(struct in_addr *) *lh->h_addr_list);

	printf("Phase 3: Doctor 2 has a static UDP port %d and IP address %s.\n",
	PORT, UDPHost);
	printf("\n");

	//keep listening for data
	while (1) {

		fflush(stdout);
		//try to receive some data, this is a blocking call
		if ((recv_len = recvfrom(s, buf, BUFLEN, 0,
				(struct sockaddr *) &si_other, &slen)) == -1) {//reused code;
			die("recvfrom()");
		}

		buf[recv_len] = '\0';

		char * splitRt;
		int row = 0;
		int count = 1;
		char receInsurance[200][200];
		splitRt = strtok(buf, " ,.-");
		while (splitRt != NULL) {
			if (count % 2 != 0) {
				strcpy(receInsurance[row++], splitRt);
				splitRt = strtok(NULL, " ,.-");
				count++;
			} else {
				strcpy(receInsurance[row++], splitRt);
				splitRt = strtok(NULL, " ,.-");
			}

		}
		strtok(receInsurance[0], "\n"); //Delete the /n in the end
		strtok(receInsurance[1], "\n"); //Delete the /n in the end
		int len = strlen(insurance[0]);
		insurance[0][len] = '\0';
		insurance[1][len] = '\0';
		insurance[2][len] = '\0';

		printf(
				"Phase 3: Doctor 2 receives the request from the patient with port number %d and name %s with the insurance plan %s.\n",
				ntohs(si_other.sin_port), receInsurance[1], receInsurance[0]);
		printf("\n");

		int flagMatch = 0;
		int size = sizeof(insurance) / sizeof(insurance[0]);
		int m2 = 0;
		char sendstuff[200];
		int sendPORT = ntohs(si_other.sin_port);
		sprintf(sendstuff, "%d", sendPORT);
		/**
		 *Traverse to check whether the insurance received from patients exist and send according cost back to patients;
		 *
		 */
		for (m2 = 0; m2 < size; m2++) {

			if (strcmp(receInsurance[0], insurance[m2]) == 0) {
				strcat(price[m2], " ");
				strcat(price[m2], sendstuff);
				strtok(price[m2], "\n'");
				size_t sendSize = sizeof(price[m2]);
				price[m2][sendSize] = '\0'; //Mark the end of the line
				char copyPrice[200];
				strcpy(copyPrice, price[m2]);
				size_t copyPriceSize = sizeof(price[m2]);
				char onlyPrice[1][100];
				char onlyPort[1][100];

				//split name and password and save;
				char * splitF;
				int countF = 1;
				splitF = strtok(copyPrice, " ,.-");
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
				sendto(s, price[m2], recv_len, 0, (struct sockaddr*) &si_other,
						slen);
				printf(
						"Phase 3: Doctor 2 has sent estimated price $%s to patient with port number %d.\n",
						onlyPrice[0], ntohs(si_other.sin_port));
				printf("\n");

				flagMatch = 1;
				break;
			} else {

			}

		}

	}
	close(s);
	return 0;
}
