
#include "uart_comm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ctype.h>
extern UART_HandleTypeDef huart1;
uart_rx_comm_t uart_rx;
uart_rx_comm_t cmd_rx;

void delay(void);
void uart_comm_push_rx_buf(uint8_t rx);
void uart_comm_put_char(UART_HandleTypeDef *huart, uint8_t data);

//-------------------------------------------------------------------------------------------------------------

HAL_StatusTypeDef uart_comm_enable_RX_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
    return HAL_UART_Receive_IT(huart, pData, UART_INFINITE_EN_CNT);
}

void uart_comm_receive_data_IT(uint8_t data8bits)
{
    uart_comm_push_rx_buf(data8bits);
}

// HAL_StatusTypeDef UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t len){
//   if(HAL_UART_Transmit_IT(huart, pData, len) != HAL_OK){
//     if(RingBuffer_Write(&uart1_txBuf, pData, len) != RING_BUFFER_OK){
//       return 0;
//     }
//   }
//   return 1;
// }

void uart_comm_rx_init(uart_rx_comm_t *rx_type)
{
    rx_type->head = 0;
    rx_type->tail = 0;
    rx_type->cnt = 0;
}

void uart_comm_push_rx_buf(uint8_t rx)
{
    uart_rx.head = (++uart_rx.head)%RX_SIZE;

    if(uart_rx.head != uart_rx.tail){
        uart_rx.rx[uart_rx.head] = rx;
    }else{
        while(1); //NG
    }    
}

int32_t uart_comm_pop_rx_buf(uint8_t *rx)
{
    uint8_t data;
    if(uart_rx.head == uart_rx.tail)
        return 0;
    
    uart_rx.tail = (++uart_rx.tail)%RX_SIZE;
    *rx =  uart_rx.rx[uart_rx.tail];
    // data =  uart_rx.rx[uart_rx.tail];
    
    return 1;
}

void delay(void)
{
    volatile uint32_t delay = 100000; 
    while(delay--);
}

char *trim_left(char *str) {
    /* https://www.dotnetnote.com/docs/c-language/topics/string-trim/ */
    while (*str) {
        if (isspace(*str)) {
            str++;
        } else {
            break;
        }
    }
    return str;
}

char *trim_right(char *str) {
    /* https://www.dotnetnote.com/docs/c-language/topics/string-trim/ */
    int len = (int)strlen(str) - 1;

    while (len >= 0) {
        if (isspace(*(str + len))) {
            len--;
        } else {
            break;
        }
    }
    *(str + ++len) = '\0';
    return str;
}

char *trim(char *str) {
    /* https://www.dotnetnote.com/docs/c-language/topics/string-trim/ */
    return trim_left(trim_right(str));
}

void uart_comm_parse_argv(uint32_t *argc,char **argv, char *command)
{    
    char *sub_string = NULL;
    char token[] = " ";
    
    // uart_comm_put_char(&huart1, *validChar);
    sub_string = strtok(command, token);        

    while(++(*argc) <= ARGV_MAX)
    {
        *argv++ = sub_string;
        sub_string = strtok(NULL, token);

        if(sub_string == NULL)
            break;
    }    

// #include <stdio.h>
// #include <string.h>

// int main(void) {    
//     char string[] = "All you need is kill, Live, Die, and repeat.";
//     char token[] = " .,";         // whitespace( ), dot(.), and comma(,).
//     char *pch = NULL;
    
//     pch = strtok(string, token);
//     while(pch != NULL) {
//         printf("Tokenized: %s\n", pch);
//         pch = strtok(NULL, token);
//     }
    
//     // Tokenized: All
//     // Tokenized: you
//     // Tokenized: need
//     // Tokenized: is
//     // Tokenized: kill
//     // Tokenized: Live
//     // Tokenized: Die
//     // Tokenized: and
//     // Tokenized: repeat
    
//     return 0;    
// }
}

void uart_comm_command(void)
{
    char *argv[ARGV_MAX];
    uint32_t argc=0;

    char commandBuf[RX_SIZE];

    static uint16_t cmd_buf_cnt = 0;
    uint8_t data;

    if(uart_comm_pop_rx_buf(&data))
    {
        //huart1.Instance->DR = data;

        if(isprint(data))
        {
            //uart_comm_put_char(&huart1, data);
            cmd_rx.rx[cmd_rx.cnt++] = data;
            cmd_rx.cnt = cmd_rx.cnt%sizeof(cmd_rx.rx);
        }
        else
        {
            switch(data)
            {
                case 0x1B: //esc
                    uart_comm_rx_init(&cmd_rx);
                    break;
                case 0x0D: //enter : Carrige Return
                    memcpy(commandBuf, cmd_rx.rx, cmd_rx.cnt);
                    commandBuf[cmd_rx.cnt] = 0;
                    
                    for(int i=0; i<ARGV_MAX; i++)
                        argv[i]=NULL;

                    uart_comm_parse_argv(&argc,argv,commandBuf);

                    for(int i = 0; i < cmd_rx.cnt; i++){
                        uart_comm_put_char(&huart1, commandBuf[i]);
                    }
                    uart_comm_put_char(&huart1, 0x0A);
                    uart_comm_put_char(&huart1, 0x0D);
                    uart_comm_rx_init(&cmd_rx);
                    break;
                default : 
                    break;
            }
        }
        // else{
        //     uart_comm_put_char(&huart1, 'p');
        // }
        switch(data)
        {
            
        }
    }
}

void uart_comm_put_char(UART_HandleTypeDef *huart, uint8_t data)
{
    huart->Instance->DR = data;

    while(__HAL_UART_GET_FLAG(huart, UART_FLAG_TXE) != SET){
        ;
    }
    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_TXE);
    // delay();
}

#include <stdarg.h>
void variable_argument_list_test(char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    va_arg(args, int);
    va_end(args);
}

/*
MSVC - include - vadefs.h

#ifndef _VA_LIST_DEFINED
    #define _VA_LIST_DEFINED
    #ifdef _M_CEE_PURE
        typedef System::ArgIterator va_list;
    #else
        typedef char* va_list;
    #endif
#endif
*/