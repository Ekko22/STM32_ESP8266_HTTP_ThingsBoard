#include "http.h"
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "string.h"
#include "stdio.h"
#include "temp.h"

#define THINGS_IP "43.143.141.18"           // ����thingsboard IP��ַ
#define THINGS_PORT "8080"                  // ����thingsboard�˿�
#define THINGS_TOKEN "psPBidsv4XBNbl3bu91D" // ����thingsboard token

void http_init(void);

// �¶�ʪ��ƴ�ӳ�json��ʽ
void temp_humi_json(char *json)
{
    uint8_t temp, humi;
    temp_read(&temp);
    humi_read(&humi);
    sprintf(json, "{\"temp\":\"%d\", \"humi\":\"%d\"}", temp, humi);
}

// ƴ��������
/*
    ʾ��
    char request[] = "POST /api/v1/psPBidsv4XBNbl3bu91D/telemetry HTTP/1.1\r\n"
                     "Host: 43.143.141.18:8080\r\n"
                     "Content-Type: application/json\r\n"
                     "Content-Length: 35\r\n"
                     "\r\n"
                     "{\"zut0\":\"ekko5\", \"cs20\":\"value2\"}\r\n";
*/
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
