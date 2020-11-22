/*
 * Nombre del archivo:   main.c
 * Autor:   Santiago Sempé
 *
 * Descripción: 
 *        Proyecto usando un display matricial con MAX7219.
 *        Sistema de cordenadas q mantiene encendidos los LEDs q se le piden
 *        para dibujar en la tabla.
 */

#include <xc.h>
#include <stdio.h>
#include <stdint.h>

#include "max7219.h"

/* ------------------------ Definiciones ------------------------------------ */
#define _XTAL_FREQ  4000000L    // Frecuencia de operación del cristal

#define LED_ON      1
#define LED_OFF     0

#define MAX_TABLE   8           //Cambiar este número para determinar el tamaño de la tabla

/* ------------------------ Definición de salidas --------------------------- */
#define PIN_LED1    PORTEbits.RE2
#define TRIS_LED1   TRISEbits.TRISE2

#define PIN_LED2    PORTEbits.RE1
#define TRIS_LED2   TRISEbits.TRISE1

#define PIN_LED3    PORTEbits.RE0
#define TRIS_LED3   TRISEbits.TRISE0

/* ------------------------ Definición de entradas -------------------------- */
#define PIN_TEC1    PORTBbits.RB0
#define TRIS_TEC1   TRISBbits.TRISB0

#define PIN_TEC2    PORTBbits.RB1
#define TRIS_TEC2   TRISBbits.TRISB1

/* ------------------------ Bits de configuración --------------------------- */
// CONFIG1
#pragma config FOSC = XT        // Oscillator Selection bits
//#pragma config FOSC = INTRC_NOCLKOUT // Oscillator Selection bits
#pragma config WDTE = OFF       // Watchdog Timer Enable bit
#pragma config PWRTE = OFF      // Power-up Timer Enable bit
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit
#pragma config CP = OFF         // Code Protection bit
#pragma config CPD = OFF        // Data Code Protection bit
#pragma config BOREN = OFF      // Brown Out Reset Selection bits
#pragma config IESO = OFF       // Internal External Switchover bit
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit
#pragma config LVP = OFF        // Low Voltage Programming Enable bit

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits





/* ------------------------ Prototipos de funciones ------------------------- */
void gpio_config();
void show_num(int num);
void uart_config();
uint8_t uart_rx_byte(uint8_t *dato);
void uart_tx_byte(uint8_t dato);

/* ------------------------ Implementación de funciones --------------------- */
void main(void) { // Función principal
    int i;

    uint8_t dato_recibido, resultado; // Variable donde se almacenan datos
    int num;

    char coordenada[2];

    int fila, columna;







    gpio_config(); // Inicializo las entradas y salidas
    uart_config();
    PORTD = 0;
    PIN_LED1 = 0; // Apago el LED1

    max7219_config();
    max7219_shutdown(MAX_DISPLAY_0, 0);
    max7219_set_scan_limit(MAX_DISPLAY_0, 7);
    max7219_set_intensity(MAX_DISPLAY_0, 15);
    max7219_clear_display(MAX_DISPLAY_0);

    printf("Inserte Coordenadas\r\n");
    printf("Las coordenadas van del 1 al 8 y de la A a la H\r\n");
    printf("Utilice 'P' para imprimir la coordenada y 'C' para limpiar la pantalla\r\n");
    while (1) { // Super loop
        // Ver este link: https://pbs.twimg.com/media/BafQje7CcAAN5en.jpg
        resultado = uart_rx_byte(&dato_recibido);
        if (resultado == 1) {
            if (dato_recibido >= 'A' && dato_recibido <= MAX_TABLE + 'A' - 1) {
                coordenada[0] = dato_recibido;
                columna = coordenada[0] - 'A';
                printf("%c", dato_recibido);
            }
            if (dato_recibido >= '1' && dato_recibido <= MAX_TABLE + '1' - 1) {
                coordenada[1] = dato_recibido;
                fila = coordenada[1] - '1';
                printf("%c", dato_recibido);
            }
            if (dato_recibido == 'P') {
                max7219_set_led(MAX_DISPLAY_0, columna, fila, LED_ON);
                printf("\n\r");
            }
            if (dato_recibido == 'Z') {
                max7219_clear_display(MAX_DISPLAY_0);

            }
        }


    }

    // NO DEBE LLEGAR NUNCA AQUÍ, debido a que este programa se ejecuta
    // directamente sobre un microcontrolador y no es llamado por un ningún
    // sistema operativo, como en el caso de un programa para PC.

    return;
}

void interrupt isr() { // Rutina de atención de interrupciones
    // Esta rutina debe ejecutarse rápidamente sin delays ni loops infinitos.
    // Recuerda colocar en 0 el flag que provocó la interrupción.
}

void gpio_config() {
    ANSEL = 0; // Configuro todas las entradas
    ANSELH = 0; // como digitales

    TRIS_TEC1 = 1; // Configuro la TEC1 como entrada
    TRIS_LED1 = 0; // Configuro el LED1 como salida

    TRISD = 0;
}

