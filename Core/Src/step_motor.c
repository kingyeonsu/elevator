#include "button.h"
#include <stdio.h>

#define	IDLE		0
#define	FORWARD		1
#define	BACKWARD	2
#define STEPS_PER_REV	4096
#define HALF_TURN	360
#define TURN		720
#define	floor1		0
#define	floor2		1
#define	floor3		2
#define	floor4		3
#define	floor5		4

extern uint8_t prev_button1_state;
extern uint8_t prev_button2_state;
extern uint8_t prev_button3_state;
extern uint8_t prev_button4_state;
extern uint8_t prev_button5_state;
extern volatile int floor_state;

char lcd_floor_buff[40];
volatile int motor_state = IDLE;
volatile int motor_floor_state = floor1;
volatile int cycle_state = IDLE;
volatile int manual_cycle_state = IDLE;
volatile int pause_state = IDLE;
volatile int buzzer_indicator=0;

void step_motor_driver(int direction);
void step_motor_forward_backward_stop();
void set_rpm(int rpm);
void manual_step_motor_driver_up();
void manual_step_motor_driver_down();

extern void led_all_off();
extern void led_on_up();
extern void led_on_down();
extern void delay_us(unsigned long us);
extern void lcd_string(uint8_t *str);
extern void move_cursor(uint8_t row, uint8_t column);
extern void custom_buzzer();
extern void servo_motor_control_main();

void manual_step_motor_driver_up()
{
	motor_floor_state = floor5;
	motor_state = FORWARD;
	sprintf(lcd_floor_buff, "BT UP to 5F       ");
	move_cursor(1,0);
	lcd_string(lcd_floor_buff);
	servo_motor_control_main();
}

void manual_step_motor_driver_down()
{
	motor_floor_state = floor1;
	motor_state = BACKWARD;
	sprintf(lcd_floor_buff, "BT DOWN to 1F    ");
	move_cursor(1,0);
	lcd_string(lcd_floor_buff);
	servo_motor_control_main();
}

void external_interrupt_step_motor_move()
{
	switch (floor_state)
	{
		case floor1:
			step_motor_driver(FORWARD);
			set_rpm(13);	// rpm==13 --> MAX
			if (cycle_state >= TURN*floor2)
			{
				motor_state = IDLE;
				floor_state = IDLE;
				cycle_state = IDLE;
			}
			break;
		case floor2:
			step_motor_driver(FORWARD);
			set_rpm(13);	// rpm==13 --> MAX
			if (cycle_state >= TURN*floor3)
			{
				motor_state = IDLE;
				floor_state = IDLE;
				cycle_state = IDLE;
			}
			break;
	}
}

// 1 sec == 1000 ms == 1000000 us
// 1 min == 60 sec == 60000000 us
// rpm : revolutions per minutes
// 60000000 / 4096 / rpm (1~13)

void set_rpm(int rpm)
{
	delay_us(60000000/STEPS_PER_REV/rpm);
	// MAX SPEED(rpm==13) --> delay(1126)
}

