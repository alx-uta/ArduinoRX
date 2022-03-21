// Motor control
int pos = 0;
int zeroThrottle = 90; //90 Needs to be between 66 and 101.
int maxThrottle = 120; //120 seems to be where the maximum is. Above this doesn't give any extra speed.
int minThrottle = 60;  //60 This represents the "reverse" speed.

//nRF24L01
#define TX_CE 10
#define TX_CSN 9

#define RX_CE 8
#define RX_CSN 7

uint8_t address[][6] = {"A1", "B1", "A2", "B2"};

bool debug = false;