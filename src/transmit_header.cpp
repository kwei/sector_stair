
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "transmit_header.h"
#define CONFIG_FILE "config.txt"
#define MAX_KEY_LENGTH 64
#define KEY_WIGIG_ID "WiGig_id"

// #define UNITTEST 1


WiGig_header* WiGig_create_header(){
	FILE* fp;



	char *key;
	char line[MAX_KEY_LENGTH];
	// int value;

	if((fp = fopen(CONFIG_FILE, "r")) == NULL){
	    return NULL;
	}


	WiGig_header* wiGig_header = (WiGig_header*) malloc(sizeof(WiGig_header));
	
	while(fscanf(fp,"%s\n", &line[0]) != EOF){

		char *token;

		token = strtok(line, ":");
		key = token;		
		token = strtok(NULL, ":");
		// value = atoi(token);

		if(strcmp(key,KEY_WIGIG_ID) == 0){
			wiGig_header->WiGig_id = token[0];
		}
		memset(key,0,MAX_KEY_LENGTH);

	}
	fclose(fp);
	return wiGig_header;
}

void WiGig_set_sector(WiGig_header* ptr, unsigned char sector){
	if(ptr == NULL){
		return;
	}
	ptr->sector = sector;
	return;
}

unsigned char  WiGig_get_sector(WiGig_header* ptr){
	if(ptr == NULL){
		return 0;
	}
	return ptr->sector;
}

unsigned char  WiGig_get_ID(WiGig_header* ptr){
	if(ptr == NULL){
		return 0;
	}
	return ptr->WiGig_id;
}

// void WiGig_free_header(WiGig_header* ptr){
// 	free(ptr);
// 	return;
// }

#ifdef UNITTEST

int main(){
	WiGig_header* whptr = WiGig_create_header();
	WiGig_set_sector(whptr,1);
	printf("wigig id : %d\n",WiGig_get_ID(whptr));
	printf("sector : %d\n",WiGig_get_sector(whptr));
	WiGig_free_header(whptr);
	return 0;
}

#endif