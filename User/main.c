#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/LCD/lcd.h"
#include "ESP8266.h"
#include "temp.h"
#include "http.h"
// ��Ϣ��ʾ
void show_mesg(void)
{
    /* LCD��ʾʵ����Ϣ */
    lcd_show_string(10, 10, 320, 32, 32, "THINGSBOARS_MONITOR", RED);
    lcd_show_string(10, 48, 256, 24, 24, "DESIGN_BY_CS20", RED);
    lcd_show_string(10, 80, 256, 16, 16, "KEY0: NULL", BLUE);
    lcd_show_string(10, 100, 256, 16, 16, "KEY1: NULL", BLUE);
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
    temp_init();                        /* ��ʼ����ʪ�ȴ����� */
    ESP8266_init();                     /* ��ʼ��ESP8266 */

    uint8_t is_unvarnished = 0; // ��ʶ�Ƿ����͸��ģʽ
    uint8_t t;                  // ���ڼ�ʱ
    char request[1024];         // ���ڴ��http����
    uint8_t temperature;        // �¶�
    uint8_t humidity;           // ʪ��
    uint16_t is_on = 0;         // LED��״̬
    while (1)
    {
        /* ÿ200ms����һ��post*/
        if (t % 200 == 0)
        {
            /* ��ȡ��ʪ��ֵ,ƴ�������� */
            request_body(request);
            /* http post ��ʪ��*/
            http_post(request, is_unvarnished);
            /* ��ȡ��ʪ��ֵ */
            dht11_read_data(&temperature, &humidity);
            /* ��ʾ�¶� */
            lcd_show_string(10, 240, 256, 24, 24, "Temp:    C", BLUE);
            lcd_show_num(65, 240, temperature, 3, 24, BLUE);
            /* ��ʾʪ�� */
            lcd_show_string(10, 270, 256, 24, 24, "Humi:    %", BLUE);
            lcd_show_num(65, 270, humidity, 3, 24, BLUE);
        }

        /* ÿ500ms getһ�� */
        if (t % 490 == 0)
        {
            is_on = http_get_led(is_unvarnished);
        }

        delay_ms(10);
        t++;

        // ���״̬Ϊ1��LED������������
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
