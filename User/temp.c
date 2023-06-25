#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LCD/lcd.h"
#include "temp.h"

// 温度模块初始化
void temp_init(void)
{
    if (dht11_init()) /* DHT11初始化 */
    {
        lcd_show_string(10, 100, 256, 16, 16, "DHT11 Error", RED);
    }
    else
    {
        lcd_show_string(10, 100, 256, 16, 16, "DHT11 init success!", BLUE);
    }
}
=
    // 读取温度
    void temp_read(uint8_t * temp)
{
    uint8_t temperature;
    uint8_t humidity;
    dht11_read_data(&temperature, &humidity); /* 读取温湿度值 */
    *temp = temperature;
}

// 读取湿度
void humi_read(uint8_t *humi)
{
    uint8_t temperature;
    uint8_t humidity;
    dht11_read_data(&temperature, &humidity); /* 读取温湿度值 */
    *humi = humidity;
}
