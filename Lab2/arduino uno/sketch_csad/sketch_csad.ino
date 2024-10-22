void setup() {
  pinMode(13, OUTPUT); //Initialization of foam for light bulb
  digitalWrite(13, HIGH);

  Serial.begin(9600);  //Configuring serial communication
  while (!Serial);     //Wait until the port becomes available (only for Leonardo and similar boards)
  // delay(2000);    
  digitalWrite(13, LOW);
}

void loop() {
  if (Serial.available() > 0) {
    String receivedMessage = Serial.readString();  //Read data from the serial port
    digitalWrite(13, HIGH);

    String parsedMessage = parseJSON(receivedMessage);
    
    //Convert all letters to uppercase
    for (int i = 0; i < parsedMessage.length(); i++) {
      parsedMessage[i] = toupper(parsedMessage[i]);
    }

    String jsonToSend = createJSON(parsedMessage);

    digitalWrite(13, LOW);
    //Send the modified message back
    Serial.print(jsonToSend);
  }
}

//JSON creation function
String createJSON(String message) {
  String json = "{\"message\":\"";
  json += message;
  json += "\"}";
  return json;
}

//JSON parsing function
String parseJSON(String json) {
  int start = json.indexOf("\"message\":\"") + 11; 
  int end = json.indexOf("\"", start);
  
  if (start != -1 && end != -1) {
    return json.substring(start, end);
  }
  
  return ""; //If JSON is invalid
}
