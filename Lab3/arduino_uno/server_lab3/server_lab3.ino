

String connect_arduino, game_started, game_mode, ai_strategy, message, next_turn;
String board[3][3];


void setup() {

  Serial.begin(9600);  // Налаштування серійного зв'язку зі швидкістю 9600 біт/с
  randomSeed(analogRead(0));  // Ініціалізація генератора випадкових чисел
}

void loop() {
  if (Serial.available() > 0) {

    String receivedMessage = Serial.readString();  // Читаємо дані з серійного порту
    parseJSON(receivedMessage);

    if (connect_arduino == "0"){
      connect_arduino = "1";
    }


    if (game_started == "1" && game_mode == "mva" && ai_strategy == "rand") { makeRandomMove(); }

    if (game_started == "1" && game_mode == "ava" && ai_strategy == "rand") { avaMove(); }

    if (game_started == "1" && game_mode == "mva" && ai_strategy == "win") { stratMove("o", true); }

    if (game_started == "1" && game_mode == "ava" && ai_strategy == "win"){
      if (next_turn == "o") { stratMove("o", false); }
      else { stratMove("x", false); }
    }

    if (game_started == "1" && game_mode == "mvm") { check_mvm(); }


    String output = buildJSON();
    output.replace("\n", ""); // Заміна /n на пустий рядок
    Serial.print(output);

  }
}


bool checkBoardWin(String board[3][3], String player) {
  // Перевірка рядків (горизонталі)
  for (int i = 0; i < 3; i++) {
    if (board[i][0] == player && board[i][1] == player && board[i][2] == player) {
      return true;
    }
  }

  // Перевірка колонок (вертикалі)
  for (int i = 0; i < 3; i++) {
    if (board[0][i] == player && board[1][i] == player && board[2][i] == player) {
      return true;
    }
  }

  // Перевірка діагоналей
  if (board[0][0] == player && board[1][1] == player && board[2][2] == player) {
    return true;
  }
  if (board[0][2] == player && board[1][1] == player && board[2][0] == player) {
    return true;
  }

  return false;
}

bool allFieldsOccupied(String board[3][3]) {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      if (board[i][j] != "x" && board[i][j] != "o") {
        return false; // Знайшли пусте поле, повертаємо false
      }
    }
  }
  return true; // Якщо всі поля заповнені
}

bool checkForWinner() {

  if (checkBoardWin(board, "x")) {
    message = "Winner_X";
    game_started = "0";
    return true;
  }
  else if (checkBoardWin(board, "o")) {
    message = "Winner_O";
    game_started = "0";
    return true;
  }
  else if (allFieldsOccupied(board)) {
    message = "Draw";
    game_started = "0";
    return true;
  }
  else { 
    return false;
  }
}



void avaMove() {
  int emptyCells[9];
  int emptyCount = 0;
  int x_cells = 0;
  int o_cells = 0;

  if (checkForWinner()) { return; }

  // Проходимо через поле та знаходимо всі порожні клітинки
  for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 3; col++) {

      if (board[row][col] == "x") {
        x_cells++;
      }
      else if (board[row][col] == "o") {
        o_cells++;
      }
      else {
        emptyCells[emptyCount] = row * 3 + col;  // Зберігаємо індекс клітинки у вигляді одного числа
        emptyCount++;
      }

    }
  }

  // Якщо є порожні клітинки, вибираємо випадкову
  if (emptyCount > 0) {
    int randomIndex = random(0, emptyCount);  // Випадковий індекс від 0 до emptyCount-1
    int rand_cell = emptyCells[randomIndex];  // Індекс випадкової клітинки

    // Перетворюємо індекс назад у координати (row, col)
    int row = rand_cell / 3;
    int col = rand_cell % 3;

    if(next_turn == "o"){
      board[row][col] = "o"; 
      next_turn = "x";
    }
    else {
      board[row][col] = "x"; 
      next_turn = "o";
    }  
  }
  else {
    message = "Draw";
    game_started = "0";
  }
}

