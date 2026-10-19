#include "Driver_Lora.h"
#include "Driver_USART.h"

static sx1262_handle_t gs_handle; /**< sx1262 handle */

static void Driver_Lora_PrintStatus(void)
{
    uint8_t status;
    uint16_t errors;

    if (sx1262_get_status(&gs_handle, &status) == 0)
    {
        printf("sx1262: status=0x%02X mode=%u command=%u\r\n",
               (unsigned int)status, (unsigned int)((status >> 4) & 0x07),
               (unsigned int)((status >> 1) & 0x07));
    }
    if (sx1262_get_device_errors(&gs_handle, &errors) == 0)
    {
        printf("sx1262: device errors=0x%04X\r\n", (unsigned int)errors);
    }
}

uint8_t Driver_Lora_Init(void)
{
    uint8_t res;
    uint32_t reg;
    uint8_t modulation;
    uint8_t config;
    uint16_t sync_word;

    Driver_SPI_Init();

    // Busy - PA3
    GPIOA->CRL &= ~GPIO_CRL_MODE3;
    GPIOA->CRL &= ~GPIO_CRL_CNF3_1;
    GPIOA->CRL |= GPIO_CRL_CNF3_0;

    // RXEN - PA2
    GPIOA->CRL |= GPIO_CRL_MODE2;
    GPIOA->CRL &= ~GPIO_CRL_CNF2;

    // TXEN - PA1
    GPIOA->CRL |= GPIO_CRL_MODE1;
    GPIOA->CRL &= ~GPIO_CRL_CNF1;

    // RST - PA0
    GPIOA->CRL |= GPIO_CRL_MODE0;
    GPIOA->CRL &= ~GPIO_CRL_CNF0;

    TX_LOW;
    RX_LOW;

    /* link interface function */
    DRIVER_SX1262_LINK_INIT(&gs_handle, sx1262_handle_t);
    DRIVER_SX1262_LINK_SPI_INIT(&gs_handle, sx1262_interface_spi_init);
    DRIVER_SX1262_LINK_SPI_DEINIT(&gs_handle, sx1262_interface_spi_deinit);
    DRIVER_SX1262_LINK_SPI_WRITE_READ(&gs_handle, sx1262_interface_spi_write_read);
    DRIVER_SX1262_LINK_RESET_GPIO_INIT(&gs_handle, sx1262_interface_reset_gpio_init);
    DRIVER_SX1262_LINK_RESET_GPIO_DEINIT(&gs_handle, sx1262_interface_reset_gpio_deinit);
    DRIVER_SX1262_LINK_RESET_GPIO_WRITE(&gs_handle, sx1262_interface_reset_gpio_write);
    DRIVER_SX1262_LINK_BUSY_GPIO_INIT(&gs_handle, sx1262_interface_busy_gpio_init);
    DRIVER_SX1262_LINK_BUSY_GPIO_DEINIT(&gs_handle, sx1262_interface_busy_gpio_deinit);
    DRIVER_SX1262_LINK_BUSY_GPIO_READ(&gs_handle, sx1262_interface_busy_gpio_read);
    DRIVER_SX1262_LINK_DELAY_MS(&gs_handle, sx1262_interface_delay_ms);
    DRIVER_SX1262_LINK_DEBUG_PRINT(&gs_handle, sx1262_interface_debug_print);
    DRIVER_SX1262_LINK_RECEIVE_CALLBACK(&gs_handle, sx1262_interface_receive_callback);

    /* init the sx1262 */
    res = sx1262_init(&gs_handle);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: init failed.\n");

        return 1;
    }

    /* Stay in STDBY_RC until DIO3 supplies the E22-900M22S TCXO. */
    res = sx1262_set_standby(&gs_handle, SX1262_CLOCK_SOURCE_RC_13M);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set standby failed.\n");
        (void)sx1262_deinit(&gs_handle);

        return 1;
    }

    res = sx1262_timeout_convert_to_register(&gs_handle, SX1262_LORA_DEFAULT_TCXO_DELAY_US, &reg);
    if (res == 0)
    {
        res = sx1262_set_dio3_as_tcxo_ctrl(&gs_handle, SX1262_LORA_DEFAULT_TCXO_VOLTAGE, reg);
    }
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set TCXO failed.\n");
        (void)sx1262_deinit(&gs_handle);
        return 1;
    }

    /* Repeat all calibrations with the external reference now powered. */
    res = sx1262_set_calibration(&gs_handle, 0x7F);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: calibration failed.\n");
        (void)sx1262_deinit(&gs_handle);
        return 1;
    }

    res = sx1262_set_standby(&gs_handle, SX1262_CLOCK_SOURCE_XTAL_32MHZ);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: start XOSC failed.\n");
        Driver_Lora_PrintStatus();
        (void)sx1262_deinit(&gs_handle);
        return 1;
    }

    /* set stop timer on preamble */
    res = sx1262_set_stop_timer_on_preamble(&gs_handle, SX1262_LORA_DEFAULT_STOP_TIMER_ON_PREAMBLE);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: stop timer on preamble failed.\n");
        (void)sx1262_deinit(&gs_handle);

        return 1;
    }

    /* set regulator mode */
    res = sx1262_set_regulator_mode(&gs_handle, SX1262_LORA_DEFAULT_REGULATOR_MODE);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set regulator mode failed.\n");
        (void)sx1262_deinit(&gs_handle);

        return 1;
    }

    /* set pa config */
    res = sx1262_set_pa_config(&gs_handle, SX1262_LORA_DEFAULT_PA_CONFIG_DUTY_CYCLE, SX1262_LORA_DEFAULT_PA_CONFIG_HP_MAX);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set pa config failed.\n");
        (void)sx1262_deinit(&gs_handle);

        return 1;
    }

    /* enter to stdby xosc mode */
    res = sx1262_set_rx_tx_fallback_mode(&gs_handle, SX1262_RX_TX_FALLBACK_MODE_STDBY_XOSC);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set rx tx fallback mode failed.\n");
        (void)sx1262_deinit(&gs_handle);

        return 1;
    }

    /* set dio irq */
    res = sx1262_set_dio_irq_params(&gs_handle, 0x03FF, 0x03FF, 0x0000, 0x0000);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set dio irq params failed.\n");
        (void)sx1262_deinit(&gs_handle);

        return 1;
    }

    /* clear irq status */
    res = sx1262_clear_irq_status(&gs_handle, 0x03FF);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: clear irq status failed.\n");
        (void)sx1262_deinit(&gs_handle);

        return 1;
    }

    /* set lora mode */
    res = sx1262_set_packet_type(&gs_handle, SX1262_PACKET_TYPE_LORA);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set packet type failed.\n");
        (void)sx1262_deinit(&gs_handle);

        return 1;
    }

    /* set tx params */
    res = sx1262_set_tx_params(&gs_handle, SX1262_LORA_DEFAULT_TX_DBM, SX1262_LORA_DEFAULT_RAMP_TIME);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set tx params failed.\n");
        (void)sx1262_deinit(&gs_handle);

        return 1;
    }

    /* set lora modulation params */
    res = sx1262_set_lora_modulation_params(&gs_handle, SX1262_LORA_DEFAULT_SF, SX1262_LORA_DEFAULT_BANDWIDTH,
                                            SX1262_LORA_DEFAULT_CR, SX1262_LORA_DEFAULT_LOW_DATA_RATE_OPTIMIZE);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set lora modulation params failed.\n");
        (void)sx1262_deinit(&gs_handle);

        return 1;
    }

    /* convert the frequency */
    res = sx1262_frequency_convert_to_register(&gs_handle, SX1262_LORA_DEFAULT_RF_FREQUENCY, (uint32_t *)&reg);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: convert to register failed.\n");
        (void)sx1262_deinit(&gs_handle);

        return 1;
    }

    /* Image calibration for the selected 915 MHz band (902-928 MHz). */
    res = sx1262_set_calibration_image(&gs_handle, 0xE1, 0xE9);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: image calibration failed.\n");
        (void)sx1262_deinit(&gs_handle);
        return 1;
    }

    /* set the frequency */
    res = sx1262_set_rf_frequency(&gs_handle, reg);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set rf frequency failed.\n");
        (void)sx1262_deinit(&gs_handle);

        return 1;
    }

    /* set base address */
    res = sx1262_set_buffer_base_address(&gs_handle, 0x00, 0x00);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set buffer base address failed.\n");
        (void)sx1262_deinit(&gs_handle);

        return 1;
    }

    /* set lora symb num */
    res = sx1262_set_lora_symb_num_timeout(&gs_handle, SX1262_LORA_DEFAULT_SYMB_NUM_TIMEOUT);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set lora symb num timeout failed.\n");
        (void)sx1262_deinit(&gs_handle);

        return 1;
    }

    /* reset stats */
    res = sx1262_reset_stats(&gs_handle, 0x0000, 0x0000, 0x0000);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: reset stats failed.\n");
        (void)sx1262_deinit(&gs_handle);

        return 1;
    }

    /* clear device errors */
    res = sx1262_clear_device_errors(&gs_handle);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: clear device errors failed.\n");
        (void)sx1262_deinit(&gs_handle);

        return 1;
    }

    /* set the lora sync word */
    res = sx1262_set_lora_sync_word(&gs_handle, SX1262_LORA_DEFAULT_SYNC_WORD);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set lora sync word failed.\n");
        (void)sx1262_deinit(&gs_handle);

        return 1;
    }

    /* A completed SPI transfer alone does not prove the radio received it. */
    res = sx1262_get_lora_sync_word(&gs_handle, &sync_word);
    if (res != 0 || sync_word != SX1262_LORA_DEFAULT_SYNC_WORD)
    {
        sx1262_interface_debug_print("sx1262: SPI sync word readback failed; check NSS/MISO/MOSI/SCK.\n");
        (void)sx1262_deinit(&gs_handle);
        return 1;
    }

    /* get tx modulation */
    res = sx1262_get_tx_modulation(&gs_handle, (uint8_t *)&modulation);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get tx modulation failed.\n");
        (void)sx1262_deinit(&gs_handle);

        return 1;
    }
    modulation |= 0x04;

    /* set the tx modulation */
    res = sx1262_set_tx_modulation(&gs_handle, modulation);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set tx modulation failed.\n");
        (void)sx1262_deinit(&gs_handle);

        return 1;
    }

    /* set the rx gain */
    res = sx1262_set_rx_gain(&gs_handle, SX1262_LORA_DEFAULT_RX_GAIN);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set rx gain failed.\n");
        (void)sx1262_deinit(&gs_handle);

        return 1;
    }

    /* set the ocp */
    res = sx1262_set_ocp(&gs_handle, SX1262_LORA_DEFAULT_OCP);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set ocp failed.\n");
        (void)sx1262_deinit(&gs_handle);

        return 1;
    }

    /* get the tx clamp config */
    res = sx1262_get_tx_clamp_config(&gs_handle, (uint8_t *)&config);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: get tx clamp config failed.\n");
        (void)sx1262_deinit(&gs_handle);

        return 1;
    }
    config |= 0x1E;

    /* set the tx clamp config */
    res = sx1262_set_tx_clamp_config(&gs_handle, config);
    if (res != 0)
    {
        sx1262_interface_debug_print("sx1262: set tx clamp config failed.\n");
        (void)sx1262_deinit(&gs_handle);

        return 1;
    }

    return 0;
}

