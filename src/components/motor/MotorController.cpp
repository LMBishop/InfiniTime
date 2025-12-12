#include "components/motor/MotorController.h"
#include <hal/nrf_gpio.h>
#include <portmacro_cmsis.h>
#include "systemtask/SystemTask.h"
#include "drivers/PinMap.h"

using namespace Pinetime::Controllers;

void MotorController::Init() {
  nrf_gpio_cfg_output(PinMap::Motor);
  nrf_gpio_pin_set(PinMap::Motor);

  shortVib = xTimerCreate("shortVib", 1, pdFALSE, nullptr, StopMotor);
  longVib = xTimerCreate("longVib", pdMS_TO_TICKS(1000), pdTRUE, this, Ring);
  pulse = xTimerCreate("pulse", 1, pdTRUE, this, RingWithLatch);
}

void MotorController::Ring(TimerHandle_t xTimer) {
  auto* motorController = static_cast<MotorController*>(pvTimerGetTimerID(xTimer));
  motorController->RunForDuration(50);
}

void MotorController::RingWithLatch(TimerHandle_t xTimer) {
  auto* motorController = static_cast<MotorController*>(pvTimerGetTimerID(xTimer));
  if (--(motorController->pulseLatch) == 0) {
    xTimerStop(motorController->pulse, 0);
    return;
  }
  motorController->RunForDuration(50);
}

void MotorController::RunForDuration(uint8_t motorDuration) {
  if (motorDuration > 0 && xTimerChangePeriod(shortVib, pdMS_TO_TICKS(motorDuration), 0) == pdPASS && xTimerStart(shortVib, 0) == pdPASS) {
    nrf_gpio_pin_clear(PinMap::Motor);
  }
}

void MotorController::Pulse(uint32_t interval, uint8_t repetitions) {
  pulseLatch = repetitions;
  if (xTimerChangePeriod(pulse, pdMS_TO_TICKS(interval), 0) == pdPASS) {
    xTimerStart(pulse, 0);
  }
}

void MotorController::StartRinging() {
  RunForDuration(50);
  xTimerStart(longVib, 0);
}

void MotorController::StopRinging() {
  xTimerStop(longVib, 0);
  nrf_gpio_pin_set(PinMap::Motor);
}

void MotorController::StopMotor(TimerHandle_t /*xTimer*/) {
  nrf_gpio_pin_set(PinMap::Motor);
}
