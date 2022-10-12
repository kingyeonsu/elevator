/*
 * UART0.c
 *
 * Created: 2022-08-09 오전 10:44:49
 *  Author: kccistc
 */ 

#include "main.h"
#include "UART0.h"
#include "led.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define  COMMAND_MAX  50    // 최대로 저장할 수 있는 command수
#define  COMMAND_LENGTH 30	// 1 line에 최대로 save 할 수 있는 data 수

volatile int input_pointer = 0;				// ISR(UART0_RX_vect)에서 save하는 index값
volatile int output_pointer = 0;			// pc_command_processing 에서 가져가는 index값
volatile unsigned char rx_buff[COMMAND_MAX][COMMAND_LENGTH];	// 문자열을 저장하는 buffer
volatile uint8_t rx_data;   // 1. PC로 부터 1byte의 INT가 들어오면 저장 하는 변수
volatile int rx_index = 0;					// 문장의 끝을 판단하는 변수
// rx_index --> led all off
volatile int manual_step_motor_state=0;

int is_empty();
int is_full();

extern volatile int motor_floor_state;
extern UART_HandleTypeDef huart1;	// hal uart1	// BT
extern UART_HandleTypeDef huart2;	// hal uart2	// PC

extern void set_rtc_data_time(char *date_time);
extern void manual_half_up(int cycle);
extern void manual_half_down(int cycle);
extern void manual_step_motor_driver_up();
extern void manual_step_motor_driver_down();

// copy from HAL_UART_RxCpltCallback of stm32f4xx_hal_uart to here
// UART로 부터 1byte가 수신되면 H/W가 call을 해 준다.
// UART1 / UART2번으로 부터 1 byte가 수신(stop bit) 하면 rx interrupt가 발생
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{	
	if (huart == &huart2)   // PC
	{
		if(is_full())
		{
			//printf("Queue is Full");
		}
		else
		{
			unsigned char data;

			data = rx_data;

			if(data == '\n' || data == '\r')						// 문장의 끝을 만났을 때
			{
				rx_buff[input_pointer][rx_index] = '\0';					// 문장의 끝일 때는 NULL을 집어넣는다.
				rx_index = 0;												// 개선점 : circular queue(환영큐)로 개선을 해야 한다.
				//printf("input_pointer = %d\n", input_pointer);
				input_pointer++;
				input_pointer %= COMMAND_MAX;
			}
			else											    // 문장이 끝나지 않았을 때
			{
				rx_buff[input_pointer][rx_index] = data;
				rx_index++;
			}
		}
		// 주의: 반드시 HAL_UART_Receive_IT를 call 해주야 다음 INT가 발생
		HAL_UART_Receive_IT(&huart2, &rx_data, 1);
	}

	if (huart == &huart1)   // bluetooth
	{
		if(is_full())
		{
			//printf("Queue is Full");
		}
		else
		{
			unsigned char data;

			data = rx_data;

			if(data == '\n' || data == '\r')						// 문장의 끝을 만났을 때
			{
				rx_buff[input_pointer][rx_index] = '\0';					// 문장의 끝일 때는 NULL을 집어넣는다.
				rx_index = 0;												// 개선점 : circular queue(환영큐)로 개선을 해야 한다.
				//printf("input_pointer = %d\n", input_pointer);
				input_pointer++;
				input_pointer %= COMMAND_MAX;
			}
			else											    // 문장이 끝나지 않았을 때
			{
				rx_buff[input_pointer][rx_index] = data;
				rx_index++;
			}
		}
		// 주의: 반드시 HAL_UART_Receive_IT를 call 해주야 다음 INT가 발생
		HAL_UART_Receive_IT(&huart1, &rx_data, 1);
	}
}

int is_empty()		// 큐의 공백 상태를 알려주는 함수
{
	if (input_pointer == output_pointer)
		return 1;
	else
		return 0;
}

int is_full()		// 큐의 포화 상태를 알려주는 함수
{
	int temp;

	temp = (input_pointer + 1) % COMMAND_MAX;
	if (temp == output_pointer)
		return 1;
	else
		return 0;
}

int check_print_on=1;
void pc_command_processing()
{
	if (is_empty())
	{
		// printf("Queue is Empty");
	}
	else
	{
		if (check_print_on)
		{
			printf("output_pointer = %d\n", output_pointer);
			printf("%s\n", rx_buff[output_pointer]);
		}
		if(strncmp((const char *)rx_buff[output_pointer], "check_print_on", strlen("check_print_on")-1) == 0)
		{
			check_print_on = 1;
		}
		if(strncmp((const char *)rx_buff[output_pointer], "check_print_off", strlen("check_print_off")-1) == 0)
		{
			check_print_on = 0;
		}

		if(strncmp((const char *)rx_buff[output_pointer], "up", strlen("up")-1) == 0)
		{
			manual_step_motor_driver_up();
		}
		if(strncmp((const char *)rx_buff[output_pointer], "down", strlen("down")-1) == 0)
		{
			manual_step_motor_driver_down();
		}



		// (const char *) --> warning 제거
/*		if(strncmp((const char *)rx_buff[output_pointer], "shift_left_led_on", strlen("shift_left_led_on")-1) == 0)
		{

		}
		else if(strncmp((const char *)rx_buff[output_pointer], "flower_off", strlen("flower_off")-1) == 0)
		{

		}
		else if(strncmp((const char *)rx_buff[output_pointer], "flower_on", strlen("flower_on")-1) == 0)
		{
			
		}
*/
		if(strncmp((const char *)rx_buff[output_pointer], "setrtc", strlen("setrtc")-1) == 0)
		{
			set_rtc_data_time((char *)&rx_buff[output_pointer][6]);
		}

//		if(strncmp((const char *)rx_buff[output_pointer], "dht11time", strlen("dht11time")-1) == 0)
//		{
//			if (digit_check((char *)&rx_buff[output_pointer][9], 4))
//			{
//				int time_value;
//				time_value = atoi((char *)&rx_buff[output_pointer][9]);	// ascii to integer func.
//printf("time_value: %d\n", time_value);
//				dht11time = time_value / 10;
//printf("dht11time: %d\n", dht11time);
//			}
//			else
//			{
//				printf("invalid number: %s\n", &rx_buff[output_pointer][9]);
//			}
//
//		}
//		if(strncmp((const char *)rx_buff[output_pointer], "dht11on", strlen("dht11on")-1) == 0)
//		{
//			dht11on = 1;
//		}
//
//		if(strncmp((const char *)rx_buff[output_pointer], "dht11off", strlen("dht11off")-1) == 0)
//		{
//			dht11on = 0;
//		}
//		if(strncmp((const char *)rx_buff[output_pointer], "dht11status", strlen("dht11status")-1) == 0)
//		{
//			printf("dht11on : %d\n", dht11on);
//			printf("dht11time : %d\n", dht11time*10);
//		}
		if(strncmp((const char *)rx_buff[output_pointer], "led_all_on", strlen("led_all_on")-1) == 0)
		{
			printf("led_all_on\n");
			led_all_on();
		}
		if(strncmp((const char *)rx_buff[output_pointer], "led_all_off", strlen("led_all_off")-1) == 0)
		{
			printf("led_all_off\n");
			led_all_off();
		}
		output_pointer = (output_pointer + 1) % COMMAND_MAX;
	}
}

// TRUE	: 1
// FALSE: 0
int digit_check(char *digit, int length)
{
	for (int i=0; i < length; i++)
	{
		if (digit[i] >= '0' && digit[i] <= '9')
			;
		else
			return 0;
	}
	return 1;
}
