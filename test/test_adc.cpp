#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  pinMode(A0, INPUT);
}

void loop() {
  int adcValue = analogRead(A0);
  float voltage = adcValue * (3.3 / 1023.0); // 将ADC值转换为电压值
  
  Serial.print("ADC Raw Value: ");
  Serial.print(adcValue);
  Serial.print(" | Voltage: ");
  Serial.print(voltage, 3);
  Serial.println("V");
  
  delay(1000);
}
