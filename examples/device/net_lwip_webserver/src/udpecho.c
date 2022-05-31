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
#include "udpecho.h"

#include "lwip/opt.h"

#include "lwip/api.h"
#include "lwip/sys.h"

#include <string.h>
#define INIT_IP4(a,b,c,d) { PP_HTONL(LWIP_MAKEU32(a,b,c,d)) }
/*-----------------------------------------------------------------------------------*/
static void
udpecho1_thread(void *arg)
{
  char buffer[32];
  const ip4_addr_t ipaddr  = INIT_IP4(192, 168, 7, 2);
  const int ip_port = 48001;
  struct netconn *conn;
  struct netbuf *buf;
  err_t err;
  LWIP_UNUSED_ARG(arg);

  conn = netconn_new(NETCONN_UDP);
  LWIP_ERROR("udpecho: invalid conn", (conn != NULL), while(1){};);
//  netconn_bind(conn, IP_ADDR_ANY, ip_port);
  strcpy(buffer, "Jest HiHi :) 48001");
  while (1) {
    sys_msleep(1000);
    buf = netbuf_new();

    char *mem = (char *)netbuf_alloc(buf, strlen(buffer));
    if (mem == NULL) {
	    //LOG
	    continue;
    }
    strncpy(mem, buffer, strlen(buffer));
    netconn_sendto(conn, buf, &ipaddr, ip_port);
//    netconn_send(conn, buf);

    netbuf_delete(buf);
#if 0
    err = netconn_recv(conn, &buf);
    if (err == ERR_OK) {
      /*  no need netconn_connect here, since the netbuf contains the address */
      if(netbuf_copy(buf, buffer, sizeof(buffer)) != buf->p->tot_len) {
        LWIP_DEBUGF(LWIP_DBG_ON, ("netbuf_copy failed\n"));
      } else {
        buffer[buf->p->tot_len] = '\0';
        err = netconn_send(conn, buf);
        if(err != ERR_OK) {
          LWIP_DEBUGF(LWIP_DBG_ON, ("netconn_send failed: %d\n", (int)err));
        } else {
          LWIP_DEBUGF(LWIP_DBG_ON, ("got %s\n", buffer));
        }
      }
      netbuf_delete(buf);
    }
#endif
  }
}
static void
udpecho2_thread(void *arg)
{
  char buffer[32];
  const ip4_addr_t ipaddr  = INIT_IP4(192, 168, 7, 2);
  const int ip_port = 48002;
  struct netconn *conn;
  struct netbuf *buf;
  err_t err;
  LWIP_UNUSED_ARG(arg);

  conn = netconn_new(NETCONN_UDP);
  LWIP_ERROR("udpecho: invalid conn", (conn != NULL), while(1){};);
//  netconn_bind(conn, IP_ADDR_ANY, ip_port);
  strcpy(buffer, "Jest HiHi :) 48002");
  while (1) {
    sys_msleep(1000);
    buf = netbuf_new();

    char *mem = (char *)netbuf_alloc(buf, strlen(buffer));
    if (mem == NULL) {
        //LOG
        continue;
    }
    strncpy(mem, buffer, strlen(buffer));
    netconn_sendto(conn, buf, &ipaddr, ip_port);
//    netconn_send(conn, buf);

    netbuf_delete(buf);
#if 0
    err = netconn_recv(conn, &buf);
    if (err == ERR_OK) {
      /*  no need netconn_connect here, since the netbuf contains the address */
      if(netbuf_copy(buf, buffer, sizeof(buffer)) != buf->p->tot_len) {
        LWIP_DEBUGF(LWIP_DBG_ON, ("netbuf_copy failed\n"));
      } else {
        buffer[buf->p->tot_len] = '\0';
        err = netconn_send(conn, buf);
        if(err != ERR_OK) {
          LWIP_DEBUGF(LWIP_DBG_ON, ("netconn_send failed: %d\n", (int)err));
        } else {
          LWIP_DEBUGF(LWIP_DBG_ON, ("got %s\n", buffer));
        }
      }
      netbuf_delete(buf);
    }
#endif
  }
}
/*-----------------------------------------------------------------------------------*/
void
udpecho_init(void)
{
  sys_thread_new("udpecho1_thread", udpecho1_thread, NULL, 8192, 5);
  sys_thread_new("udpecho2_thread", udpecho2_thread, NULL, 8192, 5);
}
