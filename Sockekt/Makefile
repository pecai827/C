CC      = gcc
CFLAGS  = -lsocket -lnsl -lresolv
RM      = rm -f


default: all

all: healthcenterserver  patient1 patient2 doctor1 doctor2

healthcenterserver: healthcenterserver.c
	$(CC) $(CFLAGS) -o healthcenterserver healthcenterserver.c
patient1: patient1.c
	$(CC) $(CFLAGS) -o patient1 patient1.c

patient2: patient2.c
	$(CC) $(CFLAGS) -o patient2 patient2.c

doctor1: doctor1.c
	$(CC) $(CFLAGS) -o doctor1 doctor1.c

doctor2: doctor2.c
	$(CC) $(CFLAGS) -o doctor2 doctor2.c

clean veryclean:
	$(RM) healthcenterserver
	$(RM) patient1
	$(RM) patient2
	$(RM) doctor1
	$(RM) doctor2
