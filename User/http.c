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

// �¶�ʪ��ƴ�ӳ�json��ʽ
static void temp_humi_json(char *json)
{
    uint8_t temp, humi;
    temp_read(&temp);
    humi_read(&humi);
    sprintf(json, "{\"temperature\":\"%d\", \"humidity\":\"%d\"}", temp, humi);
}

// ƴ��������
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
 * @brief HTTP POST ����
 * @param request
 * @param is_unvarnished
 * @note 0��δ����͸��
 *       1���ѽ���͸��
 * @note ͸��ģʽ�£��������ݵ�TCP Server
 * @author Ekko
 */
void http_post(char request[], uint8_t is_unvarnished)
{ // requestΪ����������
    // ��͸��ģʽ
    if (is_unvarnished == 0)
    {
        change_unvarnished(&is_unvarnished);
    }
    delay_ms(100);
    atk_mw8266d_uart_printf(request);
    delay_ms(200);
    lcd_show_string(10, 220, 256, 16, 16, "HTTP POST success!   sending.....", RED);
    delay_ms(1000);
    lcd_show_string(10, 220, 256, 16, 16, "HTTP POST success!   sending.....", WHITE);

    // �ر�͸��ģʽ
    if (is_unvarnished == 1)
    {
        change_unvarnished(&is_unvarnished);
    }
}
/**
 * @brief HTTP GET �����״̬
 * @param is_unvarnished
 * @return uint16_t
 * @note 0���ƹ�
 *       1���ƿ�
 * @author Ekko
 */
uint16_t http_get_led(uint8_t is_unvarnished)
{
    char flagout = 0;
    // ��͸��ģʽ
    if (is_unvarnished == 0)
    {
        change_unvarnished(&is_unvarnished);
    }
    // ƴ��������
    char request[200];
    sprintf(request, "GET /api/v1/%s/attributes?clientKeys=switch&sharedKeys=shared1 HTTP/1.1\r\n"
                     "Host: %s:%s\r\n"
                     "\r\n",
            THINGS_TOKEN, THINGS_IP, THINGS_PORT);
    // ��������
    atk_mw8266d_uart_printf(request);
    // �ӳ�500ms���ڽ�������
    delay_ms(500);
    upload_data(is_unvarnished, &flagout);
    // �ر�͸��ģʽ
    if (is_unvarnished == 1)
    {
        change_unvarnished(&is_unvarnished);
    }
    // ���ؿ���״̬
    if (flagout == '1')
    {
        return 1;
    }
    if (flagout == '0')
    {
        return 0;
    }
}
