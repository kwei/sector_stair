#ifndef MRLOOPSDKHEADER
#define MRLOOPSDKHEADER

#include <stdint.h>

//This header is for the variable to communicate with SDK and upper SW

typedef struct{
    uint32_t PHY_Total_Tx_Count;
    uint32_t PHY_Total_Rx_Count;
    uint32_t PHY_Rx_CP_PKT;
    uint32_t PHY_Rx_SC_PKT;
    uint32_t PHY_Rx_STF_Err;
    uint32_t PHY_Rx_HCS_Err;
    uint32_t PHY_RX_FCS_Err;
    int8_t  PHY_Rx_EVM;
    int8_t  PHY_RX_SNR;
    int8_t  PHY_RSSI;
    int8_t  PHY_RCPI;
    uint16_t PHY_AGC_Gain;
    uint32_t MAC_Tx_Total;
    uint32_t MAC_Rx_Total;
    uint32_t MAC_Total_Fail;
    uint32_t MAC_Total_Ack;
    uint32_t MAC_Total_Tx_Done;
    int16_t PHY_pwr;
    uint32_t MCS;
    // uint32_t id;
} ML_RF_INF;

//0~42 offset
#endif // MRLOOPSDKHEADER

