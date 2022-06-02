/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#include "tusb.h"
#include "udpuart.h"

#include "lwip/opt.h"

#include "lwip/api.h"
#include "lwip/sys.h"

#include "stm32f7xx_hal.h"

#include <string.h>

#define INIT_IP4(a,b,c,d) { PP_HTONL(LWIP_MAKEU32(a,b,c,d)) }

void uart_init(UART_HandleTypeDef *uart, uint32_t rs485, uint32_t bitrate, uint32_t stopbits, uint32_t parity, uint32_t length) {
    __HAL_RCC_UART7_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_UART7;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(UART7_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(UART7_IRQn);

    uart->Instance = UART7;
    uart->Init.BaudRate = bitrate;
    uart->Init.WordLength = length;
    uart->Init.StopBits = stopbits;
    uart->Init.Parity = parity;
    uart->Init.Mode = UART_MODE_TX_RX;
    uart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uart->Init.OverSampling = UART_OVERSAMPLING_16;
    uart->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    uart->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (rs485) {
        HAL_RS485Ex_Init(uart, UART_DE_POLARITY_HIGH, 0, 0);
    } else {
        HAL_UART_Init(uart);
    }

    HAL_UART_ReceiverTimeout_Config(uart, 100);
    HAL_UART_EnableReceiverTimeout(uart);
}

/* UDP receive task */
void udpuart_4wire_thread_udp(void *arg) {
    const int ip_port = 48001;
    UART_HandleTypeDef *uart = (UART_HandleTypeDef*) arg;
    uint8_t buffer[32];
    struct netbuf *buf;
    err_t err;

    struct netconn *conn = netconn_new(NETCONN_UDP);
    netconn_bind(conn, IP_ADDR_ANY, ip_port);
    while (1) {
        err = netconn_recv(conn, &buf);
        if (err == ERR_OK) {
            memset(buffer, 0, sizeof(buffer));
            if (netbuf_copy(buf, buffer, sizeof(buffer)) == buf->p->tot_len) {
                HAL_UART_Transmit(uart, buffer, buf->p->tot_len, 0xffff);
                TU_LOG1("Data sent to UART: [%d] [%s]\n", buf->p->tot_len, buffer);
            }
            netbuf_delete(buf);
        }
    }
}

/* UART receive task */
void udpuart_4wire_thread_uart(void *arg) {
    const ip4_addr_t ipaddr = INIT_IP4(192, 168, 7, 2);
    const int ip_port = 48001;
    UART_HandleTypeDef *uart = (UART_HandleTypeDef*) arg;
    uint8_t buffer[32];

    struct netconn *conn = netconn_new(NETCONN_UDP);
    while (1) {
        vTaskDelay(10);
        if (HAL_UART_Receive(uart, buffer, sizeof(buffer), HAL_MAX_DELAY) == HAL_OK) {
            int len = sizeof(buffer) - uart->RxXferCount;
            TU_LOG1("Data received from UART: [%d] [%s]\n", len, buffer);
            struct netbuf *buf = netbuf_new();
            char *mem = (char *)netbuf_alloc(buf, sizeof(buffer));
            if (mem != NULL) {
                memcpy(mem, buffer, sizeof(buffer));
                netconn_sendto(conn, buf, &ipaddr, ip_port);
            }
            netbuf_delete(buf);
        }
    }
}

void udpuart_4wire_init(void) {
    UART_HandleTypeDef uart;
    uart.Instance = UART7;
    uart_init(&uart, 9600, 0, UART_STOPBITS_1, UART_PARITY_NONE, 8);

    sys_thread_new("udpuart_4wire_thread_udp", udpuart_4wire_thread_udp, NULL, 1000, 5);
    sys_thread_new("udpuart_4wire_thread_uart", udpuart_4wire_thread_uart, NULL, 1000, 7);
}

/* UDP echo task */
static void udpecho_thread(void *arg) {
    static struct netconn *conn;
    static struct netbuf *buf;
    static ip_addr_t *addr;
    static unsigned short port;
    int remote_port = 48002;
    err_t err;
    char buffer[32];

    conn = netconn_new(NETCONN_UDP);
    LWIP_ASSERT("con != NULL", conn != NULL);
    netconn_bind(conn, NULL, remote_port);

    while (1) {
        err = netconn_recv(conn, &buf);
        if (err == ERR_OK) {
            addr = netbuf_fromaddr(buf);
            port = netbuf_fromport(buf);
            netconn_connect(conn, addr, port);
            netbuf_copy(buf, buffer, buf->p->tot_len);
            buffer[buf->p->tot_len] = '\0';
            LWIP_DEBUGF(LWIP_DBG_ON, ("got %s at port: %d\n", buffer, remote_port));
            netconn_send(conn, buf);
            netbuf_delete(buf);
        }
    }
}

static void udpecho_init(void) {
    sys_thread_new("udpecho_thread", udpecho_thread, NULL, 1000, 5);
}

void udpdemo_init(void) {
    udpuart_4wire_init();
    udpecho_init();
}

