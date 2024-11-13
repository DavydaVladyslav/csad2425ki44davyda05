/**
 * @file server_lab3.ino
 */
String connect_arduino, game_started, game_mode, ai_strategy, message, next_turn;
String board[3][3];

/**
 * @brief Sets up the Arduino by initializing serial communication and random seed.
 * Configures the serial port and prepares random number generation for AI moves.
 */
void setup() {

  Serial.begin(9600);  //Configure serial communication at 9600 bps
  randomSeed(analogRead(0));  //Initialization of the random number generator
}
/**
 * @brief Main loop of the Arduino program.
 * Processes serial input to control game flow and sends responses based on the game state.
 */
void loop() {
  if (Serial.available() > 0) {

    String receivedMessage = Serial.readString();  //Read data from the serial port
    parseJSON(receivedMessage);

    if (connect_arduino == "0"){
      connect_arduino = "1";
    }


    if (game_started == "1") {
    if (game_mode == "mva") {
        if (ai_strategy == "rand") {
            makeRandomMove();
        } else if (ai_strategy == "win") {
            stratMove("o", true);
        }
    } else if (game_mode == "ava") {
        if (ai_strategy == "rand") {
            avaMove();
        } else if (ai_strategy == "win") {
            if (next_turn == "o") {
                stratMove("o", false);
            } else {
                stratMove("x", false);
            }
        }
    }
}

    if (game_started == "1" && game_mode == "mvm") { check_mvm(); }


    String output = buildJSON();
    output.replace("\n", ""); // Заміна /n на пустий рядок
    Serial.print(output);

  }
}

/**
 * @brief Checks if the specified player has won on the current board state.
 * @param board 3x3 game board.
 * @param player The player ("x" or "o") to check for a winning condition.
 * @return True if the player has won; otherwise, false.
 */
bool checkBoardWin(String board[3][3], String player) {
  //Check lines (contours)
  for (int i = 0; i < 3; i++) {
    if (board[i][0] == player && board[i][1] == player && board[i][2] == player) {
      return true;
    }
  }

  //Check columns (vertical)
  for (int i = 0; i < 3; i++) {
    if (board[0][i] == player && board[1][i] == player && board[2][i] == player) {
      return true;
    }
  }

  //Check diagonals
  if (board[0][0] == player && board[1][1] == player && board[2][2] == player) {
    return true;
  }
  if (board[0][2] == player && board[1][1] == player && board[2][0] == player) {
    return true;
  }

  return false;
}
/**
 * @brief Checks if all fields on the board are occupied.
 * @param board 3x3 game board.
 * @return True if all fields are occupied; otherwise, false.
 */
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
/**
 * @brief Checks for a winner or if the board is fully occupied.
 * @return True if there is a winner or a draw; otherwise, false.
 */
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


/**
 * @brief AI move for AI vs AI mode in "random" strategy.
 */
void avaMove() {
  int emptyCells[9];
  int emptyCount = 0;
  int x_cells = 0;
  int o_cells = 0;

  if (checkForWinner()) { return; }

  //We pass through the field and find all the empty cells
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

  //If there are empty cells, choose a random
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
/**
 * @brief Checks and manages moves in "man vs man" mode.
 */
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
/**
 * @brief Executes a random move for the AI in "man vs AI" mode.
 */
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


/**
 * @brief Checks for a winning move and makes it if possible.
 * @param player The player making the move ("x" or "o").
 * @param currentPlayer The current player attempting a strategic move.
 * @return True if a winning move is made; otherwise, false.
 */
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
/**
 * @brief Executes a strategic move for the AI based on the current strategy.
 * @param currentPlayer The AI player making the move ("x" or "o").
 * @param randomPlayer Whether to add randomness to the initial move.
 */
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


/**
 * @brief Builds a JSON-like string representing the current game state.
 * @return A JSON-like string containing the current game state variables.
 */
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
/**
 * @brief Parses JSON-like input string to set global variables.
 * @param jsonInput The JSON string to parse.
 * @details Assumes a specific format for the JSON input string, updates global variables based on parsed values.
 */
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


