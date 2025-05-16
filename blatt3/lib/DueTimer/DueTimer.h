#ifndef DUETIMER_H
#define DUETIMER_H

#include <Arduino.h>
#include <inttypes.h>

class DueTimer
{
public:
  DueTimer();
  ~DueTimer();
  bool configure(const uint32_t hz, void (*callback)(void));
  bool start();
  bool stop();

private:
  uint8_t id;

  typedef struct
  {
    bool is_available;
    TcChannel *tc_channel;
    uint32_t pmc_id;
    IRQn nvic_irqn;
    void (*callback)(void);
  } HWTimer;
  static HWTimer timers[9];

  // Timer ISRs declared "friends" to allow access to class variables for callbacks
  friend void TC0_Handler(void);
  friend void TC1_Handler(void);
  friend void TC2_Handler(void);
  friend void TC3_Handler(void);
  friend void TC4_Handler(void);
  friend void TC5_Handler(void);
  friend void TC6_Handler(void);
  friend void TC7_Handler(void);
  friend void TC8_Handler(void);
};

#endif // DUETIMER_H
