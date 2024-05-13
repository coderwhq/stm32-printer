#ifndef __USER_CONFIG_H
#define __USER_CONFIG_H

/* User Global */

/* LED */
#define ACTIVE_HIGH_LEVEL 0

#define LED_RCC_GPIO RCC_APB2Periph_GPIOB
#define LED_PORT GPIOB
#define LED_PIN GPIO_Pin_0

/* MySPI */
#define USE_SPI_PERIPH 1 // Ê¹ÓÃÓ²¼þSPI

#define SPI_X SPI1
#define SPI_RCC RCC_APB2Periph_SPI1
#define SPI_RCC_GPIO RCC_APB2Periph_GPIOA

#define SPI_PINS_PORT GPIOA
#define SPI_SCK_PIN GPIO_Pin_5
#define SPI_MISO_PIN GPIO_Pin_6
#define SPI_MOSI_PIN GPIO_Pin_7

/* PrinterHead */
#define PRINTER_LAT_RCC_GPIO RCC_APB2Periph_GPIOB
#define PRINTER_LAT_PORT GPIOB
#define PRINTER_LAT_PIN GPIO_Pin_1

#define PRINTER_STB_RCC_GPIO RCC_APB2Periph_GPIOB
#define PRINTER_STB_PORT GPIOB
#define PRINTER_STB1_PIN GPIO_Pin_10
#define PRINTER_STB2_PIN GPIO_Pin_11
#define PRINTER_STB3_PIN GPIO_Pin_12
#define PRINTER_STB4_PIN GPIO_Pin_13
#define PRINTER_STB5_PIN GPIO_Pin_14
#define PRINTER_STB6_PIN GPIO_Pin_15
#define PRINTER_STB_PINS                                                                                               \
    PRINTER_STB1_PIN | PRINTER_STB2_PIN | PRINTER_STB3_PIN | PRINTER_STB4_PIN | PRINTER_STB5_PIN | PRINTER_STB6_PIN

#define HEAT_TIME 20
#define LAT_TIME 1
#define DOTLINE_SIZE 48

/* PrinterMoto */
#define PRINTER_MOTO_RCC_GPIO RCC_APB2Periph_GPIOB
#define PRINTER_MOTO_PORT GPIOB

#define PRINTER_A1 GPIO_Pin_9 /* Pin9 -> AMIN1 -> AMOUT1 -> A+ -> 28 */
#define PRINTER_A2 GPIO_Pin_8 /* Pin8 -> AMIN2 -> AMOUT2 -> A- -> 27 */
#define PRINTER_B1 GPIO_Pin_7 /* Pin7 -> BMIN1 -> BMOUT1 -> B+ -> 30 */
#define PRINTER_B2 GPIO_Pin_6 /* Pin6 -> BMIN2 -> BMOUT2 -> B- -> 29 */

#define PRINTER_MOTO_DELAY 4000

/* Serial */
#define SERIAL_USART_X USART2

#define SERIAL_RCC RCC_APB1Periph_USART2
#define SERIAL_RCC_GPIO RCC_APB2Periph_GPIOA

#define SERIAL_PINS_PORT GPIOA
#define SERIAL_TX_PIN GPIO_Pin_2
#define SERIAL_RX_PIN GPIO_Pin_3

#endif
