#######################################
##########iosched Makefile#############
#######################################


all : iosched

iosched : schedulers
	g++ -Wall -o "iosched" "main.cpp"

schedulers:
	g++ -Wall -o "schedulers" "schedulers.h"
			
clean :
	rm iosched schedulers -rf
