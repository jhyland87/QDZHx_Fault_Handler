// Tells compiler this file is to be included only once in a single compilation
#pragma once

#ifndef FAULT_SETTINGS_H
#define FAULT_SETTINGS_H


// Times used for determining what state the fault report from the compressor is in
constexpr unsigned int  FAULT_SIG_INTERIM_MS          = 250;  // Milliseconds between flashes of the current sequence (1/4 of a second)
constexpr unsigned int  FAULT_SIG_DURATION_MS         = 250;  // How many ms is the signal expected to stay in HIGH state for?
constexpr unsigned int  FAULT_REPORT_INTERIM_MS       = 5000; // Miliseconds between when the last blink of the error ends and the 
                                                              // first blink of the next error.
constexpr unsigned int  FAULT_FUDGE_FACTOR_MS         = 100;  // Just some extra MS to add to the checks to account for slight variations,
                                                              // (which there shouldn't actually be any)

// States of the fault analysis loop iterations
constexpr unsigned int  FAULT_CYCLE_STATE_OK          = 0;    // No error found
constexpr unsigned int  FAULT_CYCLE_STATE_MID_REPORT  = 1;    // In process of reporting (signals detected but not yet completed sending)
constexpr unsigned int  FAULT_CYCLE_STATE_REPORTED    = 2;    // Fault report cycle completed (ie: the beep has stopped for ~4 seconds)
constexpr unsigned int  FAULT_CYCLE_STATE_ERROR       = 3;    // When an unexpected state is encountered (eg: LED signal on for too long)


constexpr unsigned int INTERRUPT_HANDLER_DETACHED     = false;
constexpr unsigned int INTERRUPT_HANDLER_ATTACHED     = true;

#endif