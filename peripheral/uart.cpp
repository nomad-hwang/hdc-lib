#include "uart.hpp"

Error UartReceiver::init() {
  lwrb_init(&m_rb, m_rb_buffer, UART_RX_BUFFER_SIZE);

  HAL_UART_RegisterCallback(m_huart, HAL_UART_ERROR_CB_ID, [](UART_HandleTypeDef *huart) { m_ref.get(huart)->handle_error(huart); });

  if (m_mode == DMA) {
    HAL_UART_RegisterRxEventCallback(m_huart, [](UART_HandleTypeDef *huart, uint16_t Pos) { m_ref.get(huart)->handle_rx_event_dma(huart, Pos); });
    HAL_UARTEx_ReceiveToIdle_DMA(m_huart, m_rx_buffer, UART_RX_BUFFER_SIZE);
  } else {
    HAL_UART_RegisterRxEventCallback(m_huart, [](UART_HandleTypeDef *huart, uint16_t Pos) { m_ref.get(huart)->handle_rx_event_it(huart, Pos); });
    HAL_UARTEx_ReceiveToIdle_IT(m_huart, m_rx_buffer, UART_RX_BUFFER_SIZE);
  }

  m_initialized = true;

  return Error::OK;
}

int UartReceiver::read() {
  assert(m_initialized);

  char c;
  return lwrb_read(&m_rb, (uint8_t *)&c, 1) == 1 ? c : -1;
}

int UartReceiver::read(uint8_t *data, size_t size) {
  assert(m_initialized);

  return lwrb_read(&m_rb, data, size);
}

void UartReceiver::handle_rx_event_dma(UART_HandleTypeDef *_, uint16_t pos) {
  // uint16_t pos = UART_RX_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(m_huart->hdmarx);

  if (pos == m_old_pos) {
    return;
  }

  if (pos > m_old_pos) {
    lwrb_write(&m_rb, m_rx_buffer + m_old_pos, pos - m_old_pos);
  } else {
    lwrb_write(&m_rb, m_rx_buffer + m_old_pos, UART_RX_BUFFER_SIZE - m_old_pos);
    lwrb_write(&m_rb, m_rx_buffer, pos);
  }

  m_old_pos = pos;

  if (m_callback) {
    m_callback(this);
  }

  if (m_task_handle) {
    vTaskNotifyGiveFromISR(m_task_handle, NULL);
  }
}

void UartReceiver::handle_rx_event_it(UART_HandleTypeDef *_, uint16_t Pos) {
  lwrb_write(&m_rb, m_rx_buffer, Pos);
  HAL_UARTEx_ReceiveToIdle_IT(m_huart, m_rx_buffer, UART_RX_BUFFER_SIZE);

  if (m_callback) {
    m_callback(this);
  }

  if (m_task_handle) {
    vTaskNotifyGiveFromISR(m_task_handle, NULL);
  }
}

void UartReceiver::handle_error(UART_HandleTypeDef *huart) {
  // TODO: handle various errors(maybe no reinit for some errors such as frame, noise, overrun)
  HAL_UART_MspDeInit(huart);
  HAL_UART_MspInit(huart);
  m_ref.get(huart)->init();
}

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

int UartTransmitter::init() {
  lwrb_init(&m_rb, m_rb_buffer, UART_RX_BUFFER_SIZE);

  m_transmit = m_mode == DMA ? HAL_UART_Transmit_DMA : HAL_UART_Transmit_IT;

  HAL_UART_RegisterCallback(m_huart, HAL_UART_TX_COMPLETE_CB_ID, [](UART_HandleTypeDef *huart) { m_ref.get(huart)->handle_tx_cplt(huart); });
  HAL_UART_RegisterCallback(m_huart, HAL_UART_ERROR_CB_ID, [](UART_HandleTypeDef *huart) { m_ref.get(huart)->handle_error(huart); });

  m_initialized = true;

  return 0;
}

size_t UartTransmitter::write(const uint8_t *data, size_t size) {
  assert(m_initialized);

  __disable_irq();

  if (m_tx_running) {
    size_t written = lwrb_write(&m_rb, data, size);
    __enable_irq();
    return written;
  }

  m_tx_running = true;

  __enable_irq();

  memcpy(m_tx_buffer, data, size);
  return m_transmit(m_huart, m_tx_buffer, size) == HAL_OK ? size : 0;
}

void UartTransmitter::handle_tx_cplt(UART_HandleTypeDef *huart) {
  if (lwrb_get_full(&m_rb) == 0) {
    m_tx_running = false;
  } else {
    if (HAL_UART_Transmit_DMA(huart, m_tx_buffer, lwrb_read(&m_rb, m_tx_buffer, UART_TX_BUFFER_SIZE)) != HAL_OK) {
      m_tx_running = false;
    }
  }
}

void UartTransmitter::handle_error(UART_HandleTypeDef *huart) {
  HAL_UART_MspDeInit(huart);
  HAL_UART_MspInit(huart);
  m_ref.get(huart)->init();
  m_tx_running = false;
}

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
