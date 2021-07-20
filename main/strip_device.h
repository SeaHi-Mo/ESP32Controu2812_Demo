#ifndef _STRIP_DEVICE_H
#define _STRIP_DEVICE_H

#include "driver/rmt.h"
#include "led_strip.h"
#include "esp_system.h"


/**********************************************/
#define RMT_TX_NUM 3
#define RMT_TX_CHANNEL RMT_CHANNEL_1
#define LED_STRIP_NUM 24   //RGB灯的个数

#define LED_OPEN 6
#define LED_CLOSE 7
/*************color define************/
#define COLOR_RED 9
#define COLOR_GREE 8
#define COLOR_BULE 10
#define COLOR_PURPLE 11
#define COLOR_ORANGE 12

#define Lamp_Open 13
#define lamp_close 14

#define lamp_speed_Up 15
#define lamp_speed_down 16

#define ligth_down 18
#define ligth_up 19

struct WS2812_COLOR
{
	uint8_t lamp;
	uint8_t ligth_rank;
	uint8_t lamp_speed;
	uint32_t red;
	uint32_t green;
	uint32_t blue;
};
extern struct WS2812_COLOR WS2812_RGB;
 static led_strip_t *strip;
extern struct timespec time1;
void RGB16for10(struct WS2812_COLOR *RGB, uint32_t reb_16);
void set_rgb(uint32_t rgb_24bit, uint8_t ligth_rank);
void stripLamp(uint32_t rgb_16data, uint8_t ligth_rank);
void init_led();
void led_color_change();
#endif
