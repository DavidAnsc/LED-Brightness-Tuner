#include "driver/ledc.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "soc/gpio_num.h"
#include "esp_attr.h"

#define FREQ_HZ 5000
#define PWM_BITS LEDC_TIMER_10_BIT

volatile uint16_t duty = (1u << 10);
volatile uint8_t state = 0;

void IRAM_ATTR buttonIsr(void* args) {
  if (state == 3) {
    state = -1;
  }
  ++state;

  switch (state) {
    case (0):
      duty = (1u << 10);
      break;
    case (1):
      duty = (1u << 9);
      break;
    case (2):
      duty = (7u << 6);
      break;
    case (3):
      duty = (1u << 8);
      break;
  }

  ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

void initButton() {
  gpio_config_t io_conf;
  
  io_conf.mode = GPIO_MODE_INPUT;
  io_conf.pin_bit_mask = 1ULL << GPIO_NUM_0;
  io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;

  io_conf.intr_type = GPIO_INTR_NEGEDGE;

  gpio_config(&io_conf);

  gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
  gpio_isr_handler_add(GPIO_NUM_0, buttonIsr, nullptr);
}

void app_main(void) {
  initButton();

  ledc_timer_config_t pwmTimer = {
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .duty_resolution = PWM_BITS,
    .timer_num = LEDC_TIMER_0,
    .freq_hz = FREQ_HZ,
    .clk_cfg = LEDC_AUTO_CLK
  };

  ESP_ERROR_CHECK(ledc_timer_config(&pwmTimer));

  ledc_channel_config_t pwmTimerChannelConfig = {
    .gpio_num = GPIO_NUM_2,
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel = LEDC_CHANNEL_0,
    .timer_sel = LEDC_TIMER_0,
    .intr_type = LEDC_INTR_DISABLE,
    .duty = duty,
    .hpoint = 0
  };
  ESP_ERROR_CHECK(ledc_channel_config(&pwmTimerChannelConfig));
}
