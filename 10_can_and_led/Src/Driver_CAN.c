#include "Driver_CAN.h"
#include "Driver_USART.h"

bool is_received = false;

void Driver_CAN_Init()
{
    RCC->APB1ENR |= RCC_APB1ENR_CAN1EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // TD
    GPIOA->CRH |= GPIO_CRH_MODE12;
    GPIOA->CRH &= ~GPIO_CRH_CNF12_0;
    GPIOA->CRH |= GPIO_CRH_CNF12_1;

    // RD
    GPIOA->CRH &= ~GPIO_CRH_MODE11;
    GPIOA->CRH |= GPIO_CRH_CNF11_0;
    GPIOA->CRH &= ~GPIO_CRH_CNF11_1;

    // 初始化模式
    CAN1->MCR &= ~CAN_MCR_SLEEP;

    CAN1->MCR |= CAN_MCR_INRQ;
    while (!(CAN1->MSR & CAN_MSR_INAK))
        ;
    CAN1->BTR |= 0x03;         // 3
    CAN1->BTR |= (0x0E << 16); // 14
    CAN1->BTR |= (0x01 << 20); // 1
    CAN1->BTR |= (0x00 << 24); // 0

    CAN1->MCR |= CAN_MCR_AWUM;
    CAN1->MCR |= CAN_MCR_ABOM;

    // 迴路模式
    CAN1->BTR &= ~CAN_BTR_LBKM;
    CAN1->BTR &= ~CAN_BTR_SILM;

    // 過濾器
    CAN1->FMR |= CAN_FMR_FINIT;
    CAN1->FFA1R &= ~CAN_FFA1R_FFA0;
    CAN1->sFilterRegister[0].FR1 = 0x00;
    CAN1->sFilterRegister[0].FR2 = 0x00;
    CAN1->FA1R |= CAN_FA1R_FACT0;
    CAN1->FMR &= ~CAN_FMR_FINIT;

    // 正常模式
    CAN1->MCR &= ~CAN_MCR_SLEEP;
    CAN1->MCR &= ~CAN_MCR_INRQ;
    while (CAN1->MSR & CAN_MSR_INAK)
        ;
}

uint8_t Driver_CAN_GetPendingMessageCount()
{
    return (uint8_t)(CAN1->RF0R & CAN_RF0R_FMP0);
}

bool Driver_CAN_Transmit(uint16_t id, uint8_t *data, uint8_t len)
{
    if (len > 8)
    {
        len = 8;
    }

    CAN1->sTxMailBox[0].TIR = (id & 0x7FFU) << 21;
    CAN1->sTxMailBox[0].TDTR &= ~CAN_TDT0R_DLC;
    CAN1->sTxMailBox[0].TDTR |= len;

    CAN1->sTxMailBox[0].TDLR = 0;
    CAN1->sTxMailBox[0].TDHR = 0;

    for (uint8_t i = 0; i < len; i++)
    {
        if (i < 4)
        {
            CAN1->sTxMailBox[0].TDLR |= (data[i] << (i * 8));
        }
        else
        {
            CAN1->sTxMailBox[0].TDHR |= (data[i] << ((i - 4) * 8));
        }
    }

    CAN1->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;
    int timeout = 1000000;

    while (!(CAN1->TSR & CAN_TSR_RQCP0))
    {
        if (--timeout == 0)
            return false;
    }

    bool success = (CAN1->TSR & CAN_TSR_TXOK0) != 0;
    CAN1->TSR = CAN_TSR_RQCP0;

    return success;
}

bool Driver_CAN_Receive(uint16_t *id, uint8_t *data, uint8_t *len)
{
    if (Driver_CAN_GetPendingMessageCount() > 0)
    {
        *id = CAN1->sFIFOMailBox[0].RIR >> 21;
        *len = (CAN1->sFIFOMailBox[0].RDTR & CAN_RDT0R_DLC);

        for (uint8_t i = 0; i < *len; i++)
        {
            if (i < 4)
            {
                data[i] = (uint8_t)(CAN1->sFIFOMailBox[0].RDLR >> (i * 8));
            }
            else
            {
                data[i] = (uint8_t)(CAN1->sFIFOMailBox[0].RDHR >> ((i - 4) * 8));
            }
        }

        CAN1->RF0R |= CAN_RF0R_RFOM0;
        data[*len] = '\0';

        return false;
    }
    return true;
}
