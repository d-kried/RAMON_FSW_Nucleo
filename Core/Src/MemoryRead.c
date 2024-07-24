/*
 * MemoryRead.c
 *
 *  Created on: Jul 11, 2024
 *      Author: DeViL
 */
/*
//25AA040A Instructions
const uint8_t EEPROM_READ = 0b00000011;
const uint8_t EEPROM_WRITE = 0b00000010;
const uint8_t EEPROM_WRDI = 0b00000100;
const uint8_t EEPROM_WREN = 0b00000110;
const uint8_t EEPROM_RDSR = 0b00000101;
const uint8_t EEPROM_WRSR = 0b00000001;

char uart_buf[50];
	int uart_buf_len;
	char spi_buf[20];
	uint8_t addr;
	uint8_t wip;

	  // CS pin should default high
	  //HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);


	  HAL_UART_Transmit(&huart2, (int8_t *)uart_buf, uart_buf_len,100);
	  // Enable write enable latch (allow write operation)
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
	  HAL_SPI_Transmit(&hspi1, (uint8_t *)&EEPROM_WREN, 1, 100);
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);


	  // write to file
	  //uint8_t addr_buf[2];

	  //addr_buf[0] = (0x0000 >> 8) & 0xFF; // High byte of the address
	  //addr_buf[1] = 0x0000 & 0xFF;        // Low byte of the address


	  //Say something
	  uart_buf_len = sprintf(uart_buf, "SPI Test\r\n");
	  spi_buf[0] = 0xAB;
	  spi_buf[1] = 0xCD;
	  spi_buf[2] = 0xEF;
	  addr = 0x05;
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
	  HAL_SPI_Transmit(&hspi1, (uint8_t *)&EEPROM_WRITE, 1, 100);
	  HAL_SPI_Transmit(&hspi1, (uint8_t *)&addr, 1, 100);
	  HAL_SPI_Transmit(&hspi1,(uint8_t *)&spi_buf ,3 , 100);
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);

	  // Read status register
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
	  HAL_SPI_Transmit(&hspi1, (uint8_t *)&EEPROM_RDSR, 1, 100);
	  HAL_SPI_Receive(&hspi1, (uint8_t *)spi_buf, 1, 100);
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);

	  // Print status register
	  uart_buf_len = sprintf(uart_buf, "Status: 0x%02x\r\n", (unsigned int)spi_buf[0]);
	  HAL_UART_Transmit(&huart2, (uint8_t *)uart_buf, uart_buf_len,100);
	*/
