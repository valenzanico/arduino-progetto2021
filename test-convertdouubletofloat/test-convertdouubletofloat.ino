void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  

}

void loop() {
  // put your main code here, to run repeatedly:
  float a = 24.2947124147;
  double b = (double) a;
  Serial.println(b);
  delay(2000);
}
