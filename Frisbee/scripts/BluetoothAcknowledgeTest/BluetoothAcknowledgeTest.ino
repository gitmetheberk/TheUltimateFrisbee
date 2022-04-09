void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial2.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial2.print("ComCheck\n");
  String received = Serial2.readString();
  Serial.println(received);

  if (received.startsWith("ComEstablished"))
  {
    Serial.println("Sending data");
    Serial2.print("SendingData\n");
    for (int i = 0; i < 300; i++)
    {
      Serial2.print(i);
      Serial2.print(',');
    }
    Serial2.print("\nEndTransmission\n");
  }
  else
  {
    Serial.println("Delaying");
    delay(1000);
  }
}
