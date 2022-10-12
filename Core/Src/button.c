#include "button.h"

uint8_t prev_button1_state=BUTTON_RELEASE;
uint8_t prev_button2_state=BUTTON_RELEASE;
uint8_t prev_button3_state=BUTTON_RELEASE;
uint8_t prev_button4_state=BUTTON_RELEASE;
uint8_t prev_button5_state=BUTTON_RELEASE;

//	1. GPIO	2. GPIO_PIN	3. 이전의 버튼
int get_button(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t *prev_button_state)
{
	unsigned char current_state;

	current_state = HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);

	if (current_state == BUTTON_PRESS && *prev_button_state == BUTTON_RELEASE)	// 처음 누른 상태
	{
		*prev_button_state = current_state;
		HAL_Delay(100);	// noise 제거용

		return BUTTON_RELEASE;	// 버튼이 아직 눌리지 않은 상태
	}
	else if (current_state == BUTTON_RELEASE && *prev_button_state == BUTTON_PRESS)
	{
		*prev_button_state = current_state;

		return BUTTON_PRESS;	// 버튼이 제대로 눌린 상태
	}

	return BUTTON_RELEASE;	// 버튼이 눌렀다 떼어진 상태가 아니다.
}
