#include <stdlib.h>


int seats[3][10] = {0};

char * auditorium_info(int number){
	char * info = malloc(11);
	int i;
	for(i = 0; i < 10 ; i++){
		info[i] = '0' + seats[number][i];
	}
	info[i] = '\0';
	return info;
}


int reserve_seat(int auditorium, int seat){
	if(auditorium > 2 || seat > 9)
		return -1;
	
	if(seats[auditorium][seat] == 1){
		return -1;
	} else {
		seats[auditorium][seat] = 1;
		return 1;
	}
}