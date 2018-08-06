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

#include "iconv.h"
#include "pthread.h"
#include "speedtest.h"

#include "Common/mrloopsdkheader.h"
#include "mrloopbf_release.h"

#define BUFSIZE  4096
bool is_open, s_index = false, running = false;
unsigned char count = 0;
long bits;

bool rx_rfstatus = false;

pthread_t thread;
pthread_t show_bitrate;
pthread_t getRFstatus;

ML_RF_INF ML_RF_Record;

void readRFPackets(ML_RF_INF Packets)
{
		uint32_t n_detect = Packets.PHY_Rx_SC_PKT + Packets.PHY_Rx_CP_PKT;
		uint32_t n_pass = Packets.PHY_Total_Rx_Count - Packets.PHY_RX_FCS_Err;
		uint32_t n_error= n_detect - n_pass;
		if(n_detect == 0)
				n_detect = 1;

		fprintf(stdout,"\033[10;0H PHY COUNTERS: \n");
		fprintf(stdout,"\033[11;3H Total Tx Counter: %u\n", Packets.PHY_Total_Tx_Count);
		fprintf(stdout,"\033[12;3H Total Rx Counter: %u\n", Packets.PHY_Total_Rx_Count);
		fprintf(stdout,"\033[13;3H Rx CP PKT: %u\n", Packets.PHY_Rx_CP_PKT);
		fprintf(stdout,"\033[14;3H Rx SC PKT: %u\n", Packets.PHY_Rx_SC_PKT);

		fprintf(stdout,"\033[15;3H PER: %1.3f\n", (float)n_error / (float)n_detect);
		fprintf(stdout,"\033[16;3H RX STF: %1.3f\n", (float)Packets.PHY_Rx_STF_Err / (float)n_detect);
		fprintf(stdout,"\033[17;3H RX HCS: %1.3f\n", (float)Packets.PHY_Rx_HCS_Err / (float)n_detect);
		fprintf(stdout,"\033[18;3H RX FCS: %1.3f\n\n", (float)Packets.PHY_RX_FCS_Err / (float)n_detect);

		fprintf(stdout,"\033[11;40H RX EVM(dBm): %d\n", Packets.PHY_Rx_EVM);
		fprintf(stdout,"\033[12;40H RX SNR(dBm): %d\n", Packets.PHY_RX_SNR);
		fprintf(stdout,"\033[13;40H RX RSSI(dBm): %d\n", Packets.PHY_RSSI);
		fprintf(stdout,"\033[14;40H RX RCPI(dBm): %d\n", Packets.PHY_RCPI);
		fprintf(stdout,"\033[15;40H AGC GAIN: %d\n\n", Packets.PHY_AGC_Gain);
	fprintf(stdout,"\033[16;40H MCS: %d\n\n", Packets.MCS);

		fprintf(stdout,"\033[20;0H MAC COUNTERS: \n");
		fprintf(stdout,"\033[22;3H Total Tx: %u\n", Packets.MAC_Tx_Total);
		fprintf(stdout,"\033[22;3H Total Rx: %u\n", Packets.MAC_Rx_Total);
		fprintf(stdout,"\033[23;3H Total Fail: %u\n", Packets.MAC_Total_Fail);
		fprintf(stdout,"\033[24;3H Total Ack: %u\n", Packets.MAC_Total_Ack);
		fprintf(stdout,"\033[25;3H Total Tx Done: %u\n\n", Packets.MAC_Total_Tx_Done);
	// fprintf(stdout,"\033[26;3H Identify: %u\n",Packets.id);
}

void *TxGetRF(void *ptr)
{
	uint8_t *buf = new uint8_t[BUFSIZE];
	buf[0]='1';
	int length;

		while(s_index){
		//ML_RF_Record.id=1;
		ML_GetRFStatus(&ML_RF_Record);
		length = BUFSIZE;
		/*Send request to RF*/
		ML_SendRFStatusReq();
		/*Get RF data buf*/
		// fprintf(stdout,"\033[28;3H send buf : %s",buf);
		ML_Receiver(buf, &length);
		/*Decode RF status packet*/
		if(ML_DecodeRFStatusPacket(buf, &ML_RF_Record))
			readRFPackets(ML_RF_Record);

				sleep(1);
		}
	return ((void *)0);
}

void *RxGetRF(void *ptr)
{
	while(s_index)
	{
		if(!rx_rfstatus)
			rx_rfstatus = true;
		
		sleep(1);
	}
	return ((void *)0);
}