// uint8_t Driver_Lora_SendData(uint8_t *data, uint16_t len)
// {
//     TX_HIGH;
//     RX_LOW;

//     /* set dio irq */
//     if (sx1262_set_dio_irq_params(&gs_handle, SX1262_IRQ_TX_DONE | SX1262_IRQ_TIMEOUT | SX1262_IRQ_CAD_DONE | SX1262_IRQ_CAD_DETECTED,
//                                   SX1262_IRQ_TX_DONE | SX1262_IRQ_TIMEOUT | SX1262_IRQ_CAD_DONE | SX1262_IRQ_CAD_DETECTED,
//                                   0x0000, 0x0000) != 0)
//     {
//         return 1;
//     }

//     /* clear irq status */
//     if (sx1262_clear_irq_status(&gs_handle, 0x03FFU) != 0)
//     {
//         return 1;
//     }

//     if (sx1262_lora_transmit(&gs_handle, SX1262_CLOCK_SOURCE_XTAL_32MHZ,
//                              SX1262_LORA_DEFAULT_PREAMBLE_LENGTH, SX1262_LORA_DEFAULT_HEADER,
//                              SX1262_LORA_DEFAULT_CRC_TYPE, SX1262_LORA_DEFAULT_INVERT_IQ,
//                              (uint8_t *)data, len, 0) != 0)
//     {
//         return 1;
//     }

