DEPS_SUBMODULES += lib/lwip
DEPS_SUBMODULES += lib/FreeRTOS-Kernel

include ../../../tools/top.mk
include ../../make.mk

FREERTOS_SRC = lib/FreeRTOS-Kernel

# suppress warning caused by lwip
CFLAGS += \
  -Wno-error=null-dereference \
  -Wno-error=unused-parameter \
  -Wno-error=unused-variable \
  -Wno-error=maybe-uninitialized \
  -Wno-error=cast-qual

INC += \
  src \
  $(TOP)/hw \
  $(TOP)/lib/lwip/src/include \
  $(TOP)/lib/lwip/src/include/ipv4 \
  $(TOP)/lib/lwip/src/include/lwip/apps \
  src/FreeRTOSConfig \
  $(TOP)/hw \
  $(TOP)/$(FREERTOS_SRC)/include \
  $(TOP)/$(FREERTOS_SRC)/portable/GCC/$(FREERTOS_PORT) \
  $(TOP)/lib/networking


# Example source
EXAMPLE_SOURCE += $(wildcard src/*.c)
SRC_C += $(addprefix $(CURRENT_PATH)/, $(EXAMPLE_SOURCE))

# lwip sources
SRC_C += \
  lib/lwip/src/core/altcp.c \
  lib/lwip/src/core/altcp_alloc.c \
  lib/lwip/src/core/altcp_tcp.c \
  lib/lwip/src/core/def.c \
  lib/lwip/src/core/dns.c \
  lib/lwip/src/core/inet_chksum.c \
  lib/lwip/src/core/init.c \
  lib/lwip/src/core/ip.c \
  lib/lwip/src/core/mem.c \
  lib/lwip/src/core/memp.c \
  lib/lwip/src/core/netif.c \
  lib/lwip/src/core/pbuf.c \
  lib/lwip/src/core/raw.c \
  lib/lwip/src/core/stats.c \
  lib/lwip/src/core/sys.c \
  lib/lwip/src/core/tcp.c \
  lib/lwip/src/api/err.c \
  lib/lwip/src/api/tcpip.c \
  lib/lwip/src/api/api_lib.c \
  lib/lwip/src/api/api_msg.c \
  lib/lwip/src/api/netbuf.c \
  lib/lwip/src/core/tcp_in.c \
  lib/lwip/src/core/tcp_out.c \
  lib/lwip/src/core/timeouts.c \
  lib/lwip/src/core/udp.c \
  lib/lwip/src/core/ipv4/autoip.c \
  lib/lwip/src/core/ipv4/dhcp.c \
  lib/lwip/src/core/ipv4/etharp.c \
  lib/lwip/src/core/ipv4/icmp.c \
  lib/lwip/src/core/ipv4/igmp.c \
  lib/lwip/src/core/ipv4/ip4.c \
  lib/lwip/src/core/ipv4/ip4_addr.c \
  lib/lwip/src/core/ipv4/ip4_frag.c \
  lib/lwip/src/netif/ethernet.c \
  lib/lwip/src/netif/slipif.c \
  lib/networking/dhserver.c \
  lib/networking/dnserver.c \
  lib/networking/rndis_reports.c \
  $(FREERTOS_SRC)/list.c \
  $(FREERTOS_SRC)/queue.c \
  $(FREERTOS_SRC)/tasks.c \
  $(FREERTOS_SRC)/timers.c \
  $(subst ../../../,,$(wildcard ../../../$(FREERTOS_SRC)/portable/GCC/$(FREERTOS_PORT)/*.c))

LDFLAGS += -Wl,--undefined=vTaskSwitchContext

include ../../rules.mk
