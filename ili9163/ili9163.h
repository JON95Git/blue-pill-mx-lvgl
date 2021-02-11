#ifndef __ILI9163_H
#define __ILI9163_H

#include "lv_conf.h"
#include "lvgl.h"
#include <string.h>
#include "spi.h"

#define TFT_HOR_RES 128
#define TFT_VER_RES 160

#define DATA 0
#define REG 1

void my_tft_init(void);
void ili9163_init(void);

void send_byte(uint8_t data, uint8_t data_reg);
void send_word(uint16_t data, uint8_t data_reg);
void send_data(uint8_t *data, uint8_t data_len);
void send_command(uint8_t data);

#endif /* __ILI9163_H */