//     return 0;
// }

uint8_t Driver_Lora_SendData(uint8_t *data, uint16_t len)
{
    uint8_t res;

    if (data == NULL || len == 0 || len > 255)
    {
        return 10;
    }

    RX_LOW;
    TX_HIGH;

    res = sx1262_set_dio_irq_params(
        &gs_handle,
        SX1262_IRQ_TX_DONE | SX1262_IRQ_TIMEOUT,
        SX1262_IRQ_TX_DONE | SX1262_IRQ_TIMEOUT,
        0,
        0);

    if (res != 0)
    {
        TX_LOW;
        printf("Set IRQ failed: %u\r\n", res);
        return res;
    }

    res = sx1262_lora_transmit(
        &gs_handle,
        SX1262_CLOCK_SOURCE_XTAL_32MHZ,
        SX1262_LORA_DEFAULT_PREAMBLE_LENGTH,
        SX1262_LORA_DEFAULT_HEADER,
        SX1262_LORA_DEFAULT_CRC_TYPE,
        SX1262_LORA_DEFAULT_INVERT_IQ,
        data,
        len,
        0);

    if (res != 0)
    {
        Driver_Lora_PrintStatus();
        (void)sx1262_set_standby(&gs_handle, SX1262_CLOCK_SOURCE_RC_13M);
    }
    TX_LOW;

    return res;
}

