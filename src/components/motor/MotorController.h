#pragma once

#include <FreeRTOS.h>
#include <timers.h>
#include <cstdint>

namespace Pinetime {
  namespace Controllers {

    class MotorController {
    public:
      MotorController() = default;

      void Init();
      void RunForDuration(uint8_t motorDuration);
      void Pulse(uint32_t interval, uint8_t repetitions);
      void StartRinging();
      void StopRinging();

    private:
      static void Ring(TimerHandle_t xTimer);
      static void RingWithLatch(TimerHandle_t xTimer);
      static void StopMotor(TimerHandle_t xTimer);
      TimerHandle_t shortVib;
      TimerHandle_t longVib;
      TimerHandle_t pulse;
      uint8_t pulseLatch;
    };
  }
}
