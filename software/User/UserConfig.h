#ifndef __USER_CONFIG_H
#define __USER_CONFIG_H

/* User Global */

// #define MAX_RX_BYTE 1024 // 最大能打印多少个字节
#define MAX_RX_BYTE 384 // 最大能打印多少个字节
#define FONTSIZE 32
#define ASCII 0x1078D0L // ASCII字体在每种字体中的偏移量

/* LED */
#define ACTIVE_HIGH_LEVEL 0

#define LED_RCC_GPIO RCC_APB2Periph_GPIOB
#define LED_PORT GPIOB
#define LED_PIN GPIO_Pin_0

/* Button */
#define BUTTON_RCC_GPIO RCC_APB2Periph_GPIOB
#define BUTTON_RCC_AFIO RCC_APB2Periph_AFIO
#define BUTTON_PORT GPIOB
#define BUTTON_PIN GPIO_Pin_4
#define BUTTON_EXTI_AFIO_PORT GPIO_PortSourceGPIOB
#define BUTTON_EXTI_AFIO_LINE GPIO_PinSource4
#define BUTTON_EXTI_LINE EXTI_Line4
#define BUTTON_EXTI_CHANNEL EXTI4_IRQn
#define BUTTON_EXTI_FUNCTION EXTI4_IRQHandler

/* MySPI */
#define USE_SPI_PERIPH 1 // 使用硬件SPI

#define SPI_X SPI1
#define SPI_RCC RCC_APB2Periph_SPI1
#define SPI_RCC_GPIO RCC_APB2Periph_GPIOA

#define SPI_PINS_PORT GPIOA
#define SPI_SCK_PIN GPIO_Pin_5
#define SPI_MISO_PIN GPIO_Pin_6
#define SPI_MOSI_PIN GPIO_Pin_7

/* MySPI2 */

#define SPI2_X SPI2
#define SPI2_RCC RCC_APB1Periph_SPI2
#define SPI2_RCC_GPIO RCC_APB2Periph_GPIOB

#define SPI2_PINS_PORT GPIOB
#define SPI2_SS_PIN GPIO_Pin_12
#define SPI2_SCK_PIN GPIO_Pin_13
#define SPI2_MISO_PIN GPIO_Pin_14
#define SPI2_MOSI_PIN GPIO_Pin_15

/* PrinterHead */
#define PRINTER_LAT_RCC_GPIO RCC_APB2Periph_GPIOB
#define PRINTER_LAT_PORT GPIOB
#define PRINTER_LAT_PIN GPIO_Pin_1

#define PRINTER_STB_RCC_GPIO RCC_APB2Periph_GPIOA
#define PRINTER_STB_PORT GPIOA
#define PRINTER_STB1_PIN GPIO_Pin_8
#define PRINTER_STB2_PIN GPIO_Pin_9
#define PRINTER_STB3_PIN GPIO_Pin_10
#define PRINTER_STB4_PIN GPIO_Pin_11
#define PRINTER_STB5_PIN GPIO_Pin_12
#define PRINTER_STB6_PIN GPIO_Pin_15
#define PRINTER_STB_PINS                                                                                               \
    PRINTER_STB1_PIN | PRINTER_STB2_PIN | PRINTER_STB3_PIN | PRINTER_STB4_PIN | PRINTER_STB5_PIN | PRINTER_STB6_PIN

#define USE_CIRCLE_HEAT 1

#define HEAT_TIME 5 // 默认使用的加热时间 ms
#define LAT_TIME 1  // ms

#define DOTLINE_SIZE 48 // Byte
// #define DOTLINE_SIZE_PIXELS DOTLINE_SIZE * 8
#define BlankDotLine 64 * 4

#define WORD_SIZE FONTSIZE
#define LINE_SPACE WORD_SIZE + (uint8_t)(WORD_SIZE / 2) // 1.5行距
#define SEGMENT_SPACE LINE_SPACE * 3                    // 3行距的段距

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

/* Instruction String */

#define CMD_DEFAULT "CMD_DEFAULT" // 默认模式（打印字符）
#define CMD_PIC "CMD_PIC"         // 打印图片

#endif
