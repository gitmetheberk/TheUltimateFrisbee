void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial5.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial5.print("ComCheck\n");
  String received = Serial5.readString();
  Serial.println(received);

  if (received.startsWith("ComEstablished"))
  {
    Serial.println("Sending data");
    Serial5.print("SendingData\n");
    for (int i = 0; i < 300; i++)
    {
      Serial5.print(i);
      Serial5.print(',');
    }
    Serial5.print("\nEndTransmission\n");
  }
  else
  {
    Serial.println("Delaying");
    delay(1000);
  }
}