void check_mvm() {
  int emptyCells[9];
  int emptyCount = 0;
  int x_cells = 0;
  int o_cells = 0;

  if (checkForWinner()) { return; }

  // Проходимо через поле та знаходимо всі порожні клітинки
  for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 3; col++) {

      if (board[row][col] == "x") {
        x_cells++;
      }
      else if (board[row][col] == "o") {
        o_cells++;
      }
      else {
        emptyCells[emptyCount] = row * 3 + col;  // Зберігаємо індекс клітинки у вигляді одного числа
        emptyCount++;
      }
    }
  }

  // Вибір гравця для першого ходу
  if (emptyCount == 9) {
    int randomChoice = random(0, 2);  // Випадковий вибір: 0 або 1
    if (randomChoice == 0) {
      message = "Player_X_turn";
      next_turn = "x";
      return;
    }
    else {
      message = "Player_O_turn";
      next_turn = "o";
      return;
    }
  }

  if(next_turn == "x") {
    message = "Player_O_turn";
    next_turn = "o";
  }
  else {
    message = "Player_X_turn";
    next_turn = "x";
  }
}

void makeRandomMove() {
  int emptyCells[9];
  int emptyCount = 0;
  int x_cells = 0;
  int o_cells = 0;

  if (checkForWinner()) { return; }

  // Проходимо через поле та знаходимо всі порожні клітинки
  for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 3; col++) {

      if (board[row][col] == "x") {
        x_cells++;
      }
      else if (board[row][col] == "o") {
        o_cells++;
      }
      else {
        emptyCells[emptyCount] = row * 3 + col;  // Зберігаємо індекс клітинки у вигляді одного числа
        emptyCount++;
      }
    }
  }

  next_turn = "x";
  message = "Player_x_turn";

  if (emptyCount == 9) {
    int randomChoice = random(0, 2);  // Випадковий вибір: 0 або 1
    if (randomChoice == 0) {
      return;
    }
  }

  // Якщо є порожні клітинки, вибираємо випадкову
  if (emptyCount > 0) {
    int randomIndex = random(0, emptyCount);  // Випадковий індекс від 0 до emptyCount-1
    int rand_cell = emptyCells[randomIndex];  // Індекс випадкової клітинки

    // Перетворюємо індекс назад у координати (row, col)
    int row = rand_cell / 3;
    int col = rand_cell % 3;

    // Записуємо "o" в обрану клітинку
    board[row][col] = "o"; 
  }

  checkForWinner();
}



bool checkWin(String player, String currentPlayer) {
  for (int i = 0; i < 3; i++) {
    // Перевірка рядків і стовпців
    if ((board[i][0] == player && board[i][1] == player && board[i][2] == "-") ||
        (board[i][0] == player && board[i][2] == player && board[i][1] == "-") ||
        (board[i][1] == player && board[i][2] == player && board[i][0] == "-")) {
      board[i][0] == "-" ? board[i][0] = currentPlayer : 
      board[i][1] == "-" ? board[i][1] = currentPlayer : board[i][2] = currentPlayer;
      return true;
    }
    
    if ((board[0][i] == player && board[1][i] == player && board[2][i] == "-") ||
        (board[0][i] == player && board[2][i] == player && board[1][i] == "-") ||
        (board[1][i] == player && board[2][i] == player && board[0][i] == "-")) {
      board[0][i] == "-" ? board[0][i] = currentPlayer : 
      board[1][i] == "-" ? board[1][i] = currentPlayer : board[2][i] = currentPlayer;
      return true;
    }
  }

  // Перевірка діагоналей
  if ((board[0][0] == player && board[1][1] == player && board[2][2] == "-") ||
      (board[0][0] == player && board[2][2] == player && board[1][1] == "-") ||
      (board[1][1] == player && board[2][2] == player && board[0][0] == "-")) {
    board[0][0] == "-" ? board[0][0] = currentPlayer : 
    board[1][1] == "-" ? board[1][1] = currentPlayer : board[2][2] = currentPlayer;
    return true;
  }

  if ((board[0][2] == player && board[1][1] == player && board[2][0] == "-") ||
      (board[0][2] == player && board[2][0] == player && board[1][1] == "-") ||
      (board[1][1] == player && board[2][0] == player && board[0][2] == "-")) {
    board[0][2] == "-" ? board[0][2] = currentPlayer : 
    board[1][1] == "-" ? board[1][1] = currentPlayer : board[2][0] = currentPlayer;
    return true;
  }

  return false;
}

