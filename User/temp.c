#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LCD/lcd.h"
#include "temp.h"

// �¶�ģ���ʼ��
void temp_init(void)
{
    if (dht11_init()) /* DHT11��ʼ�� */
    {
        lcd_show_string(10, 100, 256, 16, 16, "DHT11 Error", RED);
    }
    else
    {
        lcd_show_string(10, 100, 256, 16, 16, "DHT11 init success!", BLUE);
    }
}
=
    // ��ȡ�¶�
    void temp_read(uint8_t * temp)
{
    uint8_t temperature;
    uint8_t humidity;
    dht11_read_data(&temperature, &humidity); /* ��ȡ��ʪ��ֵ */
    *temp = temperature;
}

// ��ȡʪ��
void humi_read(uint8_t *humi)
{
    uint8_t temperature;
    uint8_t humidity;
    dht11_read_data(&temperature, &humidity); /* ��ȡ��ʪ��ֵ */
    *humi = humidity;
}
