#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/LCD/lcd.h"
#include "ESP8266.h"
#include "temp.h"
#include "http.h"
// #include "http.h"
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

    uint8_t is_unvarnished = 0;
    uint8_t t; // 用于计时
    char request[1024];
    uint8_t temperature;
    uint8_t humidity;
    while (1)
    {
        /* 每2000ms执行一次*/
        if (t % 200 == 0)
        {
            request_body(request);
            printf("%s\r\n", request);
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

        delay_ms(10);
        t++;

        if (t == 30)
        {
            t = 0;
            LED0_TOGGLE(); /* LED0闪烁 */
        }
    }
}
