#
## CSCI 370: Sample Makefile for OCCI compilation
#

CFLAGS = -I/usr/local/lib/Oracle/instantclient_11_2/sdk/include
LFLAGS = -L/usr/local/lib/Oracle/instantclient_11_2 -locci -lociei
all: addDentest addPatient addAssistent addProcedure dentalOffice

addDentest.o: addDentest.cpp
	g++ -c $(CFLAGS) addDentest.cpp

addDentest: addDentest.o
	g++ $(LFLAGS) -o addDentest addDentest.o

addPatient.o: addPatient.cpp
	g++ -c $(CFLAGS) addPatient.cpp

addPatient: addPatient.o
	g++ $(LFLAGS) -o addPatient addPatient.o

addAssistent.o: addAssistent.cpp
	g++ -c $(CFLAGS) addAssistent.cpp

addAssistent: addAssistent.o
	g++ $(LFLAGS) -o addAssistent addAssistent.o


addProcedure.o: addProcedure.cpp
	g++ -c $(CFLAGS) addProcedure.cpp

addProcedure: addProcedure.o
	g++ $(LFLAGS) -o addProcedure addProcedure.o

dentalOffice.o: dentalOffice.cpp
	g++ -c $(CFLAGS) dentalOffice.cpp

dentalOffice: dentalOffice.o
	g++ $(LFLAGS) -o dentalOffice dentalOffice.o

clean:
	rm -f *.o addDentest addPatient addAssistent addProcedure dentalOffice
