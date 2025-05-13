// Primary Engineers Radar Code

// Radar Left
const int RadarLeftTrig = 2;
const int RadarLeftEcho = 3;
long DurationLeft;
float DistanceLeft;

//Radar Centre
const int RadarCentreTrig = 4;
const int RadarCentreEcho = 5;
long DurationCentre;
float DistanceCentre;

//Radar Right
const int RadarRightTrig = 6;
const int RadarRightEcho = 7;
long DurationRight;
float DistanceRight;

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
//Left Radar
pinMode(RadarLeftTrig,OUTPUT);
pinMode(RadarLeftEcho,INPUT);
//Centre Radar
pinMode(RadarCentreTrig,OUTPUT);
pinMode(RadarCentreEcho,INPUT);
//Right Radar
pinMode(RadarRightTrig,OUTPUT);
pinMode(RadarRightEcho,INPUT);
}

void loop() {
  // === LEFT SENSOR ===
  digitalWrite(RadarLeftTrig, LOW);
  delayMicroseconds(2);
  digitalWrite(RadarLeftTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(RadarLeftTrig, LOW);
  DurationLeft = pulseIn(RadarLeftEcho, HIGH);
  DistanceLeft = DurationLeft * 0.034 / 2;

  Serial.print("Left Sensor Distance = ");
  Serial.println(DistanceLeft);


  // === Centre Sensor ===
  digitalWrite(RadarCentreTrig, LOW);
  delayMicroseconds(2);
  digitalWrite(RadarCentreTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(RadarCentreTrig, LOW);
  DurationCentre = pulseIn(RadarCentreEcho, HIGH);
  DistanceCentre = DurationCentre * 0.034 / 2;

  Serial.print("Centre Sensor Distance = ");
  Serial.println(DistanceCentre);

  // === Right Sensor ===
  digitalWrite(RadarRightTrig,LOW);
  delayMicroseconds(2);
  digitalWrite(RadarRightTrig,HIGH);
  delayMicroseconds(10);
  digitalWrite(RadarRightTrig,LOW);
  DurationRight = pulseIn(RadarRightEcho,HIGH);
  DistanceRight = DurationRight * 0.034 / 2;

  Serial.print("Right Sensor Distance = ");
  Serial.println(DistanceRight);


}
