
/* Code to interface with C# program */
/* This will read data from the computer's serial/USB port. */
/* The program will look for the letters 'G' or 'S' to turn */
/* the LED on or off. These letters can be anything, just */
/* make sure it's the same as what the C# program is sending */


int Led = 11; // using pin 11 for the LED
int InputData = 0;

void setup() {
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
  pinMode(Led, OUTPUT); // sets pin as output

  digitalWrite(Led, HIGH); // Cycle led on/off during startup to
  delay(300);              // show program is running
  digitalWrite(Led, LOW);
  delay(300);
  digitalWrite(Led, HIGH);
  delay(300);
  digitalWrite(Led, LOW);
}


void loop() {


 while (Serial.available() > 0) { // do while serial data is available
    
    InputData = Serial.read();

 }

            //not used----> Serial.println(Csharp, DEC); //sends data TO computer

    if (InputData == 'G') digitalWrite(Led, HIGH);   // set the LED on
    if (InputData == 'S') digitalWrite(Led, LOW);   // set the LED on

}
