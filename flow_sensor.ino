// GLOBALS
byte sensorInterrupt = 0;  // 0 = digital pin 2
byte sensorPin       = 2;

// The hall-effect flow sensor outputs approximately 4.5 pulses per second per
// litre/minute of flow.
float hallCalibrationFactor = 4.5;

volatile byte pulseCount;  

float flowRate;
unsigned int flowMilliLitres;
unsigned long timeDifference;

void setup(){
  
    // serial port setup 
    Serial.begin(9600);

    pinMode(sensorPin, INPUT);
    digitalWrite(sensorPin, HIGH);

    pulseCount        = 0;
    flowRate          = 0.0;
    flowMilliLitres   = 0;
    totalMilliLitres  = 0;
    timeDifference    = 0;

    // The Hall-effect sensor is connected to pin 2 which uses interrupt 0.
    // Triggers when falling - impuls HIGH to LOW
    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
}

void loop(){
    // Do things once per second
    if((millis() - timeDifference) > 1000){ 
        // Disable the interrupt while calculating flow rate
        detachInterrupt(sensorInterrupt);

        // Calculate pulse counts per loop (loop may not complete in 1 second interval).
        // apply the hallCalibrationFactor to scale the output
        // based on the number of pulses per second per units of measure
        flowRate = ((1000.0 / (millis() - timeDifference)) * pulseCount) / hallCalibrationFactor;

        // millis(), returns the number of milliseconds since the Arduino board began running the current program. 
        // This number will overflow (go back to zero), after approximately 50 days.
        timeDifference = millis();

        // Divide the flow rate in litres/minute by 60 to determine how many litres have
        // passed through the sensor in this 1 second interval, then multiply by 1000 to
        // convert to millilitres.
        flowMilliLitres = (flowRate / 60) * 1000;

        // Add the millilitres passed in this second to the cumulative total
        totalMilliLitres += flowMilliLitres;

        Serial.print("Exhaled air [mL]: ");
        Serial.print("\t");
        Serial.print(flowMilliLitres);
        Serial.print("\t");

        // Print the flow rate [L/min] 
        Serial.print("Flow rate[L/min] : ");
        Serial.print("\t");
        Serial.println(flowRate);

        resetPulseCounter();

        // Enable the interrupt
        attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
    }
}

void pulseCounter(){
    // Interrupt routine
    // Increment the pulse counter.
    pulseCount++;
}

void resetPulseCounter(){
    // Reset pulse counter 
    pulseCount = 0;
}
