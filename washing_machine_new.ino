
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>


#define FIREBASE_HOST "insert your firebase host link"              //insert firebase host link
#define FIREBASE_AUTH "insert your Auth Key"                        //Auth key dari firebase
#define WIFI_SSID "insert your wifi name"                           //wifi id tukar ikut network yang digunakan
#define WIFI_PASSWORD "insert your wifi password"                   //password wifi


//variable declaration
float currentMillis, interval  = 0;
//variable motor
int PWM = D0;
int Dir1 = D1;
int Dir2 = D2;

//int led = D4;

//variable vibration sensor
int vib = D3;
//variable reed
int reed = D4;


//universal variable
int _duration;
int _vibrate;
int _lid;
int wm_usestatus;
float x;

void setup() {
  Serial.begin(9600);     //initiate serial monitor

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);   //initiate firebase

  //declare motor sebagai output
  pinMode(PWM, OUTPUT);
  pinMode(Dir1, OUTPUT);
  pinMode(Dir2, OUTPUT);


  //declare vibration sensor sebagai output
  pinMode(vib, INPUT);
  //declare magnetic switch sebagai output
  pinMode(reed, INPUT);
}
void loop() {

  _lid = digitalRead(reed);
  _duration = Firebase.getInt("03/duration");
  wm_usestatus = Firebase.getBool("03/isUsed");
  _vibrate = digitalRead(_vibrate);


  //lid close
  if (_lid == LOW) {
    Serial.println("the lid is close,washing machine ready");
    Firebase.setBool("03/isDoorOpened", 0);

    //wm is idle
    if (wm_usestatus == 0 && _duration == 0) {
      motoroff();
      Serial.println("the washing machine ready");
      currentMillis = millis();
      interval = millis() - currentMillis;
    }


    //wm being used
    else if (wm_usestatus == 1 && _duration > 0) {

      x = _duration * 1000;  //value duration yang diletakan dalam app akan didarab dengan 1000,sebab 1s sama dengan 1000ms
      Serial.println(x);
      Serial.println(interval);


      //wm operate within the duration set
      if (interval < x) {
        Serial.println("motor on");
        motor();
        //            digitalWrite(led,HIGH);
        interval = millis() - currentMillis;
        if ( _vibrate == HIGH ) {
          Serial.print("WASHING");
        }
        else {
          Serial.print("ERROR!");
        }
      }
      else {
        currentMillis = millis();
        interval = millis() - currentMillis;
        Serial.print("motor off");
        motoroff();
        Firebase.setInt("03/duration", 0);
      }
    }


    //wm done washing
    else if (wm_usestatus == 1 && _duration == 0 ) {
      motoroff();

      currentMillis = millis();
      interval = millis() - currentMillis;
      Serial.println("the washing is done please unload");
    }
  }


  //lid open
  else {
    Serial.println("the lid is open,please close the lid");
    Firebase.setBool("03/isDoorOpened", 0);
    Firebase.setInt("03/duration", 0);
    Firebase.setBool("03/isUsed", 0);
    motoroff();
  }
}



//function motor
void motor() {
  digitalWrite (Dir1, HIGH);
  digitalWrite (Dir2, LOW);
  analogWrite  (PWM, 180);

}
void motoroff() {
  digitalWrite (Dir1, LOW);
  digitalWrite (Dir2, LOW);
  analogWrite  (PWM, 0);

}
