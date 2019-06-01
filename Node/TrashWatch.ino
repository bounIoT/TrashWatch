#define BLYNK_GREEN     "#23C48E"
#define BLYNK_BLUE      "#04C0F8"
#define BLYNK_YELLOW    "#ED9D00"
#define BLYNK_RED       "#D3435C"
#define BLYNK_DARK_BLUE "#5F7CD8"
#define BLYNK_WHITE "#FFFFFF"

#include <Bridge.h>
#include <YunClient.h>
#include <YunServer.h>
#include <BlynkSimpleYun.h>

char auth[] = "b23ed1ccbbb34c9697b45e817d2a267e";

// ultrasonic sensor pin configuration
int trigPin = 11; // trig is connected to d11
int echoPin = 12; // echo is connected to d12

// led pin configuration
int ledpin = 13; // led is connected to d13

// measures to calculate distance and fullness
long duration;
long distance;
int fullness = 0;
int fulldist = 20; // the distance reading then the bin is full
int emptydist = 50; // the distance reading then the bin is empty

// UI LED Configuration
WidgetLED led1(V1); // UI LED is on v1

// the threshold to alert the user
double threshold = 30; // the user is alerted when the distance is < 30 cm

// flags
bool isOn = false; // indicates whether the led is on or off, isOn == true when the trash is filled, false when trash is not full
bool first = true; // indicates whether this is the first time the loop function is called

void setup() {
  Blynk.begin(auth); // start connection via WiFi

  // set the pins to input or output as needed
  pinMode(ledpin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // UI LED will have maximum brightness and will start at off state
  led1.setValue(255);
  led1.off();  
}

void loop() {

  // this is needed because when this block runs on setup, the UI does not work as expected
  if(first){
    // the "will be collected" button will always be false since initially the bin will be considered empty
    // can be considered as a deactivation of the button and configuration UI
    Blynk.setProperty(V2, "onColor", BLYNK_WHITE);
    Blynk.setProperty(V2, "offColor", BLYNK_WHITE);
    Blynk.setProperty(V2, "onBackColor", BLYNK_RED);
    Blynk.setProperty(V2, "offBackColor", BLYNK_RED);
    Blynk.setProperty(V2, "onLabel", "False");
    Blynk.setProperty(V2, "offLabel", "False");
    Blynk.virtualWrite(V2, 0);
    Blynk.setProperty(V0, "color", BLYNK_GREEN);
    Blynk.run(); // this function call prevents connection loss
    first = false;
  }

  // calculate distance and fullness
  distance = getDistance();
  fullness = getFullness(distance);

  // sends fullness information to user
  Blynk.virtualWrite(V0, fullness);

  // delay added to get more accurate measurements
  delay(5000);
  
  Blynk.run(); // this function call prevents connection loss
  
  if(distance < threshold && !isOn){  // if distance is less than threshold and we are in the state "trash bin is not filled"
    bool check = true; // flag to indicate whether the test is passed

    // we check if the trash is really filled by making 5 more measurements periodically every 5 seconds
    for(int i = 0; i < 5; i++){
      distance = getDistance();
      fullness = getFullness(distance);
      
      Blynk.run();
      delay(5000);
      Blynk.run();
      
      // if in one of the measurements we see the ditance is more than the threshold, we conclude that the trash is actually not filled and cancel the test
      if(distance >= threshold){ 
        check = false;
        break;
      }
    }
    
    // if in each of the tests we see the distance is less than the threshold, the test is passed, we switch state to "trash bin is filled"
    if(check){ 
      digitalWrite(ledpin, HIGH); // light up the LED
      Blynk.virtualWrite(V1,1); // change the value of V1 to 1 in order to send notification  notification logic is defined in blyk app)
      
      led1.on(); // light up the UI LED
      isOn = true; // we are in state " trash bin is filled"

      // activate the "will be collected button" and reconfigure the UI according to the state 
      Blynk.setProperty(V2, "onBackColor", BLYNK_GREEN);
      Blynk.setProperty(V2, "offBackColor", BLYNK_RED);
      Blynk.setProperty(V2, "onLabel", "True");
      Blynk.setProperty(V2, "offLabel", "False");
      Blynk.virtualWrite(V2, 0);
      Blynk.setProperty(V0, "color", BLYNK_RED);
      Blynk.run();
    }
  } else if(distance >= threshold && isOn){ // if the distance is more than the threshold and we are in state "trash bin is not filled"
    // same logic applies, but this time we check whether the bin is really empty
    bool check = true;
    for(int i = 0; i < 5; i++){
      distance = getDistance();
      fullness = getFullness(distance);
      
      Blynk.run();
      delay(5000);
      Blynk.run();
      
      if(distance < threshold){
        check = false;
        break;
      }
    }
    if(check){ // if test is passed, change state to "trash bin is not filled"
      digitalWrite(ledpin, LOW); // turn off the led
      Blynk.virtualWrite(V1,0); // set v1's value back to 0
      
      led1.off(); // turn off the UI LED
      isOn = false; change state to "trash bin is not filled"
      
      // deactivate the button and reconfigure the UI according to the state
      Blynk.setProperty(V2, "onBackColor", BLYNK_RED);
      Blynk.setProperty(V2, "offBackColor", BLYNK_RED);
      Blynk.setProperty(V2, "onLabel", "False");
      Blynk.setProperty(V2, "offLabel", "False");
      Blynk.virtualWrite(V2, 0);
      Blynk.setProperty(V0, "color", BLYNK_GREEN);
      Blynk.run();
    }
  }

  // this if else block is needed because when the user stops the project and starts it back, the UI might need to be reconfigured
  // reconfigure the data accoreding to the state
  if(isOn){
      Blynk.run();
      Blynk.setProperty(V2, "onBackColor", BLYNK_GREEN);
      Blynk.setProperty(V2, "offBackColor", BLYNK_RED);
      Blynk.setProperty(V2, "onLabel", "True");
      Blynk.setProperty(V2, "offLabel", "False");
      Blynk.setProperty(V0, "color", BLYNK_RED);
      Blynk.run();
  } else{
      Blynk.run();
      Blynk.setProperty(V2, "onBackColor", BLYNK_RED);
      Blynk.setProperty(V2, "offBackColor", BLYNK_RED);
      Blynk.setProperty(V2, "onLabel", "False");
      Blynk.setProperty(V2, "offLabel", "False");
      Blynk.setProperty(V0, "color", BLYNK_GREEN);
      Blynk.run();
  }
 
  Blynk.run(); 
}

// measures the distance between ultrasonic sensor and trashes
double getDistance(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);

  distance = (duration/2)/29.1;
  //Blynk.virtualWrite(V0, (distance));
  
  return distance;
}

// calculates the fullness as a percentage according to the distance of the trashes
int getFullness(double distance){
  if(distance < fulldist){
    return 100;
  } else if(distance > emptydist){
    return 0;
  }
  return 100*((emptydist - fulldist) - (distance - fulldist))/(emptydist - fulldist);
}
