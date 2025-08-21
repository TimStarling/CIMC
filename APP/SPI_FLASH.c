/************************************************************
 * 版权：2025CIMC Copyright。 
 * 文件：SPI_Flash.c
 * 作者: Jianchuan Wang & Tao Liu @ GigaDevice
 * 平台: 2025CIMC IHD-V04
 * 版本: Jianchuan Wang     2025/4/20     V0.01    original
************************************************************/
#include "SPI_FLASH.h"

#define WRITE            0x02     /* write to memory instruction */
#define WRSR             0x01     /* write status register instruction */
#define WREN             0x06     /* write enable instruction */

#define READ             0x03     /* read from memory instruction */
#define RDSR             0x05     /* read status register instruction  */
#define RDID             0x9F     /* read identification */
#define SE               0x20     /* sector erase instruction */
#define BE               0xC7     /* bulk erase instruction */

#define WIP_FLAG         0x01     /* write in progress(wip)flag */
#define DUMMY_BYTE       0xA5

/*!
    \brief      initialize SPI1 GPIO and parameter
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi_flash_init(void)
{
    spi_parameter_struct spi_init_struct;

    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_SPI1);

	
	 /* SPI1_CLK(PB13), SPI1_MISO(PB14), SPI1_MOSI(PB15) */
    gpio_af_set(GPIOB, GPIO_AF_5, GPIO_PIN_13|GPIO_PIN_14| GPIO_PIN_15);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_13|GPIO_PIN_14| GPIO_PIN_15);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_13|GPIO_PIN_14| GPIO_PIN_15);

    /* SPI1_CS(PB12) GPIO pin configuration */
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_12);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);


    /* chip select invalid*/
    SPI_FLASH_CS_HIGH();

    /* SPI1 parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_8 ;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;;
    spi_init(SPI1, &spi_init_struct);

    /* enable SPI1 */
    spi_enable(SPI1);
}

/*!
    \brief      erase the specified flash sector
    \param[in]  sector_addr: address of the sector to erase
    \param[out] none
    \retval     none
*/
void spi_flash_sector_erase(uint32_t sector_addr)
{
    /* send write enable instruction */
    spi_flash_write_enable();

    /* sector erase */
    /* select the flash: chip select low */
    SPI_FLASH_CS_LOW();
    /* send sector erase instruction */
    spi_flash_send_byte(SE);
    /* send sector_addr high nibble address byte */
    spi_flash_send_byte((sector_addr & 0xFF0000) >> 16);
    /* send sector_addr medium nibble address byte */
    spi_flash_send_byte((sector_addr & 0xFF00) >> 8);
    /* send sector_addr low nibble address byte */
    spi_flash_send_byte(sector_addr & 0xFF);
    /* deselect the flash: chip select high */
    SPI_FLASH_CS_HIGH();

    /* wait the end of flash writing */
    spi_flash_wait_for_write_end();
}