void show_num(int num) {
    max7219_clear_display(MAX_DISPLAY_0);
    switch (num) {
        case 1:
            max7219_set_row(MAX_DISPLAY_0, 3, 0b00011000);
            max7219_set_row(MAX_DISPLAY_0, 4, 0b00011000);
            break;
        case 2:
            max7219_set_row(MAX_DISPLAY_0, 1, 0b01100000);
            max7219_set_row(MAX_DISPLAY_0, 2, 0b01100000);
            max7219_set_row(MAX_DISPLAY_0, 5, 0b00000110);
            max7219_set_row(MAX_DISPLAY_0, 6, 0b00000110);
            break;
        case 3:
            max7219_set_row(MAX_DISPLAY_0, 1, 0b01100000);
            max7219_set_row(MAX_DISPLAY_0, 2, 0b01100000);
            max7219_set_row(MAX_DISPLAY_0, 3, 0b00011000);
            max7219_set_row(MAX_DISPLAY_0, 4, 0b00011000);
            max7219_set_row(MAX_DISPLAY_0, 5, 0b00000110);
            max7219_set_row(MAX_DISPLAY_0, 6, 0b00000110);
            break;
        case 4:
            max7219_set_row(MAX_DISPLAY_0, 1, 0b01100110);
            max7219_set_row(MAX_DISPLAY_0, 2, 0b01100110);
            max7219_set_row(MAX_DISPLAY_0, 5, 0b01100110);
            max7219_set_row(MAX_DISPLAY_0, 6, 0b01100110);
            break;
        case 5:
            max7219_set_row(MAX_DISPLAY_0, 1, 0b01100110);
            max7219_set_row(MAX_DISPLAY_0, 2, 0b01100110);
            max7219_set_row(MAX_DISPLAY_0, 5, 0b01100110);
            max7219_set_row(MAX_DISPLAY_0, 6, 0b01100110);
            max7219_set_row(MAX_DISPLAY_0, 3, 0b00011000);
            max7219_set_row(MAX_DISPLAY_0, 4, 0b00011000);

            break;

        case 6:
            max7219_set_row(MAX_DISPLAY_0, 0, 0b01100110);
            max7219_set_row(MAX_DISPLAY_0, 1, 0b01100110);
            max7219_set_row(MAX_DISPLAY_0, 3, 0b01100110);
            max7219_set_row(MAX_DISPLAY_0, 4, 0b01100110);
            max7219_set_row(MAX_DISPLAY_0, 6, 0b01100110);
            max7219_set_row(MAX_DISPLAY_0, 7, 0b01100110);
            break;

    }
}

void uart_config() {
    TXSTAbits.TX9 = 0; //transmision de 8 bits
    TXSTAbits.TXEN = 1; // Transmision habilitada
    TXSTAbits.SYNC = 0; //modo asincronico

    TXSTAbits.BRGH = 0;
    BAUDCTLbits.BRG16 = 1;
    SPBRG = 25; //baudrate de 9600

    RCSTAbits.SPEN = 1; //puerto serie habilitado
    RCSTAbits.RX9 = 0; //recepcion 8 bits
    RCSTAbits.CREN = 1; //recepcion habilitada
    // TODO: Completa configuración de la UART
}

/**
 * @brief	Envía un byte a la salida stdout en forma bloqueante pero por poco tiempo
 * @param	data    Byte o dato a transmitir
 * @return	Nada
 * @note    Define la salida estandar para la librería stdio
 */
void putch(char data) {
    while (PIR1bits.TXIF == 0) //Espera que haya espacio en la FIFO
        continue;
    TXREG = data; //Envía el byte
}

/**
 * @brief	Toma un byte de la entrada stdin en forma bloqueante
 * @return	El byte recibido
 * @note    Define la entrada estandar para la librería stdio
 */
char getch(void) {
    while (PIR1bits.RCIF == 0) //Espera hasta que haya un byte recibido
        continue;
    return RCREG; //retorna lo recibido
}

/**
 * @brief	Envía un byte a la salida stdout en forma bloqueante pero por poco tiempo
 * @param	data    Byte o dato a transmitir
 * @return	Nada
 * @note    Define la salida estandar para la librería stdio
 */
void uart_tx_byte(uint8_t dato) {
    while (PIR1bits.TXIF == 0) //Espera que haya espacio en la FIFO
        continue;
    TXREG = dato; //Envía el byte
}

/**
 * @brief       Toma un byte de la FIFO de recepción en forma no bloqueante,
 *              chequea primero si hay un dato disponible
 * @param[out]  dato    Apunta al destino para el byte recibido
 * @return      1 si hay un byte recibido, 0 si no hay dato disponible 
 */
uint8_t uart_rx_byte(uint8_t *dato) {
    if (PIR1bits.RCIF == 1) {
        *dato = RCREG;
        return 1;
    } else {
        return 0;
    }
}


/* ------------------------ Fin de archivo ---------------------------------- */