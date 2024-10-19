void setup() {
  pinMode(13, OUTPUT); // Ініціалізація піну для лампочки
  digitalWrite(13, HIGH);

  Serial.begin(9600);  // Налаштування серійного зв'язку зі швидкістю 9600 біт/с
  while (!Serial);     // Очікуємо поки порт стане доступним (тільки для Leonardo і схожих плат)
  // delay(2000);    
  digitalWrite(13, LOW);
}

void loop() {
  if (Serial.available() > 0) {
    String receivedMessage = Serial.readString();  // Читаємо дані з серійного порту
    digitalWrite(13, HIGH);

    String parsedMessage = parseJSON(receivedMessage);
    
    // Перетворюємо всі літери в верхній регістр
    for (int i = 0; i < parsedMessage.length(); i++) {
      parsedMessage[i] = toupper(parsedMessage[i]);
    }

    String jsonToSend = createJSON(parsedMessage);

    digitalWrite(13, LOW);
    // Відправляємо змінене повідомлення назад
    Serial.print(jsonToSend);
  }
}

// Функція створення JSON
String createJSON(String message) {
  String json = "{\"message\":\"";
  json += message;
  json += "\"}";
  return json;
}

// Функція парсингу JSON
String parseJSON(String json) {
  int start = json.indexOf("\"message\":\"") + 11; // 11 - це довжина рядка до значення "message"
  int end = json.indexOf("\"", start);
  
  if (start != -1 && end != -1) {
    return json.substring(start, end);
  }
  
  return ""; // Якщо JSON некоректний
}
