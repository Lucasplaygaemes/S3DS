/*
    3D scanner simulator - Position sender
    This code simule a 3d scanner using 6 buttons.
    He Sends the changes DeltaX, DelayY and the distance Z
    trough the serial port in the format: <DeltaX, DeltaY, DistnaceZ>
*/
// Pins for the sensor
const int TRIG_PIN = 7;
const int ECHO_PIN = 8;
// Pins for movement
const int BTN_RIGHT   = 2;
const int BTN_LEFT    = 3;
const int BTN_UP      = 4;
const int BTN_DOWN    = 5;
const int BTN_SAVE    = 6;
long current_distance_cm = 0;
long last_button_time = 0;
const long debounceDelay = 50;
void setup() {
    Serial.begin(9600);
    
    //sensor
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    // buttons
    pinMode(BTN_RIGHT,INPUT_PULLUP);
    pinMode(BTN_LEFT,INPUT_PULLUP);
    pinMode(BTN_UP,INPUT_PULLUP);
    pinMode(BTN_DOWN,INPUT_PULLUP);
    Serial.println("Scanner 3D simulator Ready.");
}
long measure_distance_cm() {
    // default ping
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    long duration = pulseIn(ECHO_PIN, HIGH);
    
    return duration / 29/ 2;
}
void loop() {
    int deltaX = 0;
    int deltaY = 0;
    bool is_saving = false;
    //uses debounce to be sure to count only once for touch
    if ((millis() - last_button_time) > debounceDelay) {
        // --- Delta x, lateral movement logic ---
        if (digitalRead(BTN_RIGHT) == LOW) {
            deltaX = 1;
        } else if (digitalRead(BTN_LEFT) == LOW) {
            deltaX = -1;
        }
        // --- delta y, vertical movement logic ---
        if (digitalRead(BTN_UP) == LOW) {
            deltaY = 1;
        }
        if (digitalRead(BTN_DOWN) == LOW) {
            deltaY = -1;
        }
        
        if (digitalRead(BTN_SAVE) == LOW || deltaX != 0 || deltaY != 0) {
            is_saving = true;
        }
        
        // --- serial sender ---
        if (is_saving) {
            
            long distanceZ = measure_distance_cm();
            
            //send format: <DeltaX, DeltaY, DistnaceZ>
            Serial.print("<");
            Serial.print(deltaX); Serial.print(",");
            Serial.print(deltaY); Serial.print(",");
            Serial.print(distanceZ);
            Serial.println(">");
            
            last_button_time = millis();
        }
    }
}
