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

// 显示局域网内IP地址
static void show_ip(char *buf)
{
    printf("IP: %s\r\n", buf);
    lcd_show_string(32, 180, 128, 16, 16, buf, BLUE);
}

/**
 * @brief       按键1功能，切换透传模式
 * @param       is_unvarnished: 0，未进入透传
 *                              1，已进入透传
 */
void change_unvarnished(uint8_t *is_unvarnished)
{
    uint8_t ret;

    if (*is_unvarnished == 0)
    {
        /* 进入透传 */
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
        /* 退出透传 */
        atk_mw8266d_exit_unvarnished();
        *is_unvarnished = 0;
        // printf("Exit unvarnished!\r\n");
    }
}

/**
 * @brief       进入透传时，将接收自TCP Server的数据发送到串口调试助手
 * @param       is_unvarnished: 0，未进入透传
 *                              1，已进入透传
 *                      *flag : 用于传出接收到的数据
 * @author      Ekko
 */
void upload_data(uint8_t is_unvarnished, char *flag)
{
    uint8_t *buf;

    if (is_unvarnished == 1)
    {
        /* 接收来自ATK-MW8266D UART的一帧数据 */
        buf = atk_mw8266d_uart_rx_get_frame();
        if (buf != NULL)
        {
            // 将远端灯的状态值传出
            flag[0] = buf[strlen(buf) - 3];
            /* 重开开始接收来自ATK-MW8266D UART的数据 */
            atk_mw8266d_uart_rx_restart();
        }
    }
}

/**
 * @brief   esp8266初始化
 * @param   无
 * @author  Ekko
 */
void ESP8266_init(void)
{
    uint8_t ret;
    char ip_buf[16];

    /* 初始化ATK-MW8266D */
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
    ret = atk_mw8266d_restore();      /* 恢复出厂设置 */
    ret += atk_mw8266d_at_test();     /* AT测试 */
    ret += atk_mw8266d_set_mode(1);   /* Station模式 */
    ret += atk_mw8266d_sw_reset();    /* 软件复位 */
    ret += atk_mw8266d_ate_config(0); /* 关闭回显功能 */
    lcd_show_string(10, 140, 256, 16, 16, "Joining to AP......", BLUE);
    ret += atk_mw8266d_join_ap(WIFI_SSID, WIFI_PWD); /* 连接WIFI */
    ret += atk_mw8266d_get_ip(ip_buf);               /* 获取IP地址 */
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

    /* 连接TCP服务器 */
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

    /* 重新开始接收新的一帧数据 */
    atk_mw8266d_uart_rx_restart();
}
