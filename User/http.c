#include "http.h"
#include "./SYSTEM/sys/sys.h"
#include "./BSP/ATK_MW8266D/atk_mw8266d.h"
#include "./BSP/LCD/lcd.h"
#include "./SYSTEM/usart/usart.h"
#include "string.h"
#include "stdio.h"
#include "temp.h"
#include "ESP8266.h"

void http_init(void);

// 温度湿度拼接成json格式
static void temp_humi_json(char *json)
{
    uint8_t temp, humi;
    temp_read(&temp);
    humi_read(&humi);
    sprintf(json, "{\"temperature\":\"%d\", \"humidity\":\"%d\"}", temp, humi);
}

// 拼接请求体
void request_body(char *request)
{
    char json[50];
    temp_humi_json(json);
    sprintf(request, "POST /api/v1/%s/telemetry HTTP/1.1\r\n"
                     "Host: %s:%s\r\n"
                     "Content-Type: application/json\r\n"
                     "Content-Length: %d\r\n"
                     "\r\n"
                     "%s\r\n",
            THINGS_TOKEN, THINGS_IP, THINGS_PORT, strlen(json), json);
}

/**
 * @brief HTTP POST 请求
 * @author Ekko
 */
void http_post(char request[], uint8_t is_unvarnished)
{ // request为整个请求体
    // 打开透传模式
    if (is_unvarnished == 0)
    {
        change_unvarnished(&is_unvarnished);
    }
    delay_ms(200);
    atk_mw8266d_uart_printf(request);

    delay_ms(200);
    lcd_show_string(10, 220, 256, 16, 16, "HTTP POST success!   sending.....", RED);

    // 关闭透传模式
    if (is_unvarnished == 1)
    {
        change_unvarnished(&is_unvarnished);
    }
}

uint16_t http_get(uint8_t is_unvarnished)
{
    char flagout=0;
    // 打开透传模式
    if (is_unvarnished == 0)
    {
        change_unvarnished(&is_unvarnished);
    }

    /*GET /api/v1/psPBidsv4XBNbl3bu91D/attributes?clientKeys=switch&sharedKeys=shared1 HTTP/1.1
    Host: 43.143.141.18:8080 */
    char request[200];
    sprintf(request, "GET /api/v1/%s/attributes?clientKeys=switch&sharedKeys=shared1 HTTP/1.1\r\n"
                     "Host: %s:%s\r\n"
                     "\r\n",
            THINGS_TOKEN, THINGS_IP, THINGS_PORT);
    delay_ms(200);
    atk_mw8266d_uart_printf(request);
    delay_ms(1000);
    upload_data(is_unvarnished, &flagout);
    delay_ms(20);
    printf("flagout1:%c", flagout);

    // 关闭透传模式
    if (is_unvarnished == 1)
    {
        change_unvarnished(&is_unvarnished);
    }

    if (flagout == '1')
    {
        return 1;
    }
    if (flagout == '0')
    {
        return 0;
    }
}
