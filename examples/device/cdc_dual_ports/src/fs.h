#ifndef __FS_H__
#define __FS_H__

void fs_init(void);
void fs_task(void);

void fs_transmit(uint8_t cdc_num, const void *buf, uint16_t len);


#endif