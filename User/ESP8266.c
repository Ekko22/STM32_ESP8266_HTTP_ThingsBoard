#include "ESP8266.h"
#include "./BSP/ATK_MW8266D/atk_mw8266d.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/LCD/lcd.h"
#include "string.h"
#include "temp.h"
#include "http.h"

// ��ʾ��������IP��ַ
static void show_ip(char *buf)
{
    printf("IP: %s\r\n", buf);
    lcd_show_string(32, 180, 128, 16, 16, buf, BLUE);
}

/**
 * @brief       ����1���ܣ��л�͸��ģʽ
 * @param       is_unvarnished: 0��δ����͸��
 *                              1���ѽ���͸��
 */
void change_unvarnished(uint8_t *is_unvarnished)
{
    uint8_t ret;

    if (*is_unvarnished == 0)
    {
        /* ����͸�� */
        ret = atk_mw8266d_enter_unvarnished();
        if (ret == 0)
        {
            *is_unvarnished = 1;
            // printf("Enter unvarnished!\r\n");
        }
        else
        {
            // printf("Enter unvarnished failed!\r\n");
        }
    }
    else
    {
        /* �˳�͸�� */
        atk_mw8266d_exit_unvarnished();
        *is_unvarnished = 0;
        // printf("Exit unvarnished!\r\n");
    }
}

/**
 * @brief       ����͸��ʱ����������TCP Server�����ݷ��͵����ڵ�������
 * @param       is_unvarnished: 0��δ����͸��
 *                              1���ѽ���͸��
 *                      *flag : ���ڴ������յ�������
 * @author      Ekko
 */
void upload_data(uint8_t is_unvarnished, char *flag)
{
    uint8_t *buf;

    if (is_unvarnished == 1)
    {
        /* ��������ATK-MW8266D UART��һ֡���� */
        buf = atk_mw8266d_uart_rx_get_frame();
        if (buf != NULL)
        {
            // ��Զ�˵Ƶ�״ֵ̬����
            flag[0] = buf[strlen(buf) - 3];
            /* �ؿ���ʼ��������ATK-MW8266D UART������ */
            atk_mw8266d_uart_rx_restart();
        }
    }
}

/**
 * @brief   esp8266��ʼ��
 * @param   ��
 * @author  Ekko
 */
void ESP8266_init(void)
{
    uint8_t ret;
    char ip_buf[16];

    /* ��ʼ��ATK-MW8266D */
    ret = atk_mw8266d_init(115200);
    if (ret != 0)
    {
        printf("ATK-MW8266D init failed!\r\n");
        while (1)
        {
            LED0_TOGGLE();
            delay_ms(200);
        }
    }
    lcd_show_string(10, 120, 256, 16, 16, "ESP8266 init success!", BLUE);
    printf("Joining to AP...\r\n");
    ret = atk_mw8266d_restore();      /* �ָ��������� */
    ret += atk_mw8266d_at_test();     /* AT���� */
    ret += atk_mw8266d_set_mode(1);   /* Stationģʽ */
    ret += atk_mw8266d_sw_reset();    /* �����λ */
    ret += atk_mw8266d_ate_config(0); /* �رջ��Թ��� */
    lcd_show_string(10, 140, 256, 16, 16, "Joining to AP......", BLUE);
    ret += atk_mw8266d_join_ap(WIFI_SSID, WIFI_PWD); /* ����WIFI */
    ret += atk_mw8266d_get_ip(ip_buf);               /* ��ȡIP��ַ */
    if (ret != 0)
    {
        printf("Error to join ap!\r\n");
        lcd_show_string(10, 160, 256, 16, 16, "Joining to AP failed!", BLUE);
        while (1)
        {
            LED0_TOGGLE();
            delay_ms(200);
        }
    }
    lcd_show_string(10, 160, 256, 16, 16, "Joining to AP success!", BLUE);
    lcd_show_string(10, 180, 256, 16, 16, "IP: ", BLUE);
    show_ip(ip_buf);

    /* ����TCP������ */
    ret = atk_mw8266d_connect_tcp_server(THINGS_IP, THINGS_PORT);
    if (ret != 0)
    {
        printf("Error to connect tcp server!\r\n");
        lcd_show_string(10, 200, 256, 16, 16, "Connect to thingsboard failed!", BLUE);
        while (1)
        {
            LED0_TOGGLE();
            delay_ms(200);
        }
    }
    lcd_show_string(10, 200, 256, 16, 16, "Connect to thingsboard success!", BLUE);

    /* ���¿�ʼ�����µ�һ֡���� */
    atk_mw8266d_uart_rx_restart();
}