/*!
    \brief      erase the specified flash bytes
    \param[in]  sector_addr: address of the sector to erase
    \param[out] num_byte_to_erase: Length to be erased
    \retval     none
*/
void spi_flash_buffer_erase(uint32_t sector_addr,  uint32_t num_byte_to_erase)
{
	uint8_t buffer_data[SPI_FLASH_SECTOR_SIZE] = {0};
	uint8_t buffer_data1[SPI_FLASH_SECTOR_SIZE] = {0};
	uint8_t num_of_sector = 0, num_of_single = 0, addr = 0, count = 0, temp = 0;

    addr          = sector_addr % SPI_FLASH_SECTOR_SIZE;		//扇区内地址
    count         = SPI_FLASH_PAGE_SIZE - addr;					//页内剩下可以写的空间长度
    num_of_sector = num_byte_to_erase / SPI_FLASH_SECTOR_SIZE;	//要擦除多少个满扇区空间
    num_of_single = num_byte_to_erase % SPI_FLASH_SECTOR_SIZE;	//剩下要擦除不满一个扇区的字节数
	
	/* 待擦除的地址是扇区对齐的 */
    if(0 == addr)
	{

		while(num_of_sector-- )		//擦除整数个扇区
		{
			spi_flash_sector_erase( sector_addr );
			sector_addr += SPI_FLASH_PAGE_SIZE;
		}
		if(0 != num_of_single)		//擦除小数个扇区
		{
			spi_flash_buffer_read(buffer_data, sector_addr+num_of_single, SPI_FLASH_SECTOR_SIZE-num_of_single);			//先读出扇区后部分内容
			spi_flash_sector_erase( sector_addr );		//再擦
			spi_flash_buffer_write(buffer_data, sector_addr+num_of_single, SPI_FLASH_SECTOR_SIZE-num_of_single);		//再写回扇区后部分内容	
		}
    }
	else/* 待擦除的地址是非扇区对齐的 */
	{
        if(num_byte_to_erase < count)
		{
			spi_flash_buffer_read(buffer_data, num_of_sector*SPI_FLASH_SECTOR_SIZE, addr);																	//先读出扇区前部分内容
			spi_flash_buffer_read(buffer_data1, num_of_sector*SPI_FLASH_SECTOR_SIZE+addr+num_byte_to_erase, SPI_FLASH_SECTOR_SIZE-(addr)-num_byte_to_erase);//  读出扇区后部分内容
			spi_flash_sector_erase( sector_addr );		//再擦
			spi_flash_buffer_write(buffer_data, num_of_sector*SPI_FLASH_SECTOR_SIZE, addr);																	//再写回扇区前部分内容
			spi_flash_buffer_write(buffer_data1, num_of_sector*SPI_FLASH_SECTOR_SIZE+addr+num_byte_to_erase, SPI_FLASH_SECTOR_SIZE-(addr)-num_byte_to_erase);//再写回扇区前部分内容
		}
		else
		{
			spi_flash_buffer_read(buffer_data, num_of_sector*SPI_FLASH_SECTOR_SIZE, addr);	//先读出扇区前部分内容
			spi_flash_sector_erase( sector_addr );		//再擦
			spi_flash_buffer_write(buffer_data, num_of_sector*SPI_FLASH_SECTOR_SIZE, addr);		//再写回扇区前部分内容
			
			//现在擦除地址又变得扇区对齐了
			num_byte_to_erase -= addr;
			num_of_sector = num_byte_to_erase / SPI_FLASH_SECTOR_SIZE;//要擦除多少个满扇区空间
			num_of_single = num_byte_to_erase % SPI_FLASH_SECTOR_SIZE;//剩下要擦除不满一个扇区的字节数
			sector_addr += count;
			
			while(num_of_sector-- )		//擦除整数个扇区
			{
				spi_flash_sector_erase( sector_addr );
				sector_addr += SPI_FLASH_PAGE_SIZE;
			}
			if(0 != num_of_single)		//擦除小数个扇区
			{
				spi_flash_buffer_read(buffer_data, sector_addr+num_of_single, SPI_FLASH_SECTOR_SIZE-num_of_single);			//先读出来
				spi_flash_sector_erase( sector_addr );		//再擦
				spi_flash_buffer_write(buffer_data, sector_addr+num_of_single, SPI_FLASH_SECTOR_SIZE-num_of_single);	//再写回去	
			}	
		}
    }
}



/*!
    \brief      erase the entire flash
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi_flash_bulk_erase(void)
{
    /* send write enable instruction */
    spi_flash_write_enable();

    /* bulk erase */
    /* select the flash: chip select low */
    SPI_FLASH_CS_LOW();
    /* send bulk erase instruction  */
    spi_flash_send_byte(BE);
    /* deselect the flash: chip select high */
    SPI_FLASH_CS_HIGH();

    /* wait the end of flash writing */
    spi_flash_wait_for_write_end();
}

/*!
    \brief      write more than one byte to the flash
    \param[in]  pbuffer: pointer to the buffer
    \param[in]  write_addr: flash's internal address to write
    \param[in]  num_byte_to_write: number of bytes to write to the flash
    \param[out] none
    \retval     none
*/
void spi_flash_page_write(uint8_t* pbuffer, uint32_t write_addr, uint16_t num_byte_to_write)
{
    /* enable the write access to the flash */
    spi_flash_write_enable();

    /* select the flash: chip select low */
    SPI_FLASH_CS_LOW();

    /* send "write to memory" instruction */
    spi_flash_send_byte(WRITE);
    /* send write_addr high nibble address byte to write to */
    spi_flash_send_byte((write_addr & 0xFF0000) >> 16);
    /* send write_addr medium nibble address byte to write to */
    spi_flash_send_byte((write_addr & 0xFF00) >> 8);
    /* send write_addr low nibble address byte to write to */
    spi_flash_send_byte(write_addr & 0xFF);

    /* while there is data to be written on the flash */
    while(num_byte_to_write--){
        /* send the current byte */
        spi_flash_send_byte(*pbuffer);
        /* point on the next byte to be written */
        pbuffer++;
    }

    /* deselect the flash: chip select high */
    SPI_FLASH_CS_HIGH();

    /* wait the end of flash writing */
    spi_flash_wait_for_write_end();
}

