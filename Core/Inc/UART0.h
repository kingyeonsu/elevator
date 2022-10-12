/*
 * UART0.h
 *
 * Created: 2022-08-09 오전 10:45:22
 *  Author: kccistc
 */ 


#ifndef UART0_H_
#define UART0_H_

void init_uart0();
void uart0_transmit(uint8_t data);
void pc_command_processing();

extern int dht11time;
extern int dht11on;		// dht11on : 1 DHT_processing 실행 // dht11on : 0 return

//volatile uint8_t RX_Ready_flag = 0;			// 완전한 문장이 들어왔을 때 1로 setting

#endif /* UART0_H_ */
