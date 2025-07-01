double temp;
double water;
float R = 10000.0; // resistor for voltage divider in temp sensor

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(13, INPUT);
  pinMode(A0, INPUT); // water
  pinMode(A1, INPUT); // temperature
}

void loop() {
  // put your main code here, to run repeatedly:
  water = analogRead(A0);
  temp = analogRead(A1)/1023.0;
  temp = temp_cal(temp); 
  
  Serial.print("Temp, Water: ");
  Serial.print(temp);
  Serial.print(",");
  Serial.println(water);
  
}

double temp_cal(float V_in){
  double R_temp;
  R_temp =  ((5*R)/V_in) - R;
  return R_temp;
}
