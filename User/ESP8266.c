#include "ESP8266.h"
#include "./BSP/ATK_MW8266D/atk_mw8266d.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/LCD/lcd.h"
#include "temp.h"
#include "http.h"

#define WIFI_SSID "Ekko_01"                 // ����WiFi����
#define WIFI_PWD "py011121"                 // ����WiFi����
#define THINGS_IP "43.143.141.18"           // ����thingsboard IP��ַ
#define THINGS_PORT "8080"                  // ����thingsboard�˿�
#define THINGS_TOKEN "psPBidsv4XBNbl3bu91D" // ����thingsboard token

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
 * @retval      ��
 */
static void change_unvarnished(uint8_t *is_unvarnished)
{
    uint8_t ret;

    if (*is_unvarnished == 0)
    {
        /* ����͸�� */
        ret = atk_mw8266d_enter_unvarnished();
        if (ret == 0)
        {
            *is_unvarnished = 1;
            printf("Enter unvarnished!\r\n");
        }
        else
        {
            printf("Enter unvarnished failed!\r\n");
        }
    }
    else
    {
        /* �˳�͸�� */
        atk_mw8266d_exit_unvarnished();
        *is_unvarnished = 0;
        printf("Exit unvarnished!\r\n");
    }
}

/**
 * @brief       ����͸��ʱ����������TCP Server�����ݷ��͵����ڵ�������
 * @param       is_unvarnished: 0��δ����͸��
 *                              1���ѽ���͸��
 * @retval      ��
 */
static void upload_data(uint8_t is_unvarnished)
{
    uint8_t *buf;

    if (is_unvarnished == 1)
    {
        /* ��������ATK-MW8266D UART��һ֡���� */
        buf = atk_mw8266d_uart_rx_get_frame();
        if (buf != NULL)
        {
            printf("%s", buf);
            /* �ؿ���ʼ��������ATK-MW8266D UART������ */
            atk_mw8266d_uart_rx_restart();
        }
    }
}

/**
 * @brief HTTP POST ����
 * @author Ekko
 */
void http_post(char request[], uint8_t is_unvarnished)
{ // requestΪ����������
    /*
    ʾ��
    char request[] = "POST /api/v1/psPBidsv4XBNbl3bu91D/telemetry HTTP/1.1\r\n"
                     "Host: 43.143.141.18:8080\r\n"
                     "Content-Type: application/json\r\n"
                     "Content-Length: 35\r\n"
                     "\r\n"
                     "{\"zut0\":\"ekko5\", \"cs20\":\"value2\"}\r\n";
    */

    // ��͸��ģʽ
    if (is_unvarnished == 0)
    {
        change_unvarnished(&is_unvarnished);
    }

    atk_mw8266d_uart_printf(request);

    delay_ms(2000);
    lcd_show_string(10, 220, 256, 16, 16, "HTTP POST success!   sending.....", RED);

    // �ر�͸��ģʽ
    if (is_unvarnished == 1)
    {
        change_unvarnished(&is_unvarnished);
    }
}

/**
 * @brief       Esp8266��ʼ��
 * @param       ��
 * @retval      ��
 */
void ESP8266_init(void)
{
    uint8_t ret;
    char ip_buf[16];
    uint8_t key;
    uint8_t is_unvarnished = 0;

    char request[1024];

    // char request[1024];

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
    lcd_show_string(10, 140, 256, 16, 16, "ESP8266 init success!", BLUE);
    printf("Joining to AP...\r\n");
    ret = atk_mw8266d_restore();      /* �ָ��������� */
    ret += atk_mw8266d_at_test();     /* AT���� */
    ret += atk_mw8266d_set_mode(1);   /* Stationģʽ */
    ret += atk_mw8266d_sw_reset();    /* �����λ */
    ret += atk_mw8266d_ate_config(0); /* �رջ��Թ��� */
    lcd_show_string(10, 160, 256, 16, 16, "Joining to AP......", BLUE);
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
    lcd_show_string(10, 140, 256, 16, 16, "Joining to AP success!", BLUE);
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
