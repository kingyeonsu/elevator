#include "main.h"
#include <stdio.h>

extern RTC_HandleTypeDef hrtc;
extern void lcd_string(uint8_t *str);
extern void move_cursor(uint8_t row, uint8_t column);

RTC_TimeTypeDef sTime = {0};
RTC_DateTypeDef sData = {0};

// 예) 22의 BCD의 메모리에 저장된 format
// 	high nibble low nibble
// 	0010		0010
//	20		+	2
//	= 22
//	bin (BCD) to dec

unsigned char bin2dec(unsigned char byte)
{
	unsigned char high, low;

	low = byte & 0x0f;	// 하위 4비트만 취한다.
	high = ((byte >> 4) & 0x0f) * 10;	// 상위 4비트만 취한다.

	return high + low;
}

//	dec to bin (BCD)
unsigned char dec2bin(unsigned char byte)
{
	unsigned char high, low;

	high = ((byte / 10) << 4);
	low = byte % 10;

	return high + low;
}

// RTC 에서 날짜와 시각 정보
void get_rtc_time(void)
{
	static RTC_TimeTypeDef oTime;	// 이전 시각 정보
	char lcd_buff[40];

	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
	HAL_RTC_GetDate(&hrtc, &sData, RTC_FORMAT_BCD);

	if (oTime.Seconds != sTime.Seconds)
	{
		/// YYYY-MM-DD hh:mm:ss
/*		printf("%04d-%02d-%02d %02d:%02d:%02d\n",
				bin2dec(sData.Year)+2000, bin2dec(sData.Month), bin2dec(sData.Date),
				bin2dec(sTime.Hours), bin2dec(sTime.Minutes), bin2dec(sTime.Seconds)); */
		sprintf(lcd_buff, "%02d%02d%02d  %02d:%02d:%02d",
				bin2dec(sData.Year), bin2dec(sData.Month), bin2dec(sData.Date),
				bin2dec(sTime.Hours), bin2dec(sTime.Minutes), bin2dec(sTime.Seconds));
		move_cursor(0,0);
		lcd_string(lcd_buff);
	}
	oTime.Seconds = sTime.Seconds;
}

// date_time 이라는 변수에는 yymmddhhmmss의 주솩 넘어온다.
void set_rtc_data_time(char *date_time)
{
	char year[4], month[4], date[4];	// 날짜	// 22\0 --> 3bytes --> 1 2 4 8 16 ... bytes 단위로 설정
	char hour[4], minute[4], second[4];	// 시각

	strncpy(year, date_time, 2);
	strncpy(month, date_time+2, 2);
	strncpy(date, date_time+4, 2);

	strncpy(hour, date_time+6, 2);
	strncpy(minute, date_time+8, 2);
	strncpy(second, date_time+10, 2);

	// 1. ascii to integer
	// 2. dec to bin
	// 3. rtc
	sData.Year = dec2bin(atoi(year));
	sData.Month = dec2bin(atoi(month));
	sData.Date = dec2bin(atoi(date));

	sTime.Hours = dec2bin(atoi(hour));
	sTime.Minutes = dec2bin(atoi(minute));
	sTime.Seconds = dec2bin(atoi(second));

	HAL_RTC_SetDate(&hrtc, &sData, RTC_FORMAT_BCD);
	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
}






