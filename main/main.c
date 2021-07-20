#include <stdio.h>
#include "stdlib.h"
#include <string.h>
#include <time.h>
#include "driver/rmt.h"
#include "led_strip.h"
#include "esp_system.h"
#include "esp_log.h"

#include "cJSON.h"
/*********RTOS Handle-file****************/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

/*********UART Handle-file*****************/
#include "driver/gpio.h"
#include "driver/uart.h"
#include "strip_device.h"
#include "nvs_flash.h"
#include "esp_flash.h"


static const int RX_BUF_SIZE = 1024;

#define UART_BAUD 38400
#define TXD_PIN (GPIO_NUM_4)
#define RXD_PIN (GPIO_NUM_5)
					
static xTaskHandle  lampLEDHandle = NULL; //任务队列
//发布需要发布的数据

typedef struct __User_data
{
	char allData[1024];
	int dataLen;
} User_data;

User_data user_data;

/***************************************************************/
void uartControlLedStrip(int cmd_id);
/*************************************************************/
/* 
 * @Description: UART配置
 * @param: 
 * @return: 
 */
void UART_Init(void)
{
	const uart_config_t uart_config = {
		.baud_rate = UART_BAUD,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.source_clk = UART_SCLK_APB,
	};
	// We won't use a buffer for sending data.
	uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
	uart_param_config(UART_NUM_1, &uart_config);
	uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}
/*
 *
 * USART DATA handle
 *
 *
 */
int uartDataHandle(uint8_t *data)
{
	uint8_t *temp;
	char *TAG = "uartDataHandle";
	int ID_NUM = 0;
	temp = data;
	if (temp != NULL)
	{
		temp = (uint8_t *)strstr((const char *)temp, "ID:");
		if (temp != NULL)
		{
			temp = (uint8_t *)strstr((const char *)temp, ":");
			++temp;
			ID_NUM = atoi((const char *)temp);
			ESP_LOGI(TAG, "Cmd ID:%d", ID_NUM);
			goto __RETURN_ID;
		}
		else
			goto __RETURN_ID;
	}
	else
		goto __RETURN_ID;

__RETURN_ID:
	return ID_NUM;
}
/*
*
*颜色提示音
*
*/
unsigned char beep[][4] = {
	{0xAA, 0x02, 0x03, 0xBB}, //开灯
	{0xAA, 0x02, 0x04, 0xBB}, //关灯
	{0xAA, 0x02, 0x05, 0xBB}, //调成绿色
	{0xAA, 0x02, 0x06, 0xBB}, //调成红色
	{0xAA, 0x02, 0x07, 0xBB}, //调成蓝色
	{0xAA, 0x02, 0x08, 0xBB}, //调成紫色
	{0xAA, 0x02, 0x09, 0xBB}, //调成橙色

	{0xAA, 0x02, 0x0A, 0xBB}, //开始跑
	{0xAA, 0x02, 0x0B, 0xBB}, //停止跑
	{0xAA, 0x02, 0x0C, 0xBB}, //提速
	{0xAA, 0x02, 0x0D, 0xBB}, //降速
	{0xAA, 0x02, 0x0E, 0xBB}, //还在跑
};
/*
 *
 * uart controu LED strip 
 *
 */