void flow_button_move()
{
	switch (motor_floor_state)
	{
		case floor1:
			if (cycle_state == TURN*floor1)
			{
				custom_buzzer();
				motor_state = IDLE;
				sprintf(lcd_floor_buff, "STOP floor1     ");
				move_cursor(1,0);
				lcd_string(lcd_floor_buff);
				servo_motor_control_main();
			}
			else if (cycle_state == TURN*floor1 + 1)
			{
				buzzer_indicator=1;
			}

			if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, &prev_button1_state) == BUTTON_PRESS)
			{
				motor_state = IDLE;
			}
			else if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, &prev_button2_state) == BUTTON_PRESS)
			{
				sprintf(lcd_floor_buff, "UP floor1 -> 2");
				move_cursor(1,0);
				lcd_string(lcd_floor_buff);
				motor_state = FORWARD;
				motor_floor_state = floor2;
			}
			else if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, &prev_button3_state) == BUTTON_PRESS)
			{
				sprintf(lcd_floor_buff, "UP floor1 -> 3");
				move_cursor(1,0);
				lcd_string(lcd_floor_buff);
				motor_state = FORWARD;
				motor_floor_state = floor3;
			}
			else if (get_button(BUTTON4_GPIO_Port, BUTTON4_Pin, &prev_button4_state) == BUTTON_PRESS)
			{
				sprintf(lcd_floor_buff, "UP floor1 -> 4");
				move_cursor(1,0);
				lcd_string(lcd_floor_buff);
				motor_state = FORWARD;
				motor_floor_state = floor4;
			}
			else if (get_button(BUTTON5_GPIO_Port, BUTTON5_Pin, &prev_button5_state) == BUTTON_PRESS)
			{
				sprintf(lcd_floor_buff, "UP floor1 -> 5");
				move_cursor(1,0);
				lcd_string(lcd_floor_buff);
				motor_state = FORWARD;
				motor_floor_state = floor5;
			}

			step_motor_driver(motor_state);
			set_rpm(13);	// rpm==13 --> MAX
			break;
		case floor2:
			if (cycle_state == TURN*floor2)
			{
				custom_buzzer();
				motor_state = IDLE;
				sprintf(lcd_floor_buff, "STOP floor2      ");
				move_cursor(1,0);
				lcd_string(lcd_floor_buff);
				servo_motor_control_main();
			}
			else if (cycle_state == TURN*floor2 + 1 || cycle_state == TURN*floor2 - 1)
			{
				buzzer_indicator=1;
			}

			if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, &prev_button1_state) == BUTTON_PRESS)
			{
				sprintf(lcd_floor_buff, "DOWN floor2 -> 1");
				move_cursor(1,0);
				lcd_string(lcd_floor_buff);
				motor_state = BACKWARD;
				motor_floor_state = floor1;
			}
			else if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, &prev_button2_state) == BUTTON_PRESS)
			{
				motor_state = IDLE;
			}
			else if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, &prev_button3_state) == BUTTON_PRESS)
			{
				sprintf(lcd_floor_buff, "UP floor2 -> 3");
				move_cursor(1,0);
				lcd_string(lcd_floor_buff);
				motor_state = FORWARD;
				motor_floor_state = floor3;
			}
			else if (get_button(BUTTON4_GPIO_Port, BUTTON4_Pin, &prev_button4_state) == BUTTON_PRESS)
			{
				sprintf(lcd_floor_buff, "UP floor2 -> 4");
				move_cursor(1,0);
				lcd_string(lcd_floor_buff);
				motor_state = FORWARD;
				motor_floor_state = floor4;
			}
			else if (get_button(BUTTON5_GPIO_Port, BUTTON5_Pin, &prev_button5_state) == BUTTON_PRESS)
			{
				sprintf(lcd_floor_buff, "UP floor2 -> 5");
				move_cursor(1,0);
				lcd_string(lcd_floor_buff);
				motor_state = FORWARD;
				motor_floor_state = floor5;
			}

			step_motor_driver(motor_state);
			set_rpm(13);	// rpm==13 --> MAX
			break;
		case floor3:
			if (cycle_state == TURN*floor3)
			{
				custom_buzzer();
				motor_state = IDLE;
				sprintf(lcd_floor_buff, "STOP floor3      ");
				move_cursor(1,0);
				lcd_string(lcd_floor_buff);
				servo_motor_control_main();
			}
			else if (cycle_state == TURN*floor3 + 1 || cycle_state == TURN*floor3 - 1)
			{
				buzzer_indicator=1;
			}

			if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, &prev_button1_state) == BUTTON_PRESS)
			{
				sprintf(lcd_floor_buff, "DOWN floor3 -> 1");
				move_cursor(1,0);
				lcd_string(lcd_floor_buff);
				motor_state = BACKWARD;
				motor_floor_state = floor1;
			}
			else if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, &prev_button2_state) == BUTTON_PRESS)
			{
				sprintf(lcd_floor_buff, "DOWN floor3 -> 2");
				move_cursor(1,0);
				lcd_string(lcd_floor_buff);
				motor_state = BACKWARD;
				motor_floor_state = floor2;
			}
			else if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, &prev_button3_state) == BUTTON_PRESS)
			{
				motor_state = IDLE;
			}
			else if (get_button(BUTTON4_GPIO_Port, BUTTON4_Pin, &prev_button4_state) == BUTTON_PRESS)
			{
				sprintf(lcd_floor_buff, "UP floor3 -> 4");
				move_cursor(1,0);
				lcd_string(lcd_floor_buff);
				motor_state = FORWARD;
				motor_floor_state = floor4;
			}
			else if (get_button(BUTTON5_GPIO_Port, BUTTON5_Pin, &prev_button5_state) == BUTTON_PRESS)
			{
				sprintf(lcd_floor_buff, "UP floor3 -> 5");
				move_cursor(1,0);
				lcd_string(lcd_floor_buff);
				motor_state = FORWARD;
				motor_floor_state = floor5;
			}

			step_motor_driver(motor_state);
			set_rpm(13);	// rpm==13 --> MAX
			break;
		case floor4:
			if (cycle_state == TURN*floor4)
			{
				custom_buzzer();
				motor_state = IDLE;
				sprintf(lcd_floor_buff, "STOP floor4     ");
				move_cursor(1,0);
				lcd_string(lcd_floor_buff);
				servo_motor_control_main();
			}
			else if (cycle_state == TURN*floor4 + 1 || cycle_state == TURN*floor4 - 1)
			{
				buzzer_indicator=1;
			}

			if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, &prev_button1_state) == BUTTON_PRESS)
			{
				sprintf(lcd_floor_buff, "DOWN floor4 -> 1");
				move_cursor(1,0);
				lcd_string(lcd_floor_buff);
				motor_state = BACKWARD;
				motor_floor_state = floor1;
			}
			else if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, &prev_button2_state) == BUTTON_PRESS)
			{
				sprintf(lcd_floor_buff, "DOWN floor4 -> 2");
				move_cursor(1,0);
				lcd_string(lcd_floor_buff);
				motor_state = BACKWARD;
				motor_floor_state = floor2;
			}
			else if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, &prev_button3_state) == BUTTON_PRESS)
			{
				sprintf(lcd_floor_buff, "DOWN floor4 -> 3");
				move_cursor(1,0);
				lcd_string(lcd_floor_buff);
				motor_state = BACKWARD;
				motor_floor_state = floor3;
			}
			else if (get_button(BUTTON4_GPIO_Port, BUTTON4_Pin, &prev_button4_state) == BUTTON_PRESS)
			{
				motor_state = IDLE;
			}
			else if (get_button(BUTTON5_GPIO_Port, BUTTON5_Pin, &prev_button5_state) == BUTTON_PRESS)
			{
				sprintf(lcd_floor_buff, "UP floor4 -> 5");
				move_cursor(1,0);
				lcd_string(lcd_floor_buff);
				motor_state = FORWARD;
				motor_floor_state = floor5;
			}

			step_motor_driver(motor_state);
			set_rpm(13);	// rpm==13 --> MAX
			break;
		case floor5:
			if (cycle_state == TURN*floor5)
			{
				custom_buzzer();
				motor_state = IDLE;
				sprintf(lcd_floor_buff, "STOP floor5     ");
				move_cursor(1,0);
				lcd_string(lcd_floor_buff);
				servo_motor_control_main();
			}
			else if (cycle_state == TURN*floor5 - 1)
			{
				buzzer_indicator=1;
			}

			if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, &prev_button1_state) == BUTTON_PRESS)
			{
				sprintf(lcd_floor_buff, "DOWN floor5 -> 1");
				move_cursor(1,0);
				lcd_string(lcd_floor_buff);
				motor_state = BACKWARD;
				motor_floor_state = floor1;
			}
			else if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, &prev_button2_state) == BUTTON_PRESS)
			{
				sprintf(lcd_floor_buff, "DOWN floor5 -> 2");
				move_cursor(1,0);
				lcd_string(lcd_floor_buff);
				motor_state = BACKWARD;
				motor_floor_state = floor2;
			}
			else if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, &prev_button3_state) == BUTTON_PRESS)
			{
				sprintf(lcd_floor_buff, "DOWN floor5 -> 3");
				move_cursor(1,0);
				lcd_string(lcd_floor_buff);
				motor_state = BACKWARD;
				motor_floor_state = floor3;
			}
			else if (get_button(BUTTON4_GPIO_Port, BUTTON4_Pin, &prev_button4_state) == BUTTON_PRESS)
			{
				sprintf(lcd_floor_buff, "DOWN floor5 -> 4");
				move_cursor(1,0);
				lcd_string(lcd_floor_buff);
				motor_state = BACKWARD;
				motor_floor_state = floor4;
			}
			else if (get_button(BUTTON5_GPIO_Port, BUTTON5_Pin, &prev_button5_state) == BUTTON_PRESS)
			{
				motor_state = IDLE;
			}

			step_motor_driver(motor_state);
			set_rpm(13);	// rpm==13 --> MAX
			break;
	}
}

