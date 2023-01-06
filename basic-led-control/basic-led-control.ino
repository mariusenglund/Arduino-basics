/********************************************************************************************************
    Basic LED-control
    
    A simple program to dim, blink and fade an LED with millis() using a button and a potensiometer.
    
    The program have 4 modes, which can be changed with the push button:
    * OFF:      The LED will turn off.
    * DIM:      The LED will dim to the brightness set by the potensiometer.
    * BLINK:    The LED will blink with the frequency set by the potensiometer.
    * FADE:     The LED will fade in/out over a period of time set by the potensiometer.
    
    For the active modes, the value of the potensiometer gets printed to the serial monitor when changed.
********************************************************************************************************/


/***************
DEFINE VARIABLES
***************/
// IO pins:
const int buttonPin             = 2;        // Push button pin.
const int potPin                = A0;       // Potensiometer pin.
const int ledPin                = 3;        // LED pin (PWM).

// Program variables:
bool buttonState;                           // Push button state.
bool lastButtonState;                       // Last push button state (for edge detection).
int potValue;                               // Potensiometer value (0-1023).
int lastPotValue;                           // Last potensiometer value (for reference).
int dimBrightness;                          // Brightness of LED in dim mode (0-255).
int frequency;                              // Frequency of blinks in blink mode [Hz].
int minFrequency                = 1;        // Minimum allowed frequency in blink mode [Hz].
int maxFrequency                = 100;      // Maximum allowed frequency in blink mode [Hz].
int fadeTime;                               // Time from darkest to brightnest in fade mode [ms].
int minFadeTime                 = 1;        // Minimum allowed fade time in fade mode [ms].
int maxFadeTime                 = 10000;    // Maximum allowed fade time in fade mode [ms].
int fadeAmount                  = 1;        // Amount of delta when changing brightness in fade mode (0-255).
int fadeBrightness              = 0;        // Brightness of LED in fade mode (0-255)
String ledModes[]               = {"off", "dim", "blink", "fade"};  // Available LED modes.
int ledModeIndex                = 0;        // Index for current LED mode.
unsigned long currentTime;                  // Time variable [ms].
unsigned long oldTime           = 0;        // Time variable for reference [ms].


/*********
INITIALIZE
*********/
void setup() {
    // START SERIAL MONITOR:
    Serial.begin(9600);
    while (!Serial);

    // CONNECT INPUT PINS:
    pinMode(buttonPin, INPUT);
    pinMode(potPin, INPUT);

    // CONNECT OUTPUT PINS:
    pinMode(ledPin, OUTPUT);

    // DEFINE START VALUES FROM CURRENT PIN STATES:
    buttonState = lastButtonState = digitalRead(buttonPin);
    potValue = lastPotValue = analogRead(potPin);

    // PRINT CURRENT LED MODE:
    printLedMode(ledModes[ledModeIndex]);
}


/**********
CONTROL LED
**********/
void loop() {
    // READ INPUT PINS:
    buttonState = digitalRead(buttonPin);
    potValue = analogRead(potPin);

    // SET VALUES FROM POTENSIOMETER:
    dimBrightness = map(potValue, 0, 1023, 0, 255);
    frequency = map(potValue, 0, 1023, minFrequency, maxFrequency);
    fadeTime = map(potValue, 0, 1023, minFadeTime, maxFadeTime);

    // PRINT VALUE OF POTENSIOMETER WHEN CHANGED:
    if (potValue != lastPotValue) {
        // The potensiometer is changed.
        
        // Print brightness when in dim mode:
        if (ledModes[ledModeIndex] == "dim") {
            Serial.print("Brightness: ");
            Serial.print(map(dimBrightness, 0, 255, 0, 100));
            Serial.println(" %");
        }

        // Print frequency when in blink mode:
        else if (ledModes[ledModeIndex] == "blink") {
            Serial.print("Frequency: ");
            Serial.print(frequency);
            Serial.println(" Hz");
        }

        // Print fade time when in fade mode:
        else if (ledModes[ledModeIndex] == "fade") {
            Serial.print("Fade time: ");
            Serial.print(fadeTime);
            Serial.println(" ms");
        }
        lastPotValue = potValue;
    }

    // CHANGE MODE WHEN BUTTON IS PRESSED:
    if (buttonState != lastButtonState) {
        if (buttonState == HIGH) {
            // The button went from off to on.

            // Change mode:
            ledModeIndex += 1;
            if (ledModeIndex >= (sizeof(ledModes)/sizeof(ledModes[0]))) {
                ledModeIndex = 0; // Start over when reaching last mode.
            }

            // Print new mode:
            printLedMode(ledModes[ledModeIndex]);
        }
        lastButtonState = buttonState;
    }
    
    // CONTROL THE LED DIFFERENTLY IN DIFFERENT MODES:
    // Turn off LED when in off mode:
    if (ledModes[ledModeIndex] == "off") {
        if (digitalRead(ledPin) == HIGH) {
            digitalWrite(ledPin, LOW);
        }
    }

    // Dim LED to current brightness when in dim mode:
    else if (ledModes[ledModeIndex] == "dim") {
        analogWrite(ledPin, dimBrightness);
    }

    // Blink LED with current frequency when in blink mode:
    else if (ledModes[ledModeIndex] == "blink") {
        currentTime = millis();
        int frequency_ms = 1/frequency;
        if (currentTime - oldTime >= frequency_ms) {
            if (digitalRead(ledPin) == LOW) {
                digitalWrite(ledPin, HIGH);
            }
            else {
                digitalWrite(ledPin, LOW);
            }
            oldTime = currentTime;
        }
    }

    // Fade LED with current fade cycle when in fade mode:
    else if (ledModes[ledModeIndex] == "fade") {
        analogWrite(ledPin, fadeBrightness);
        currentTime = millis();
        if (currentTime - oldTime >= fadeTime/(fadeAmount*255)) {
            fadeBrightness += fadeAmount;
            if ((fadeBrightness <= 0) || (fadeBrightness >= 255)) {
                fadeAmount = -fadeAmount; // Reverse fade direction when reaching limits.
            }
            oldTime = currentTime;
        }
    }
}


/**********************
DEFINE CUSTOM FUNCTIONS
**********************/
// Print LED mode:
void printLedMode(String ledMode) {
    Serial.print("LED mode: ");
    ledMode.toUpperCase();
    Serial.println(ledMode);
}