#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/LCD/lcd.h"
#include "ESP8266.h"
#include "temp.h"
#include "http.h"
// 信息显示
void show_mesg(void)
{
    /* LCD显示实验信息 */
    lcd_show_string(10, 10, 320, 32, 32, "THINGSBOARS_MONITOR", RED);
    lcd_show_string(10, 48, 256, 24, 24, "DESIGN_BY_CS20", RED);
    lcd_show_string(10, 80, 256, 16, 16, "KEY0: NULL", BLUE);
    lcd_show_string(10, 100, 256, 16, 16, "KEY1: NULL", BLUE);
}

int main(void)
{
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(RCC_PLL_MUL9); /* 设置时钟, 72Mhz */
    delay_init(72);                     /* 延时初始化 */
    usart_init(115200);                 /* 串口初始化为115200 */
    led_init();                         /* 初始化LED */
    key_init();                         /* 初始化按键 */
    lcd_init();                         /* 初始化LCD */
    show_mesg();                        /* 显示实验信息 */
    temp_init();                        /* 初始化温湿度传感器 */
    ESP8266_init();                     /* 初始化ESP8266 */

    uint8_t is_unvarnished = 0; // 标识是否进入透传模式
    uint8_t t;                  // 用于计时
    char request[1024];         // 用于存放http请求
    uint8_t temperature;        // 温度
    uint8_t humidity;           // 湿度
    uint16_t is_on = 0;         // LED灯状态
    while (1)
    {
        /* 每200ms请求一次post*/
        if (t % 200 == 0)
        {
            /* 读取温湿度值,拼接请求体 */
            request_body(request);
            /* http post 温湿度*/
            http_post(request, is_unvarnished);
            /* 读取温湿度值 */
            dht11_read_data(&temperature, &humidity);
            /* 显示温度 */
            lcd_show_string(10, 240, 256, 24, 24, "Temp:    C", BLUE);
            lcd_show_num(65, 240, temperature, 3, 24, BLUE);
            /* 显示湿度 */
            lcd_show_string(10, 270, 256, 24, 24, "Humi:    %", BLUE);
            lcd_show_num(65, 270, humidity, 3, 24, BLUE);
        }

        /* 每500ms get一次 */
        if (t % 490 == 0)
        {
            is_on = http_get_led(is_unvarnished);
        }

        delay_ms(10);
        t++;

        // 如果状态为1，LED灯亮，否则灭
        if (is_on)
        {
            LED0(0);
        }
        if (is_on == 0)
        {
            LED0(1);
        }
    }
}
