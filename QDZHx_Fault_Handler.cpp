// Tells compiler this file is to be included only once in a single compilation
#pragma once

#include "QDZHx_Fault_Handler.h"

QDZHx_Fault_Handler::QDZHx_Fault_Handler(uint8_t _pin) : _pin {_pin} {}

QDZHx_Fault_Handler::~QDZHx_Fault_Handler() { this->end(); }

// Im sure there's a better way to do this. I should probably have these in a separate header
// file (eg: FaultCodeLookup.hpp or something)
/*
const char* QDZHx_Fault_Handler::faultExceptions[] = { 
  "OK",
  "Battery protection cut-out", 
  "Fan over-current cut-out", 
  "Motor start error", 
  "Minimum motor speed error", 
  "Thermal cut-out of electronic unit", 
  "Controller hardware failure"
};
const char fault_0[] PROGMEM = "System healthy"; 
const char fault_1[] PROGMEM = "Battery protection cut-out";
const char fault_2[] PROGMEM = "Fan over-current cut-out";
const char fault_3[] PROGMEM = "Motor start error";
const char fault_4[] PROGMEM = "Min. motor speed error";
const char fault_5[] PROGMEM = "Thermal ECU cut-out";
const char fault_6[] PROGMEM = "Ctroller HW failure";

//PROGMEM const char * myArray[] =
const char *const faultExceptions[] PROGMEM =
{ 
  fault_0, 
  fault_1, 
  fault_2, 
  fault_3, 
  fault_4, 
  fault_5, 
  fault_6 
};
*/

void QDZHx_Fault_Handler::init() {
  //pinMode( this->_pin, INPUT_PULLUP );

  //attachInterruptCtx(digitalPinToInterrupt( this->_pin ));
  QDZHx_Fault_Handler::init(false);
}

void QDZHx_Fault_Handler::init(bool asInterrupt) {
  pinMode( this->_pin, INPUT_PULLUP );

  if ( asInterrupt == true ){
    //attachInterruptCtx(digitalPinToInterrupt( this->_pin ));
  }
}

void QDZHx_Fault_Handler::end() {
  if ( this->_attached == INTERRUPT_HANDLER_ATTACHED ){
    //detachInterruptCtx(digitalPinToInterrupt( this->_pin ));
  }
}

void QDZHx_Fault_Handler::_processInterrupt() { 
  unsigned long currentMs = millis();

  int msSinceLastStateChange = (currentMs - this->_lastChangeMs);

  int pinValue = digitalRead( this->_pin );

  // Is the interrupt pin in a different value than it was in the last check?..
  if ( pinValue != this->_lastPinValue ){
    // .. If so, then this is likely triggered by the interrupt..
   
   /*
    Serial.print("Pin has updated from ");
    Serial.print(this->_lastPinValue );
    Serial.print(" to ");
    Serial.println(pinValue);
    */

    //
    // Encountering state change - likely triggered by fault CHANGE interrupt
    //

    if ( pinValue == HIGH ){
      this->faultCycleStatus = FAULT_CYCLE_STATE_MID_REPORT;

      // Increment the counter since were just enterd a new HIGH signal
      this->_counter++;
    }
    else {
      // Todo
    }
    
    this->_lastChangeMs = millis();
  }

  //
  // Conditions below here are likely not triggered by an interrupt, but rather a manual
  // call to follow up on an active fault report cycle
  //
  else if ( pinValue == HIGH ) {
    //
    // Signal is HIGH and was HIGH on last _processInterrupt() execution. This
    // was either executed manually too soon or there's an issue with the 
    // fault signal that's keeping it at HIGH
    //

    if ( msSinceLastStateChange > ( FAULT_SIG_DURATION_MS + FAULT_FUDGE_FACTOR_MS ) ) {
      // I will add better error handling/reporting later.
      /*
      Serial.print("ERROR: The interrupt pin has been in HIGH state for ");
      Serial.print(msSinceLastStateChange);
      Serial.print(" ms (");
      Serial.print( FAULT_SIG_DURATION_MS - msSinceLastStateChange);
      Serial.println(" ms longer than expected)");
      */
      this->faultCycleStatus = FAULT_CYCLE_STATE_ERROR;
    }

  }

  else if ( msSinceLastStateChange > ( FAULT_REPORT_INTERIM_MS + FAULT_FUDGE_FACTOR_MS ) ) {
    // 
    // Fault signal has been LOW for longer than it would take for a fault report
    // cycle to be triggered by a CHANGE interrupt - thus, we can confidently
    // infer that the fault has been cleared.
    //

    // Clear the fault cycle status
    this->faultCycleStatus  = FAULT_CYCLE_STATE_OK;

    this->confirmedFault = 0;
    
  }
  else if ( msSinceLastStateChange > ( FAULT_SIG_INTERIM_MS + FAULT_FUDGE_FACTOR_MS ) && this->_counter > 0 ){
    //
    // Fault signal has been LOW for longer than it would take for another signal
    // to increment the report peak _counter - thus, we can confidently infer that
    // the compressor has finished sending all peaks for this fault, and can now
    // determine what the fault is.
    //

    // Update fault cycle status to reported
    this->faultCycleStatus  = FAULT_CYCLE_STATE_REPORTED;

    this->confirmedFault = this->_counter;

    // And reset the interrupt signal counter
    this->_counter = 0;
    
    /*
    // Set the last confirmed fault
    if ( this->_counter > 0 && this->confirmedFault != this->_counter ){
      Serial.print("Changing this->confirmedFault from ");
      Serial.print(this->confirmedFault);
      Serial.print(" to ");
      Serial.println(this->_counter);

      this->confirmedFault = this->_counter;

      // And reset the interrupt signal counter
      this->_counter = 0;
    }
    */
  }
  else {
    // Todo 
  }
  
  // Store the current pin value so it can be compared against in the next cycle
  this->_lastPinValue = pinValue;
}

void QDZHx_Fault_Handler::attachInterruptCtx(int interrupt) {
  if ( this->_attached == INTERRUPT_HANDLER_ATTACHED ) {
    //FATAL_ERROR(F("This instance was attached already"));
    //Serial.println("This instance was attached already");
    return;
  }

  QDZHx_Fault_Handler::instance_handler = this; 
  
  this->_attached = INTERRUPT_HANDLER_ATTACHED;

  
  attachInterrupt(interrupt, []{ 
    QDZHx_Fault_Handler::instance_handler->_processInterrupt();
  }, CHANGE);
  
}

void QDZHx_Fault_Handler::detachInterruptCtx(int interrupt) {
  //detachInterrupt(interrupt);
  this->_attached = INTERRUPT_HANDLER_DETACHED;
}

QDZHx_Fault_Handler *QDZHx_Fault_Handler::instance_handler {};