void uartControlLedStrip(int cmd_id)
{
	/*if (WS2812_RGB.red == 0 && WS2812_RGB.green == 0 && WS2812_RGB.blue == 0)
		return;*/
	//if(WS2812_RGB.lamp==1) return ;

	switch (cmd_id)
	{
	case LED_OPEN:
		if (WS2812_RGB.lamp)
			{uart_write_bytes(UART_NUM_1, beep[11], sizeof(beep[0]));break;}
		set_rgb(0xFFFFFF, WS2812_RGB.ligth_rank);
		break;
	case LED_CLOSE:
		if (WS2812_RGB.lamp)
			{uart_write_bytes(UART_NUM_1, beep[11], sizeof(beep[0]));break;}
			
		set_rgb(0X000000, WS2812_RGB.ligth_rank);
		break;
	case COLOR_RED:
		if (WS2812_RGB.lamp)
			{uart_write_bytes(UART_NUM_1, beep[11], sizeof(beep[0]));break;}
		set_rgb(0Xff0000, WS2812_RGB.ligth_rank);
		break;
	case COLOR_GREE:
		if (WS2812_RGB.lamp)
			{uart_write_bytes(UART_NUM_1, beep[11], sizeof(beep[0]));break;}
		set_rgb(0x00FF00, WS2812_RGB.ligth_rank);
		break;
	case COLOR_BULE:
		if (WS2812_RGB.lamp)
			{uart_write_bytes(UART_NUM_1, beep[11], sizeof(beep[0]));break;}
			
		set_rgb(0X0000ff, WS2812_RGB.ligth_rank);
		break;
	case COLOR_ORANGE:
		if (WS2812_RGB.lamp)
			{uart_write_bytes(UART_NUM_1, beep[11], sizeof(beep[0]));break;}
			
		set_rgb(0XF05308, WS2812_RGB.ligth_rank);
		break;
	case COLOR_PURPLE:
		if (WS2812_RGB.lamp){uart_write_bytes(UART_NUM_1, beep[11], sizeof(beep[0]));break;}
			
		set_rgb(0X8C0BEE, WS2812_RGB.ligth_rank);
		break;
	case Lamp_Open:
		WS2812_RGB.lamp = 1;
		WS2812_RGB.lamp_speed = 80;
		set_rgb(0,0);	
		break;
	case lamp_close:
		WS2812_RGB.lamp = 0;
		break;
	case lamp_speed_Up:
		if (!WS2812_RGB.lamp)break;
		if(WS2812_RGB.lamp_speed>0&&WS2812_RGB.lamp_speed<100)
			WS2812_RGB.lamp_speed >= 90 ? (WS2812_RGB.lamp_speed += 5) : (WS2812_RGB.lamp_speed += 10);
		else WS2812_RGB.lamp_speed=100;
		break;
	case lamp_speed_down:
		if (!WS2812_RGB.lamp)
			break;
		if(WS2812_RGB.lamp_speed>0&&WS2812_RGB.lamp_speed<101)
			WS2812_RGB.lamp_speed >= 90 ? (WS2812_RGB.lamp_speed -= 5) : (WS2812_RGB.lamp_speed -= 10);
		else WS2812_RGB.lamp_speed=10;
		break;
	case ligth_up:
		if(WS2812_RGB.ligth_rank>0 && WS2812_RGB.ligth_rank<20)
			WS2812_RGB.ligth_rank+=5;
		else {
			if(WS2812_RGB.ligth_rank==20)
			WS2812_RGB.ligth_rank=20;
		}
			
		printf("Ligth =%d\n",WS2812_RGB.ligth_rank);
		break;
	case ligth_down:
		if(WS2812_RGB.ligth_rank>0 && WS2812_RGB.ligth_rank<21)
			WS2812_RGB.ligth_rank-=5;
		
		printf("Ligth =%d\n",WS2812_RGB.ligth_rank);
		break;
	}

}
/*
 *UART RX Task
 *
 *
 */

static void uartRxTask(void *arg)
{
	static const char *RX_TASK_TAG = "RX_TASK";
	esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
	uint8_t *data = (uint8_t *)malloc(RX_BUF_SIZE + 1);

	while (1)
	{
		const int rxBytes = uart_read_bytes(UART_NUM_1, data, RX_BUF_SIZE, 10 / portTICK_RATE_MS);
		if (rxBytes > 0)
		{
			data[rxBytes] = 0;
			ESP_LOGI(RX_TASK_TAG, "Read %d bytes: %s", rxBytes, data);
	
			uartControlLedStrip(uartDataHandle(data));
			ESP_LOG_BUFFER_HEXDUMP(RX_TASK_TAG, data, rxBytes, ESP_LOG_INFO);
		}
		vTaskDelay(10 / portTICK_RATE_MS);
	}
	free(data);
}
/* 
 * @Description: MQTT参数连接的配置
 * @param: 
 * @return: 
 */

int beepPlayTheTone(struct WS2812_COLOR *RGB)
{
	uint32_t RGB_24Bit = 0;
	RGB_24Bit = RGB->red << 8 | RGB->green;
	RGB_24Bit = RGB_24Bit << 8;
	RGB_24Bit = RGB_24Bit | RGB->blue;
	if (WS2812_RGB.lamp == 1)
		return ESP_OK;
	switch (RGB_24Bit)
	{
	case 0XFF00:
		uart_write_bytes(UART_NUM_1, beep[2], sizeof(beep[0]));
		break;
	case 0xFF0000:
		uart_write_bytes(UART_NUM_1, beep[3], sizeof(beep[0]));
		break;
	case 0xFF:
		uart_write_bytes(UART_NUM_1, beep[4], sizeof(beep[0]));
		break;
	case 0XF05308:
		uart_write_bytes(UART_NUM_1, beep[6], sizeof(beep[0]));
		break;
	case 0X8C0BEE:
		uart_write_bytes(UART_NUM_1, beep[5], sizeof(beep[0]));
		break;
	case 0xFFFFFF:
		uart_write_bytes(UART_NUM_1, beep[0], 4); //播放：已经打开灯了
		break;
	case 0x0:
		uart_write_bytes(UART_NUM_1, beep[1], 4); //播放：已经关灯了
		break;
	default:
		break;
	}
	return ESP_OK;
}

void app_main(void)
{
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	init_led();
	UART_Init();
	vTaskDelay(1000 / portTICK_RATE_MS);
	
	//创建跑马灯任务
	while (1)
	{
		led_color_change();
		WS2812_RGB.lamp=1;
		WS2812_RGB.lamp_speed=75;
		stripLamp(0xff,10);

		WS2812_RGB.lamp=0;
	}
	
}