#if 1
void step_motor_driver(int direction)	// direction : forward or backward
{
	static int step=0;	// 전역변수처럼 동작

	switch (step)
	{
		case 0:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
			break;
		case 1:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
			break;
		case 2:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
			break;
		case 3:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
			break;
		case 4:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
			break;
		case 5:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
			break;
		case 6:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
			break;
		case 7:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
			break;
	}

	if (direction == FORWARD)	// for (int j=0; j < 8; j++)
	{
		// 다음 실핼 step 지정
		step++;
		if (step > 7)
		{
			step = 0;
			cycle_state++;
		}
	}
	else if (direction == BACKWARD)	// for (int j=7; j >= 0; j--)
	{
		// 다음 실핼 step 지정
		step--;
		if (step < 0)
		{
			step = 7;
			cycle_state--;
		}
	}
}
#else	// orginal
void step_motor_driver(int step)
{
	switch (step)
	{
		case 0:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
			break;
		case 1:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
			break;
		case 2:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
			break;
		case 3:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
			break;
		case 4:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
			break;
		case 5:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
			break;
		case 6:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
			break;
		case 7:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
			break;
	}
}
#endif

//	button1		: forward/backward
//	button2,3	: idle
void step_motor_forward_backward_stop()
{
	switch (motor_state)
	{
		case IDLE:
			if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, &prev_button1_state) == BUTTON_PRESS)
			{
				motor_state = FORWARD;
			}
			break;
		case FORWARD:
			step_motor_driver(motor_state);
			set_rpm(13);	// rpm==13 --> MAX
			if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, &prev_button1_state) == BUTTON_PRESS)
			{
				motor_state = BACKWARD;
			}
			else if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, &prev_button2_state) == BUTTON_PRESS)
			{
				motor_state = IDLE;
			}
			else if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, &prev_button3_state) == BUTTON_PRESS)
			{
				motor_state = IDLE;
			}
			break;
		case BACKWARD:
			step_motor_driver(motor_state);
			set_rpm(13);	// rpm==13 --> MAX
			if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, &prev_button1_state) == BUTTON_PRESS)
			{
				motor_state = FORWARD;
			}
			else if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, &prev_button2_state) == BUTTON_PRESS)
			{
				motor_state = IDLE;
			}
			else if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, &prev_button3_state) == BUTTON_PRESS)
			{
				motor_state = IDLE;
			}
			break;
	}
}