/*!
    \brief      write block of data to the flash
    \param[in]  pbuffer: pointer to the buffer
    \param[in]  write_addr: flash's internal address to write
    \param[in]  num_byte_to_write: number of bytes to write to the flash
    \param[out] none
    \retval     none
*/
void spi_flash_buffer_write(uint8_t* pbuffer, uint32_t write_addr, uint32_t num_byte_to_write)
{
	
    uint8_t num_of_page = 0, num_of_single = 0, addr = 0, count = 0, temp = 0;

    addr          = write_addr % SPI_FLASH_PAGE_SIZE;		//页内地址
    count         = SPI_FLASH_PAGE_SIZE - addr;				//页内剩下可以写的空间长度
    num_of_page   = num_byte_to_write / SPI_FLASH_PAGE_SIZE;//要写满的页数
    num_of_single = num_byte_to_write % SPI_FLASH_PAGE_SIZE;//剩下的不满一页的字节数

     /* 待写的地址是页对齐的  */
    if(0 == addr)
	{

		while(num_of_page-- )
		{
			spi_flash_page_write(pbuffer,write_addr, SPI_FLASH_PAGE_SIZE);
			write_addr += SPI_FLASH_PAGE_SIZE;
			pbuffer += SPI_FLASH_PAGE_SIZE;
		}
		if(0 != num_of_single)
			spi_flash_page_write(pbuffer,write_addr, num_of_single);
    }
	else
	{
        if(num_byte_to_write < count)
		{
			spi_flash_page_write(pbuffer,write_addr, num_byte_to_write);
		}
		else
		{
			//写完当前页剩余的空间
			spi_flash_page_write(pbuffer,write_addr, count);
			
			//现在地址又变得页对齐了
			num_of_page   = (num_byte_to_write - count) / SPI_FLASH_PAGE_SIZE;//要写满的页数
			num_of_single = (num_byte_to_write - count) % SPI_FLASH_PAGE_SIZE;//剩下的不满一页的字节数
			write_addr += count;
			pbuffer += count;
			while(num_of_page-- )
			{
				spi_flash_page_write(pbuffer,write_addr, SPI_FLASH_PAGE_SIZE);
				write_addr += SPI_FLASH_PAGE_SIZE;
				pbuffer += SPI_FLASH_PAGE_SIZE;
			}
			
			if(0 != num_of_single)
				spi_flash_page_write(pbuffer,write_addr, num_of_single);	
		}
    }
}

/*!
    \brief      read a block of data from the flash
    \param[in]  pbuffer: pointer to the buffer that receives the data read from the flash
    \param[in]  read_addr: flash's internal address to read from
    \param[in]  num_byte_to_read: number of bytes to read from the flash
    \param[out] none
    \retval     none
*/
void spi_flash_buffer_read(uint8_t* pbuffer, uint32_t read_addr, uint16_t num_byte_to_read)
{
    /* select the flash: chip slect low */
    SPI_FLASH_CS_LOW();

    /* send "read from memory " instruction */
    spi_flash_send_byte(READ);

    /* send read_addr high nibble address byte to read from */
    spi_flash_send_byte((read_addr & 0xFF0000) >> 16);
    /* send read_addr medium nibble address byte to read from */
    spi_flash_send_byte((read_addr& 0xFF00) >> 8);
    /* send read_addr low nibble address byte to read from */
    spi_flash_send_byte(read_addr & 0xFF);

    /* while there is data to be read */
    while(num_byte_to_read--){
        /* read a byte from the flash */
        *pbuffer = spi_flash_send_byte(DUMMY_BYTE);
        /* point to the next location where the byte read will be saved */
        pbuffer++;
    }

    /* deselect the flash: chip select high */
    SPI_FLASH_CS_HIGH();
}