uint8_t Driver_Lora_EnterRxMode(void)
{
    TX_LOW;
    RX_HIGH;

    uint8_t setup;

    /* set dio irq */
    if (sx1262_set_dio_irq_params(&gs_handle, SX1262_IRQ_RX_DONE | SX1262_IRQ_TIMEOUT | SX1262_IRQ_CRC_ERR | SX1262_IRQ_CAD_DONE | SX1262_IRQ_CAD_DETECTED,
                                  SX1262_IRQ_RX_DONE | SX1262_IRQ_TIMEOUT | SX1262_IRQ_CRC_ERR | SX1262_IRQ_CAD_DONE | SX1262_IRQ_CAD_DETECTED,
                                  0x0000, 0x0000) != 0)
    {
        return 1;
    }

    /* clear irq status */
    if (sx1262_clear_irq_status(&gs_handle, 0x03FFU) != 0)
    {
        return 1;
    }

    /* set lora packet params */
    if (sx1262_set_lora_packet_params(&gs_handle, SX1262_LORA_DEFAULT_PREAMBLE_LENGTH,
                                      SX1262_LORA_DEFAULT_HEADER, SX1262_LORA_DEFAULT_BUFFER_SIZE,
                                      SX1262_LORA_DEFAULT_CRC_TYPE, SX1262_LORA_DEFAULT_INVERT_IQ) != 0)
    {
        return 1;
    }

    /* get iq polarity */
    if (sx1262_get_iq_polarity(&gs_handle, (uint8_t *)&setup) != 0)
    {
        return 1;
    }

#if SX1262_LORA_DEFAULT_INVERT_IQ == SX1262_BOOL_FALSE
    setup |= 1 << 2;
#else
    setup &= ~(1 << 2);
#endif

    /* set the iq polarity */
    if (sx1262_set_iq_polarity(&gs_handle, setup) != 0)
    {
        return 1;
    }

    /* start receive */
    if (sx1262_continuous_receive(&gs_handle) != 0)
    {
        return 1;
    }

    return 0;
}

void Driver_Lora_RecvData(uint8_t *rxBuff, uint16_t *rxLen)
{
    sx1262_irq_handler(&gs_handle);

    if (gs_handle.receive_len > 0)
    {
        *rxLen = gs_handle.receive_len;
        memcpy(rxBuff, gs_handle.receive_buf, *rxLen);
        gs_handle.receive_len = 0;
    }
}
