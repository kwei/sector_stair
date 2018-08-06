#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/time.h>
#include <assert.h>
#include <stdarg.h>

#include "iconv.h"
#include "pthread.h"
#include "speedtest.h"

#include "Common/mrloopsdkheader.h"
#include "mrloopbf_release.h"

#include "transmit_header.h"

#define BUFSIZE  4096
#define DEBUG(...) do { fprintf(stdout,__VA_ARGS__); fflush(stdout); } while (0)

ML_RF_INF ML_RF_Record;

#define CHUNK 1
#define ROUNDS 10

#define MAX_SECTOR 10
#define MIN_SECTOR 1

int _length(unsigned char* s,int size) {
    int i = 0,iter=0;
    for(iter=0;iter<size;iter++){
      if(s[iter] != 0){
        ++i;
      }
    }
    return i;
}

/* current_timestamp */
long long current_timestamp() {
    struct timeval te;
    gettimeofday(&te, NULL);
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000;
    return milliseconds;
}

/* Tx_exhaustive */
void* Tx_exhaustive(void* ptr){
	char sector = MIN_SECTOR;
	while(1){
		if(ML_Init() != 1){
      exit(1);
		}
		ML_SetTxSector(sector);
		ML_SetSpeed(2);
		ML_HiddenDebugMsg();
		WiGig_header* whptr = WiGig_create_header();
		WiGig_set_sector(whptr,sector);
		int length = sizeof(WiGig_header);
		DEBUG("sector: %d\n",sector);

		unsigned char* buf = (unsigned char*) malloc(BUFSIZE * CHUNK  * sizeof(char));
		if(buf == NULL && whptr == NULL){
			ML_Close();
			continue;
		}
		memset(buf, 0, BUFSIZE * CHUNK);
		memcpy(buf,whptr,length);

		int status;
		status = ML_Transfer(buf, BUFSIZE * CHUNK);
		DEBUG("tx status: %d\n",status);
		ML_Close();

		free(whptr);
		free(buf);

		if(sector < MAX_SECTOR){
			sector++;
		}else if(sector == MAX_SECTOR){
			sector = MIN_SECTOR;
		}
	}
}

/* Rx_exhaustive */
void* Rx_exhaustive(void* ptr){
	int rx_sector = MIN_SECTOR, status, max_rssi = -1000, super_rx = 0, super_tx = 0;

	while(1){
		if(ML_Init() != 1){
			return 0;
		}
		ML_HiddenDebugMsg();
		ML_SetSpeed(2);
		ML_SetRxSector(rx_sector);

		int flag[MAX_SECTOR] = {0};
		int flag_counter = 0;
		long tstart,tend;

		tstart = current_timestamp();

		while(1){
			WiGig_header* whptr = WiGig_create_header();

			int length = sizeof(WiGig_header);

			uint8_t* buf = (uint8_t*) malloc(BUFSIZE * CHUNK);
      memset(buf, 0, BUFSIZE*CHUNK);
      int Rx_length = BUFSIZE * CHUNK;

			if(buf == NULL && whptr == NULL){
				ML_Close();
				continue;
			}
			status = ML_Receiver(buf, &Rx_length);
      // fprintf(stdout,"message size : %d\n",_length(buf,BUFSIZE * CHUNK));

      // DEBUG("status:%d\n",status);
			memcpy(whptr,buf,length);

			if(status > 0){
				int tx_sector;
				tx_sector = WiGig_get_sector(whptr);
				if(flag[tx_sector] == 0){
					flag[tx_sector] = 1;
					flag_counter++;
				}
				ML_GetRFStatus(&ML_RF_Record);
        // DEBUG("RSSI(dBm):%d\n", ML_RF_Record.PHY_RSSI);
        // DEBUG("Tx Sector:%d\n", tx_sector);
        // DEBUG("Rx Sector:%d\n", rx_sector);

        if(max_rssi < ML_RF_Record.PHY_RSSI){
          max_rssi = ML_RF_Record.PHY_RSSI;
          super_rx = rx_sector
          super_tx = tx_sector
        }
			}else{
        // DEBUG("RSSI(dBm):\n");
        // DEBUG("Tx Sector:\n");
        // DEBUG("Rx Sector:\n");
      }
			free(whptr);
			free(buf);

			tend = current_timestamp();
			if(tend - tstart > 1000){
				break;
			}

			if(flag_counter == 10){
				break;
			}
		}

		ML_Close();

		if(rx_sector<MAX_SECTOR) {
			rx_sector++;
		}else{
			rx_sector = MIN_SECTOR;
		}

    // DEBUG("The time cost of each Rx Sector:%ld\n",(tend-tstart));
    DEBUG("Current maximum RSSI : %ld\n", max_rssi)
    DEBUG("with [ Rx - Tx ] => [ %d - %d ]\n", super_rx, super_tx)
	}
  return ((void *)0);
}

#define CONFIG_FILE "config.txt"

// key of transmit in config.txt
#define KEY_TRANSMIT_MODE "Mode"

#define MAX_KEY_LENGTH 64
#define MAX_VALUE_LENGTH 64

#define TX 1
#define RX 2

pthread_t thread;

int main(int argc, char *argv[]){
	int mode = 0;
	ML_Close();

  FILE* fp;
	char *key;
	char line[MAX_KEY_LENGTH];
	int value;

	if((fp = fopen(CONFIG_FILE, "r")) == NULL){
		printf("file not found");
	    return 0;
	}
	while(fscanf(fp,"%s\n", &line[0]) != EOF){
		char *token;
		token = strtok(line, ":");
		key = token;
		token = strtok(NULL, ":");
		value = atoi(token);

		if(strcmp(key,KEY_TRANSMIT_MODE) == 0){
			if(value == RX){
				mode = RX;
			}else if(value == TX){
				mode = TX;
			}
		}
		memset(key,0,MAX_KEY_LENGTH);
	}
  fclose(fp);

	if(mode == TX){
		DEBUG("TX\n");
    int iter_rounds = 1;
    while(true){
      printf("==== [ Round %2d ] ====\n", iter_rounds);
      pthread_create(&thread, NULL, Tx_exhaustive, NULL);
      iter_rounds++;
    }
    pthread_join(thread, NULL);
	}else if (mode == RX){
		pthread_create(&thread, NULL, Rx_exhaustive, NULL);
		pthread_join(thread, NULL);
	}
}
