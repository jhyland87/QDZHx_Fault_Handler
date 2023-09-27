/**
 * Some of the core OOP related code shared by PieterP:
 *    https://forum.arduino.cc/t/interrupt-in-class-esp32/1039326/52
 */
// Tells compiler this file is to be included only once in a single compilation
#pragma once

#ifndef FAULT_HANDLER_H
#define FAULT_HANDLER_H

#ifndef FAULT_HANDLER_SETTINGS_H
#include "Settings.h"
#endif

#include <Arduino.h>

class QDZHx_Fault_Handler {
public: 

  /// Constructor: doesn't do anything until init() is called.
  QDZHx_Fault_Handler(uint8_t pin);

  /// Destructor: calls end().
  ~QDZHx_Fault_Handler();
  
  /// Copy constructor: meaningless, so it has been deleted.
  QDZHx_Fault_Handler(const QDZHx_Fault_Handler &) = delete;

  /// Copy assignment: meaningless, so it has been deleted.
  QDZHx_Fault_Handler &operator=(const QDZHx_Fault_Handler &) = delete;

  /// Move constructor: omitted for brevity.
  QDZHx_Fault_Handler(QDZHx_Fault_Handler &&other) = delete;

  /// Move assignment: omitted for brevity.
  QDZHx_Fault_Handler &operator=(QDZHx_Fault_Handler &&other) = delete;

  //void followup();

  /// Attach the interrupt.
  void init();

  void init(bool asInterrupt);

  /// Detach the interrupt.
  void end();
  
  // Brief explanation of what each error code means (0 - 6)
  //static const char* faultExceptions[];

  /// Get the actual count. Use with interrupts enabled only.
  unsigned read() const;

  // Taken out of private for now, though I may leave it here
  void _processInterrupt();

private:
  /// Single pointer to the active instance
  static QDZHx_Fault_Handler *instance_handler; 

  /// Private handler function that is called from the ISR.
  //void _processInterrupt();

  /// Register the interrupt handler for this instance.
  void attachInterruptCtx(int interrupt);
  
  /// Un-register the interrupt handler for this instance.
  void detachInterruptCtx(int interrupt);

private:
  uint8_t _pin;
  uint8_t _lastPinValue       = LOW;
  uint8_t _attached           = INTERRUPT_HANDLER_DETACHED;
  volatile unsigned _counter  = 0;
  unsigned long _lastChangeMs = millis();
  

public:
  // Status of a fault cycle
  // 0 = FAULT_CYCLE_STATE_OK
  // 1 = FAULT_CYCLE_STATE_MID_REPORT
  // 2 = FAULT_CYCLE_STATE_REPORTED
  // 3 = FAULT_CYCLE_STATE_ERROR
  uint8_t faultCycleStatus  = FAULT_CYCLE_STATE_OK;

  // Last confirmed fault 0 - 6
  uint8_t confirmedFault    = FAULT_CYCLE_STATE_OK;

  // The time the last fault was reported (the ms the last HIGH signal ended)
  unsigned long lastFaultReportMs;
};

inline unsigned QDZHx_Fault_Handler::read() const {
  //noInterrupts();
  //auto tmp = _counter;
  //interrupts();
  //return tmp;
  return confirmedFault;
}

#endif