void stratMove(String currentPlayer, bool randomPlayer) {

  if (checkForWinner()) { return; }

  bool made_move = false;

  String opponent = (currentPlayer == "x") ? "o" : "x";

  next_turn = opponent;
  message = "Player_"+ opponent +"_turn";

  int emptyCount = 0;

  for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 3; col++) {

      if (board[row][col] != "x" && board[row][col] != "0") {
        emptyCount++;
      }
    }
  }

  if (randomPlayer){
    if (emptyCount == 9) {
      int randomChoice = random(0, 2);  // Випадковий вибір: 0 або 1
      if (randomChoice == 0) {
        return;
      }
    }
  }
  
  // 1. Спробуємо виграти
  if (!made_move && checkWin(currentPlayer, currentPlayer)) {
    made_move = true;
  } 

  // 2. Заблокуємо "x", якщо він може виграти
  if (!made_move && checkWin(opponent, currentPlayer)) {
    made_move = true;
  }

  // 3. Займаємо центр, якщо він вільний
  if (!made_move && board[1][1] == "-") {
    board[1][1] = currentPlayer;
    made_move = true;
  }

  // 4. Займаємо кути, якщо вони вільні
  if (!made_move && board[0][0] == "-") {
    board[0][0] = currentPlayer;
    made_move = true;
  }
  if (!made_move && board[0][2] == "-") {
    board[0][2] = currentPlayer;
    made_move = true;
  }
  if (!made_move && board[2][0] == "-") {
    board[2][0] = currentPlayer;
    made_move = true;
  }
  if (!made_move && board[2][2] == "-") {
    board[2][2] = currentPlayer;
    made_move = true;
  }

  // 5. Займаємо будь-яке інше місце
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {

      if (!made_move && board[i][j] == "-") {
        board[i][j] = currentPlayer;
        made_move = true;
      }
    }
  }

  if (checkForWinner()) { return; }

}



String buildJSON() {
  String output = "{";
  output += "\"con\":\"" + connect_arduino + "\",";
  output += "\"gs\":\"" + game_started + "\",";
  output += "\"gm\":\"" + game_mode + "\",";
  output += "\"ais\":\"" + ai_strategy + "\",";
  output += "\"msg\":\"" + message + "\",";
  output += "\"nt\":\"" + next_turn + "\",";
  output += "\"brd\":{";
  output += "\"c11\":\"" + board[0][0] + "\",";
  output += "\"c12\":\"" + board[0][1] + "\",";
  output += "\"c13\":\"" + board[0][2] + "\",";
  output += "\"c21\":\"" + board[1][0] + "\",";
  output += "\"c22\":\"" + board[1][1] + "\",";
  output += "\"c23\":\"" + board[1][2] + "\",";
  output += "\"c31\":\"" + board[2][0] + "\",";
  output += "\"c32\":\"" + board[2][1] + "\",";
  output += "\"c33\":\"" + board[2][2] + "\"";
  output += "}";
  output += "}";
  return output;
}

String getJSONValue(String input, String key) {
  int start = input.indexOf("\"" + key + "\":\"") + key.length() + 4;
  int end = input.indexOf("\"", start);
  
  if (start == -1 || end == -1) {
    return "-";
  }

  return input.substring(start, end);
}

void parseJSON(String input) {
  connect_arduino = getJSONValue(input, "con");
  game_started = getJSONValue(input, "gs");
  game_mode = getJSONValue(input, "gm");
  ai_strategy = getJSONValue(input, "ais");
  message = getJSONValue(input, "msg");

  board[0][0] = getJSONValue(input, "c11");
  board[0][1] = getJSONValue(input, "c12");
  board[0][2] = getJSONValue(input, "c13");
  board[1][0] = getJSONValue(input, "c21");
  board[1][1] = getJSONValue(input, "c22");
  board[1][2] = getJSONValue(input, "c23");
  board[2][0] = getJSONValue(input, "c31");
  board[2][1] = getJSONValue(input, "c32");
  board[2][2] = getJSONValue(input, "c33");
}