/*!
    \brief      read flash identification
    \param[in]  none
    \param[out] none
    \retval     flash identification
*/
uint32_t spi_flash_read_id(void)
{
    uint32_t temp = 0, temp0 = 0, temp1 = 0, temp2 = 0;

    /* select the flash: chip select low */
    SPI_FLASH_CS_LOW();

    /* send "RDID " instruction */
    spi_flash_send_byte(0x9F);

    /* read a byte from the flash */
    temp0 = spi_flash_send_byte(DUMMY_BYTE);

    /* read a byte from the flash */
    temp1 = spi_flash_send_byte(DUMMY_BYTE);

    /* read a byte from the flash */
    temp2 = spi_flash_send_byte(DUMMY_BYTE);

    /* deselect the flash: chip select high */
    SPI_FLASH_CS_HIGH();

    temp = (temp0 << 16) | (temp1 << 8) | temp2;

    return temp;
}

/*!
    \brief      initiate a read data byte (read) sequence from the flash
    \param[in]  read_addr: flash's internal address to read from
    \param[out] none
    \retval     none
*/
void spi_flash_start_read_sequence(uint32_t read_addr)
{
    /* select the flash: chip select low */
    SPI_FLASH_CS_LOW();

    /* send "read from memory " instruction */
    spi_flash_send_byte(READ);

    /* send the 24-bit address of the address to read from */
    /* send read_addr high nibble address byte */
    spi_flash_send_byte((read_addr & 0xFF0000) >> 16);
    /* send read_addr medium nibble address byte */
    spi_flash_send_byte((read_addr& 0xFF00) >> 8);
    /* send read_addr low nibble address byte */
    spi_flash_send_byte(read_addr & 0xFF);
}

/*!
    \brief      read a byte from the SPI flash
    \param[in]  none
    \param[out] none
    \retval     byte read from the SPI flash
*/
uint8_t spi_flash_read_byte(void)
{
    return(spi_flash_send_byte(DUMMY_BYTE));
}

/*!
    \brief      send a byte through the SPI interface and return the byte received from the SPI bus
    \param[in]  byte: byte to send
    \param[out] none
    \retval     the value of the received byte
*/
uint8_t spi_flash_send_byte(uint8_t byte)
{
    /* loop while data register in not emplty */
    while (RESET == spi_i2s_flag_get(SPI1,SPI_FLAG_TBE));

    /* send byte through the SPI1 peripheral */
    spi_i2s_data_transmit(SPI1,byte);

    /* wait to receive a byte */
    while(RESET == spi_i2s_flag_get(SPI1,SPI_FLAG_RBNE));

    /* return the byte read from the SPI bus */
    return(spi_i2s_data_receive(SPI1));
}

/*!
    \brief      send a half word through the SPI interface and return the half word received from the SPI bus
    \param[in]  half_word: half word to send
    \param[out] none
    \retval     the value of the received byte
*/
uint16_t spi_flash_send_halfword(uint16_t half_word)
{
    /* loop while data register in not emplty */
    while(RESET == spi_i2s_flag_get(SPI1,SPI_FLAG_TBE));

    /* send half word through the SPI1 peripheral */
    spi_i2s_data_transmit(SPI1,half_word);

    /* wait to receive a half word */
    while(RESET == spi_i2s_flag_get(SPI1,SPI_FLAG_RBNE));

    /* return the half word read from the SPI bus */
    return spi_i2s_data_receive(SPI1);
}

/*!
    \brief      enable the write access to the flash
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi_flash_write_enable(void)
{
    /* select the flash: chip select low */
    SPI_FLASH_CS_LOW();

    /* send "write enable" instruction */
    spi_flash_send_byte(WREN);

    /* deselect the flash: chip select high */
    SPI_FLASH_CS_HIGH();
}

/*!
    \brief      poll the status of the write in progress(wip) flag in the flash's status register
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi_flash_wait_for_write_end(void)
{
    uint8_t flash_status = 0;

    /* select the flash: chip select low */
    SPI_FLASH_CS_LOW();

    /* send "read status register" instruction */
    spi_flash_send_byte(RDSR);

    /* loop as long as the memory is busy with a write cycle */
    do{
        /* send a dummy byte to generate the clock needed by the flash
        and put the value of the status register in flash_status variable */
        flash_status = spi_flash_send_byte(DUMMY_BYTE);
    }while((flash_status & WIP_FLAG) == SET);

    /* deselect the flash: chip select high */
    SPI_FLASH_CS_HIGH();
}
