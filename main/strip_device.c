#include <stdio.h>
#include <time.h>
#include "strip_device.h"
#include "esp_log.h"
static const char *TAG = "WS2812S";
static led_strip_t *strip;
struct WS2812_COLOR WS2812_RGB;
struct timespec time1 = { 0, 0 };

////16进制RGB转换为十进制的 R ,G B
void RGB16for10(struct WS2812_COLOR *RGB, uint32_t reb_16)
{
	uint32_t rgb_16 = reb_16;
	RGB->red = rgb_16 & 0Xff;

	rgb_16 = rgb_16 >> 8;
	RGB->green = rgb_16 & 0xff;

	rgb_16 = rgb_16 >> 8;

	RGB->blue = rgb_16 & 0xff;
}
//void set_rgb(uint16_t Red, uint16_t Green, uint16_t Blue)
void set_rgb(uint32_t rgb_24bit, uint8_t ligth_rank)
{
	RGB16for10(&WS2812_RGB, rgb_24bit);
	ligth_rank = 21 - ligth_rank;
	for (int i = 0; i < LED_STRIP_NUM; i++)
	{
		strip->set_pixel(strip, i, WS2812_RGB.red / ligth_rank, WS2812_RGB.green / ligth_rank, WS2812_RGB.blue / ligth_rank);
		//strip->set_pixel(strip, i, WS2812_RGB.red, WS2812_RGB.green, WS2812_RGB.blue);
		
		vTaskDelay(1 / portTICK_RATE_MS);
		
	}
	strip->refresh(strip, 10);	
}
//跑马灯函数
/* 参数：rgb_16data 为十六位颜色值
* 参数：ligth_rank 为亮度等级 0~20,值越大，亮度越高
*/
void stripLamp(uint32_t rgb_16data, uint8_t ligth_rank)
{
	struct WS2812_COLOR lampRGB;
	int i = 0;

	RGB16for10(&lampRGB, rgb_16data);
	
	while (i < LED_STRIP_NUM)
	{
		clock_gettime(CLOCK_REALTIME, &time1);
		srand(time1.tv_nsec);
		RGB16for10(&lampRGB, rand() % 16777214);
		if (WS2812_RGB.lamp)
		{
			strip->set_pixel(strip, i, lampRGB.red / (21 - ligth_rank), lampRGB.green / (21 - ligth_rank), lampRGB.blue / (21 - ligth_rank));
			if (i > 0)
				strip->set_pixel(strip, i - 1, 0, 0, 0);
			strip->refresh(strip, 10);
			vTaskDelay(10 * (101 - WS2812_RGB.lamp_speed) / portTICK_RATE_MS);
			i++;
		}
		else
			vTaskDelay(10 / portTICK_RATE_MS);
	}
	strip->set_pixel(strip, LED_STRIP_NUM-1, 0, 0, 0);
	strip->refresh(strip, 10);
}
//Ws2812 init function
void init_led()
{
	rmt_config_t config = RMT_DEFAULT_CONFIG_TX(RMT_TX_NUM, RMT_TX_CHANNEL);
	// set counter clock to 40MHz
	config.clk_div = 2;

	ESP_ERROR_CHECK(rmt_config(&config));
	ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));

	// install ws2812 driver
	led_strip_config_t strip_config = LED_STRIP_DEFAULT_CONFIG(LED_STRIP_NUM, (led_strip_dev_t)config.channel);
	strip = led_strip_new_rmt_ws2812(&strip_config);
	if (!strip)
	{
		ESP_LOGE(TAG, "install WS2812 driver failed");
	}
	// Clear LED strip (turn off all LEDs)
	ESP_ERROR_CHECK(strip->clear(strip, 100));
	vTaskDelay(100/ portTICK_RATE_MS);
	set_rgb(0xffFFFF, 10);
}

