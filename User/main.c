#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/LCD/lcd.h"
#include "ESP8266.h"

// ��Ϣ��ʾ
void show_mesg(void)
{
    /* LCD��ʾʵ����Ϣ */
    lcd_show_string(10, 10, 320, 32, 32, "THINGSBOARS_MONITOR", RED);
    lcd_show_string(10, 48, 256, 24, 24, "DESIGN_BY_CS20", RED);
    lcd_show_string(10, 80, 256, 16, 16, "KEY0: HTTP POST test", BLUE);
    lcd_show_string(10, 100, 256, 16, 16, "KEY1: switch unvarnished", BLUE);
}

int main(void)
{
    HAL_Init();                         /* ��ʼ��HAL�� */
    sys_stm32_clock_init(RCC_PLL_MUL9); /* ����ʱ��, 72Mhz */
    delay_init(72);                     /* ��ʱ��ʼ�� */
    usart_init(115200);                 /* ���ڳ�ʼ��Ϊ115200 */
    led_init();                         /* ��ʼ��LED */
    key_init();                         /* ��ʼ������ */
    lcd_init();                         /* ��ʼ��LCD */
    show_mesg();                        /* ��ʾʵ����Ϣ */
    ESP8266_init();                     /* ��ʼ��ESP8266 */
}
