
#include <QDZHx_Fault_Handler.h>

const uint8_t FAULT_SIGNAL_PIN = 3; // Digital pin the signal is being read from;
uint8_t lastDiagnosticStatus = 0;
uint8_t faultCycleStatus = 0;
bool lastPinStatus = LOW;

QDZHx_Fault_Handler FaultMon(FAULT_SIGNAL_PIN);

void setup() {
  Serial.begin(9600);
  FaultMon.init();
}

void loop() {
  FaultMon._processInterrupt();
  if ( FaultMon.confirmedFault != lastDiagnosticStatus ){
    lastDiagnosticStatus = FaultMon.confirmedFault;
    Serial.print("Diagnostic status changed to ");
    Serial.println(FaultMon.confirmedFault);
  }

  if ( FaultMon.faultCycleStatus != faultCycleStatus ){
    faultCycleStatus = FaultMon.faultCycleStatus;

    if ( faultCycleStatus == 0 ){
      Serial.println("Fault cleared!");
    }
  }
}