//颜色渐变效果
void led_color_change()
{
    int rank=0;
    strip->clear(strip,100);
    for (rank  = 0; rank < 255; rank++)   {
        //set_rgb(255,rank);
        for (int i = 0; i < LED_STRIP_NUM; i++)
	    {
            strip->set_pixel(strip, i, rank,0, 0);
            //strip->set_pixel(strip, i, WS2812_RGB.red, WS2812_RGB.green, WS2812_RGB.blue);
            vTaskDelay(1 / portTICK_RATE_MS);
	    }
	    strip->refresh(strip, 10);
        vTaskDelay(10/ portTICK_RATE_MS);
    }
    for (; rank>0; rank--)
    {
          //set_rgb(255,rank);
        for (int i = 0; i < LED_STRIP_NUM; i++)   {
            strip->set_pixel(strip, i, rank,0, 0);
                    
            vTaskDelay(1 / portTICK_RATE_MS);
	    }
	    strip->refresh(strip, 10);
        vTaskDelay(10/ portTICK_RATE_MS);
    }
    
     for (rank  = 0; rank < 255; rank++)   {
        //set_rgb(255,rank);
        for (int i = 0; i < LED_STRIP_NUM; i++)
	    {
            strip->set_pixel(strip, i, 0,rank, 0);
            //strip->set_pixel(strip, i, WS2812_RGB.red, WS2812_RGB.green, WS2812_RGB.blue);
            vTaskDelay(1 / portTICK_RATE_MS);
	    }
	    strip->refresh(strip, 10);
        vTaskDelay(10/ portTICK_RATE_MS);
    }
    for (; rank>0; rank--)
    {
          //set_rgb(255,rank);
        for (int i = 0; i < LED_STRIP_NUM; i++)   {
            strip->set_pixel(strip, i, 0,rank, 0);
                    
            vTaskDelay(1 / portTICK_RATE_MS);
	    }
	    strip->refresh(strip, 10);
        vTaskDelay(10/ portTICK_RATE_MS);
    }
    for (rank  = 0; rank < 255; rank++)   {
        //set_rgb(255,rank);
        for (int i = 0; i < LED_STRIP_NUM; i++)
	    {
            strip->set_pixel(strip, i, 0,0, rank);
            //strip->set_pixel(strip, i, WS2812_RGB.red, WS2812_RGB.green, WS2812_RGB.blue);
            vTaskDelay(1 / portTICK_RATE_MS);
	    }
	    strip->refresh(strip, 10);
        vTaskDelay(10/ portTICK_RATE_MS);
    }
    for (; rank>0; rank--)
    {
          //set_rgb(255,rank);
        for (int i = 0; i < LED_STRIP_NUM; i++)   {
            strip->set_pixel(strip, i, 0,0,rank);
                    
            vTaskDelay(1 / portTICK_RATE_MS);
	    }
	    strip->refresh(strip, 10);
        vTaskDelay(10/ portTICK_RATE_MS);
    }

     for (rank  = 0; rank < 255; rank++)   {
        //set_rgb(255,rank);
        for (int i = 0; i < LED_STRIP_NUM; i++)
	    {
            strip->set_pixel(strip, i, rank,0, rank);
            //strip->set_pixel(strip, i, WS2812_RGB.red, WS2812_RGB.green, WS2812_RGB.blue);
            vTaskDelay(1 / portTICK_RATE_MS);
	    }
	    strip->refresh(strip, 10);
        vTaskDelay(10/ portTICK_RATE_MS);
    }
    for (; rank>0; rank--)
    {
          //set_rgb(255,rank);
        for (int i = 0; i < LED_STRIP_NUM; i++)   {
            strip->set_pixel(strip, i, rank,0,rank);
                    
            vTaskDelay(1 / portTICK_RATE_MS);
	    }
	    strip->refresh(strip, 10);
        vTaskDelay(10/ portTICK_RATE_MS);
    }

     for (rank  = 0; rank < 255; rank++)   {
        //set_rgb(255,rank);
        for (int i = 0; i < LED_STRIP_NUM; i++)
	    {
            strip->set_pixel(strip, i, rank,rank, 0);
            //strip->set_pixel(strip, i, WS2812_RGB.red, WS2812_RGB.green, WS2812_RGB.blue);
            vTaskDelay(1 / portTICK_RATE_MS);
	    }
	    strip->refresh(strip, 10);
        vTaskDelay(10/ portTICK_RATE_MS);
    }
    for (; rank>0; rank--)
    {
          //set_rgb(255,rank);
        for (int i = 0; i < LED_STRIP_NUM; i++)   {
            strip->set_pixel(strip, i, rank,rank,0);
                    
            vTaskDelay(1 / portTICK_RATE_MS);
	    }
	    strip->refresh(strip, 10);
        vTaskDelay(10/ portTICK_RATE_MS);
    }
}