//	시계 방향 회전 <-반복-> 반시계 방향 회전
//	1바퀴 회전하는데 필요한 총 step : 4096
//	4096 / 8 (0.7도) == 512 sequence : 360도
//	모터가 360도 회전하는데 512개의 sequence 를 진행해야 한다.
//	1 sequence 당 : 0.70312도
//	0.70312 x 512 == 360도
//	이를 이용하여 동작
//	called by main()

void step_motor_forward_backward_control()
{
	switch (motor_state)
	{
		case IDLE:
			if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, &prev_button1_state) == BUTTON_PRESS)
			{
				motor_state = FORWARD;
			}
			break;
		case FORWARD:
			step_motor_driver(FORWARD);
			set_rpm(13);	// rpm==13 --> MAX
			if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, &prev_button1_state) == BUTTON_PRESS)
			{
				motor_state = BACKWARD;
			}
			break;
		case BACKWARD:
			step_motor_driver(BACKWARD);
			set_rpm(13);	// rpm==13 --> MAX
			if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, &prev_button1_state) == BUTTON_PRESS)
			{
				motor_state = FORWARD;
			}
			break;
	}
}

//	시계 방향 회전 <-반복-> 반시계 방향 회전
//	1바퀴 회전하는데 필요한 총 step : 4096
//	4096 / 8 (0.7도) == 512 sequence : 360도
//	모터가 360도 회전하는데 512개의 sequence 를 진행해야 한다.
//	1 sequence 당 : 0.70312도
//	0.70312 x 512 == 360도
//	이를 이용하여 동작
void step_motor_main_test()
{
	while (1)
	{
		for (int i=0; i < 512; i++)	// 시계 방향 회전
		{
			for (int j=0; j < 8; j++)
			{
				step_motor_driver(j);
				set_rpm(13);	// rpm==13 --> MAX
			}
		}

		for (int i=0; i < 512; i++)	// 반시계 방향 회전
		{
			for (int j=7; j >= 0; j--)
			{
				step_motor_driver(j);
				set_rpm(13);	// rpm==13 --> MAX
			}
		}
	}
}
