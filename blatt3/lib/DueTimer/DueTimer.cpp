#include <Arduino.h>
#include <inttypes.h>
#include "DueTimer.h"

#define USING_SERVO_LIB // Uncomment to enable use with Arduino's Servo library
#define DUE_F_CPU 84000000
#define INVALID_TIMER 255

#ifdef USING_SERVO_LIB
#define TIMER_AVAILABLE false
#else
#define TIMER_AVAILABLE true
#endif

DueTimer::HWTimer DueTimer::timers[9] =
{
  { TIMER_AVAILABLE, TC0->TC_CHANNEL, ID_TC0, TC0_IRQn, NULL },
  { true, TC0->TC_CHANNEL + 1, ID_TC1, TC1_IRQn, NULL },
  { TIMER_AVAILABLE, TC0->TC_CHANNEL + 2, ID_TC2, TC2_IRQn, NULL },
  { TIMER_AVAILABLE, TC1->TC_CHANNEL, ID_TC3, TC3_IRQn, NULL },
  { TIMER_AVAILABLE, TC1->TC_CHANNEL + 1, ID_TC4, TC4_IRQn, NULL },
  { TIMER_AVAILABLE, TC1->TC_CHANNEL + 2, ID_TC5, TC5_IRQn, NULL },
  { true, TC2->TC_CHANNEL, ID_TC6, TC6_IRQn, NULL },
  { true, TC2->TC_CHANNEL + 1, ID_TC7, TC7_IRQn, NULL },
  { true, TC2->TC_CHANNEL + 2, ID_TC8, TC8_IRQn, NULL }
};

DueTimer::DueTimer() : id(INVALID_TIMER)
{
  // Get next available hardware timer
  for (auto i = 0; i < 9; i++)
  {
    if (timers[i].is_available)
    {
      id = i;
      timers[i].is_available = false;
      break;
    }
  }
}

DueTimer::~DueTimer()
{
  // Stop, reset and make the timer available again
  if (id != INVALID_TIMER)
  {
    stop();
    timers[id].is_available = true;
    timers[id].callback = NULL;
  }
}

bool DueTimer::configure(const uint32_t hz, void (*callback)(void))
{
  // Fail if not a valid timer (all timers used)
  if (id == INVALID_TIMER) { return false; }

  // Store the interrupt callback function
  timers[id].callback = callback;

  // Enable the clock signal in the PMC
  pmc_set_writeprotect(false);
  pmc_enable_periph_clk(timers[id].pmc_id);

  // Set timer clock using the smallest available divisor (i.e. 2) for the master clock
  timers[id].tc_channel->TC_CMR =
    TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK1;

  // Determine and set the compare value corresponding to the frequency
  uint32_t cmp_value = (DUE_F_CPU / 2 / hz) - 1;
  timers[id].tc_channel->TC_RC = cmp_value;

  // Enable timer's "RC Compare" interrupt
  timers[id].tc_channel->TC_IER = TC_IER_CPCS;
  timers[id].tc_channel->TC_IDR = ~TC_IDR_CPCS;

  // Reset and enable interrupt handling in the NVIC
  NVIC_ClearPendingIRQ(timers[id].nvic_irqn);
  NVIC_EnableIRQ(timers[id].nvic_irqn);

  return true;
}

bool DueTimer::start()
{
  // Fail if current object is not a valid (or usable) timer
  if (id == INVALID_TIMER || timers[id].callback == NULL) { return false; }

  // Enable the counter clock, reset the counter and start the clock
  timers[id].tc_channel->TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;

  return true;
}

bool DueTimer::stop()
{
  // Fail if current object is not a valid timer
  if (id == INVALID_TIMER) { return false; }

  // Disable the counter clock
  timers[id].tc_channel->TC_CCR = TC_CCR_CLKDIS;

  return true;
}

#ifndef USING_SERVO_LIB
void TC0_Handler(void)
{
  DueTimer::timers[0].tc_channel->TC_SR;
  DueTimer::timers[0].callback();
}

void TC2_Handler(void)
{
  DueTimer::timers[2].tc_channel->TC_SR;
  DueTimer::timers[2].callback();
}

void TC3_Handler(void)
{
  DueTimer::timers[3].tc_channel->TC_SR;
  DueTimer::timers[3].callback();
}

void TC4_Handler(void)
{
  DueTimer::timers[4].tc_channel->TC_SR;
  DueTimer::timers[4].callback();
}

void TC5_Handler(void)
{
  DueTimer::timers[5].tc_channel->TC_SR;
  DueTimer::timers[5].callback();
}
#endif

void TC1_Handler(void)
{
  DueTimer::timers[1].tc_channel->TC_SR;
  DueTimer::timers[1].callback();
}

void TC6_Handler(void)
{
  DueTimer::timers[6].tc_channel->TC_SR;
  DueTimer::timers[6].callback();
}

void TC7_Handler(void)
{
  DueTimer::timers[7].tc_channel->TC_SR;
  DueTimer::timers[7].callback();
}

void TC8_Handler(void)
{
  DueTimer::timers[8].tc_channel->TC_SR;
  DueTimer::timers[8].callback();
}

