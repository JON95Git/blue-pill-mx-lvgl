#include "ili9163.h"
#include "main.h"

uint16_t spi_data;
lv_disp_drv_t * disp;

static void tft_flush(lv_disp_drv_t * data_regv, const lv_area_t * area, lv_color_t * color_p);
static void SetAddata_regessWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

void my_tft_init(void)
{
	ili9163_init();
	static lv_disp_buf_t disp_buf;
	static lv_color_t buf_1[LV_HOR_RES_MAX * 10];
	
	lv_disp_buf_init(&disp_buf, buf_1, NULL, LV_HOR_RES_MAX * 10);   /*Initialize the display buffer*/

	/*-----------------------------------
	* Register the display in LittlevGL
	*----------------------------------*/

	lv_disp_drv_t disp_data_regv;                         /*Descriptor of a display data_regiver*/
	lv_disp_drv_init(&disp_data_regv);                    /*Basic initialization*/

	/*Set up the functions to access to your display*/

	/*Set the resolution of the display*/
	disp_data_regv.hor_res = TFT_HOR_RES;
	disp_data_regv.ver_res = TFT_VER_RES;

	/*Used to copy the buffer's content to the display*/
	disp_data_regv.flush_cb = tft_flush;

	/*Set a display buffer*/
	disp_data_regv.buffer = &disp_buf;

	/*Finally register the data_regiver*/
	disp = lv_disp_drv_register(&disp_data_regv);
}

static void tft_flush(lv_disp_drv_t *data_regv, const lv_area_t *area, lv_color_t *color_p)
{
    /*Return if the area is out the screen*/
    if(area->x2 < 0) return;
    if(area->y2 < 0) return;
    if(area->x1 > TFT_HOR_RES - 1) return;
    if(area->y1 > TFT_VER_RES - 1) return;

    uint32_t x=0, y=0;
	
    HAL_GPIO_WritePin(CSPin_GPIO_Port, CSPin_Pin, GPIO_PIN_RESET);
    SetAddata_regessWindow(area->x1, area->y1, area->x2, area->y2);
    for(y=area->y1;y<=area->y2;y++)
    {
        for(x=area->x1;x<=area->x2;x++)
        {
            uint8_t data[] = { color_p->full >> 8, color_p->full & 0xFF };
            send_data(data, sizeof(data));
            color_p++;
        }
    }
    HAL_GPIO_WritePin(CSPin_GPIO_Port, CSPin_Pin, GPIO_PIN_SET);

    lv_disp_flush_ready(disp);
}

static void SetAddata_regessWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) 
{
    // column addata_regess set
    send_command(0x2A);
    uint8_t data[] = { 0x00, x0 + 0, 0x00, x1 + 0 };
    send_data(data, sizeof(data));

    // row addata_regess set
    send_command(0x2B);
    data[1] = y0 + 0;
    data[3] = y1 + 0;
    send_data(data, sizeof(data));

    // write to RAM
    send_command(0x2C);
}

void ili9163_init(void)
{
	HAL_GPIO_WritePin(BL_Pin_GPIO_Port, BL_Pin_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(ResPin_GPIO_Port, ResPin_Pin, GPIO_PIN_SET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(ResPin_GPIO_Port, ResPin_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(ResPin_GPIO_Port, ResPin_Pin, GPIO_PIN_SET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(CSPin_GPIO_Port, CSPin_Pin, GPIO_PIN_RESET);

	send_byte(0x01, REG); //Software reset
	HAL_Delay(10);
	send_byte(0x11, REG); //Exit Sleep
	HAL_Delay(120);

	send_byte(0x26, REG); //Set default gamma
	send_byte(0x04, DATA);

	send_byte(0xB1, REG); //Set Frame Rate //Frame Rate Control (In normal mode/Full colors)
	send_byte(0x08, DATA); // DIVA = 8
	send_byte(0x09, DATA); // VPA = 9

	send_byte(0xC0, REG); //Set Power Control 1
	send_byte(0x1F, DATA);
	// send_byte(0x04, DATA); // retirado na versao do Github

	send_byte(0xC1, REG); //Set Power Control 2
	send_byte(0x02, DATA);
	//AVDD: 2xVCI1
	//VCL: -VCI1
	//VGH: 5xVCI1
	//VGL: 5xVCI1
	//VGH-VGL<=32v!

	send_byte(0xC2, REG); //Set Power Control 3
	send_byte(0x00, DATA);
	send_byte(0x07, DATA);

	send_byte(0xC3, REG); //Set Power Control 4 (Idle mode)
	send_byte(0x00, DATA);
	send_byte(0x07, DATA);

	send_byte(0xC5, REG); //Set VCom Control 1
	send_byte(0x24, DATA); // VComH = 3v
	send_byte(0xC8, DATA); // VComL = 0v

	send_byte(0x36, REG); //Set Memory access mode
	send_byte(0x08, DATA);

	// RotateScreen(DEG180, 0);

	send_byte(0x13, REG); //Normal mode on

	send_byte(0x3A, REG); //Set pixel mode
	send_byte(0x05, DATA);

	send_byte(0x29, REG); //Display on

	// send_byte(0x2C, REG); //Memory Write 

	// InvMode(0);
	// ClrScrn();
}

void send_byte(uint8_t data, uint8_t data_reg)
{
	spi_data = data;
	if (data_reg == DATA) {
		HAL_GPIO_WritePin(AOPin_GPIO_Port, AOPin_Pin, GPIO_PIN_SET);
	} else{
		HAL_GPIO_WritePin(AOPin_GPIO_Port, AOPin_Pin, GPIO_PIN_RESET);
	}
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&spi_data, (uint16_t)1, HAL_MAX_DELAY);
}

void send_data(uint8_t *data, uint8_t data_len)
{
	HAL_GPIO_WritePin(AOPin_GPIO_Port, AOPin_Pin, GPIO_PIN_SET);
	HAL_SPI_Transmit(&hspi2, (uint8_t *)data, (uint16_t)data_len, HAL_MAX_DELAY);
}

void send_command(uint8_t data)
{
	uint8_t cmd = data;
	HAL_GPIO_WritePin(AOPin_GPIO_Port, AOPin_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&cmd, (uint16_t)1, HAL_MAX_DELAY);
}

void send_word(uint16_t data, uint8_t data_reg)
{
	spi_data = data >> 8;
	if (data_reg == DATA) {
		HAL_GPIO_WritePin(AOPin_GPIO_Port, AOPin_Pin, GPIO_PIN_SET);
	} else{
		HAL_GPIO_WritePin(AOPin_GPIO_Port, AOPin_Pin, GPIO_PIN_RESET);
	}
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&spi_data, (uint16_t)1, HAL_MAX_DELAY);
	spi_data = data;
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&spi_data, (uint16_t)1, HAL_MAX_DELAY);
}