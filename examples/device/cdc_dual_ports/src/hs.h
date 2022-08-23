#ifndef __HS_H__
#define __HS_H__

void hs_init(void);
void hs_task(void);

void hs_transmit(uint8_t cdc_num, const void *buf, uint16_t len);


#endif