int main(int argc, char *argv[])
{
	bool tx_idx = false, rx_idx = false;

#if 1 
	int menu, tmp_num, tmp_sp, tmp_rule;
	uint8_t *speed = NULL, *rule = NULL;

	while(true){

		do{
			fprintf(stdout,"\033[2J");
			fprintf(stdout,"\033[1;0H");
			if(tx_idx){
				fprintf(stdout,"Select Function: \n");
				fprintf(stdout,"		1. Speed Set\n");
				fprintf(stdout,"		0. Quit\n");
			}else if(rx_idx){
				fprintf(stdout,"Select Function: \n");
				fprintf(stdout,"		0. Quit\n");
			}else{
				fprintf(stdout,"Select Function: \n");
				fprintf(stdout,"	1. Speed Set\n");
				fprintf(stdout,"	2. Rule Set\n");
				fprintf(stdout,"	3. Start SpeedTest\n");
				fprintf(stdout,"	0. Quit\n");
			}
			scanf("%d", &menu);
				
			switch(menu){
				case 1:
					if(is_open){
						fprintf(stdout,"\033[2J");
						fprintf(stdout,"\033[1;0H");
					}
					fprintf(stdout,"Select Speed 1 - 7\n");
					scanf("%d", &tmp_sp);
					if(tmp_sp >= 1 && tmp_sp < 8){
						if(!is_open)
							speed = (uint8_t*) &tmp_sp;
						else{
							ML_SetSpeed(*speed);
							fprintf(stdout,"Set Speed!\n");
						}
						
					}else{
						fprintf(stdout,"Error Number\n");
						speed = NULL;
					}
					break;
				case 2:
					if(!is_open){
						fprintf(stdout,"Select Rule : 1. PCP, 2. STA \n");
						scanf("%d", &tmp_rule);
						if(tmp_rule == 1 || tmp_rule == 2){
							rule = (uint8_t*) &tmp_rule;
						}
					}
					break;
				case 3:
#if 1
					if(!running){
						if( speed != NULL && rule != NULL){
							if(ML_Init() != 1){
								return 0;
							}
							else{

								ML_SetSpeed(*speed);
								ML_SetMode(*rule);
								is_open = true;

								fprintf(stdout,"Select Function 1. Tx, 2.Rx\n");
								scanf("%d", &tmp_num);
								if(tmp_num == 1){
									s_index = true;
									pthread_create(&thread, NULL, SpeedTx, NULL);
									pthread_create(&show_bitrate, NULL, ShowBitrate, NULL);
#ifdef RF_STATUS
	  																	pthread_create(&getRFstatus, NULL, TxGetRF, NULL);
#endif
									running = true;
									tx_idx = true;
									fprintf(stdout,"Speed Test Tx Start!\n");
								}
								else if(tmp_num == 2){
									s_index = true;
									pthread_create(&thread, NULL, SpeedRx, NULL);
																		pthread_create(&show_bitrate, NULL, ShowBitrate, NULL);
#ifdef RF_STATUS
	  																	pthread_create(&getRFstatus, NULL, RxGetRF, NULL);
#endif
									running = true;
																		rx_idx = true;
								}
								else{
									fprintf(stdout,"Fail, Please select funcion");
								}
							}
						}else{
							fprintf(stdout,"Cannot Start, Please set speed & rule\n");
						}
					}else{
						fprintf(stdout,"Please close function\n");
					}
					break;
#endif
				case 0:
					if(running){
						s_index = false;
						pthread_join(thread, NULL);
						pthread_join(show_bitrate, NULL);
#ifdef RF_STATUS
						pthread_join(getRFstatus, NULL);
#endif
						running = false;
					}
					break;
				default:
					fprintf(stdout,"error\n");
					continue;
			}
		}while(menu != 0 );
			break;
	}
	
	printf("\033[2J");
#endif
	return 0;
}

void* ShowBitrate(void *ptr){

	long bitrate = 0;

	while(s_index){
		bitrate = ((bits * 8) / (1024 *1024));
		fprintf(stdout,"\033[8;0H  %ld Mbp/s\n", bitrate);
		fflush(stdout);
		bits = 0;
		sleep(1);
	}
	return ((void *)0);
}

#if 1
#define CHUNK 1

void* SpeedTx(void *ptr){

	unsigned char* buf = (unsigned char*) malloc(BUFSIZE * CHUNK  * sizeof(char));
	memset(buf, 0, BUFSIZE * CHUNK);
	snprintf((char*)buf,7,"haupang");
	int length = BUFSIZE *CHUNK, status;

	while(s_index){
			status = ML_Transfer(buf, length);
		if(status > 0){
			bits += length;
		}
	}

	free(buf);
	return ((void *)0);
}

void* SpeedRx(void *ptr){

	uint8_t* buf = (uint8_t*) malloc(BUFSIZE * CHUNK);
	int status;
	int length;

		while(s_index){
		length = BUFSIZE *CHUNK;
	
		if(rx_rfstatus){
			ML_SendRFStatusReq();
			rx_rfstatus = false;
		}
		
			status = ML_Receiver(buf, &length);	
			if(status > 0){
				bits += BUFSIZE * CHUNK;
				fprintf(stdout,"\033[27;3H message: %s\n",buf);
				for(int i = 0; i<CHUNK; i++)
				{
					if(ML_DecodeRFStatusPacket(buf+(i*BUFSIZE), &ML_RF_Record))
					{
						readRFPackets(ML_RF_Record);
					}
				}
			}
		else{
			fprintf(stdout,"Rx fail return:%d", status);
		}
		}

		free(buf);
	return ((void *)0);
}
#endif
