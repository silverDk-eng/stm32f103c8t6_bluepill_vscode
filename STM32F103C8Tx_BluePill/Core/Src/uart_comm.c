
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

void uart_comm_parse_argv(uint32_t *argc,char **argv, char *command)
{
    char *p;

    // strspan()
    p = strtok(command, " ");
}

void uart_comm_command(void)
{
    char *argv[ARGV_MAX];

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
                    for(int i = 0; i < cmd_rx.cnt; i++){
                        uart_comm_put_char(&huart1, cmd_rx.rx[i]);
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
