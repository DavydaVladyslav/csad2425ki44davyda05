/**
 @file mainwindow.cpp
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>

/**
 * @brief A handle to the serial port for communication with Arduino.
 */
HANDLE hSerial;
/**
 * @brief A String variable to store the port information for Arduino.
 */
QString portArduino;

/**
 * @brief Variables used in the main window of the client application.
*
*
* @var connect_arduino Indicates the connection status with the Arduino device.
* @var game_started Indicates whether the game has started.
* @var game_mode Represents the current game mode.
* @var ai_strategy Represents the AI strategy being used.
* @var message Stores messages to be displayed or processed.
* @var next_turn Indicates whose turn is next in then the game.
*/
QString connect_arduino, game_started, game_mode, ai_strategy, message, next_turn;
QString board[3][3];

/**
* @brief Resets the game values to their initial state.
* This function performs the following actions:
*  - Sets the game_started flag to "0".
*  - Clears the message string.
*  -Initializes the board array with the value "_"
* Note: Some variables such as connect_arduino, game_mode, and ai_strategy are commented out and not reset by this function.
*/
void resetValues() {
    // connect_arduino = "0";
    game_started = "0";
    // game_mode = "mva";
    // ai_strategy = "rand";
    message = "";

    // Fill the board array with the value "-"
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            board[i][j] = "-";
        }
    }
}

/**
 * @brief creates a JSON message to pass the game state.
 *
 * Generates a JSON object with the current values of variables, such as
 * @c connect_arduino, @c game_started, @c game_mode, @c ai_strategy,
 * @C message, @C next_turn, and the current state of the game board.
 *
 * @return JSON-string representing the game state.
 */
QString buildJSON() {
    QJsonObject json;
    json["con"] = connect_arduino;
    json["gs"] = game_started;
    json["gm"] = game_mode;
    json["ais"] = ai_strategy;
    json["msg"] = message;
    json["nt"] = next_turn;

    QJsonObject boardJson;
    boardJson["c11"] = board[0][0];
    boardJson["c12"] = board[0][1];
    boardJson["c13"] = board[0][2];
    boardJson["c21"] = board[1][0];
    boardJson["c22"] = board[1][1];
    boardJson["c23"] = board[1][2];
    boardJson["c31"] = board[2][0];
    boardJson["c32"] = board[2][1];
    boardJson["c33"] = board[2][2];

    json["brd"] = boardJson;

    QJsonDocument doc(json);
    return doc.toJson(QJsonDocument::Indented);
}
/**
 * @brief returns the key value from the JSON object.
 *
 * @Param json JSON-object for getting the value.
 * @param key The Key whose value you want to get.
 * @return key value as a string.
 */
QString getJsonValue(const QJsonObject& json, const QString& key) {
    return json.value(key).toString();
}

void MainWindow::parseJSON(QString input) {
    QJsonDocument doc = QJsonDocument::fromJson(input.toUtf8());
    if (!doc.isObject()) return;

    QJsonObject json = doc.object();

    connect_arduino = getJsonValue(json, "con");
    game_started = getJsonValue(json, "gs");
    game_mode = getJsonValue(json, "gm");
    ai_strategy = getJsonValue(json, "ais");
    message = getJsonValue(json, "msg");
    ui->label_arduino_msg->setText(message);
    next_turn = getJsonValue(json, "nt");

    QJsonObject boardJson = json["brd"].toObject();
    board[0][0] = getJsonValue(boardJson, "c11");
    board[0][1] = getJsonValue(boardJson, "c12");
    board[0][2] = getJsonValue(boardJson, "c13");
    board[1][0] = getJsonValue(boardJson, "c21");
    board[1][1] = getJsonValue(boardJson, "c22");
    board[1][2] = getJsonValue(boardJson, "c23");
    board[2][0] = getJsonValue(boardJson, "c31");
    board[2][1] = getJsonValue(boardJson, "c32");
    board[2][2] = getJsonValue(boardJson, "c33");
}



void MainWindow::saveGameState(const QString& filePath) {
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        ui->label_pc_msg->setText("cant write file for write");
        return;
    }

    QTextStream out(&file);
    QString JSONData = buildJSON();  //Call the function that forms the JSON
    out << JSONData;

    file.close();

    ui->label_pc_msg->setText("Game Saved");
}

void MainWindow::loadGameState(const QString& filePath) {
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        ui->label_pc_msg->setText("cant open file for read");
        return;
    }

    QTextStream in(&file);
    QString JSONData = in.readAll();  // We read the entire file in time
    file.close();

    parseJSON(JSONData);  // Parsim JSON and restore the state of the game
    ui->label_pc_msg->setText("Load Complete");
}



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    loadComPorts();

    connect(ui->comboBoxPorts, SIGNAL(currentTextChanged(const QString &)),
            this, SLOT(onComboBoxPortChanged(const QString &)));


    QString JSONData = buildJSON();
    updateGameBoard();


}

MainWindow::~MainWindow()
{
    delete ui;
}


/**
 * @brief updates the game board interface according to the current state of the game.
 */
void MainWindow::updateGameBoard(){
    updateButtonIcon(ui->button_11, board[0][0]);
    updateButtonIcon(ui->button_12, board[0][1]);
    updateButtonIcon(ui->button_13, board[0][2]);
    updateButtonIcon(ui->button_21, board[1][0]);
    updateButtonIcon(ui->button_22, board[1][1]);
    updateButtonIcon(ui->button_23, board[1][2]);
    updateButtonIcon(ui->button_31, board[2][0]);
    updateButtonIcon(ui->button_32, board[2][1]);
    updateButtonIcon(ui->button_33, board[2][2]);
}
/**
 * @brief updates the button icon according to the value on the game board.
 *
 * @param button pointer to the button to update.
 * @param value the value on the game board that defines the icon.
 */
void MainWindow::updateButtonIcon(QPushButton *button, const QString &value){
    if (value == "x") {
        button->setIcon(QIcon(":/resources/assets/x2c.png"));
    } else if (value == "o") {
        button->setIcon(QIcon(":/resources/assets/o2c.png"));
    } else {
        button->setIcon(QIcon());  //Clear the icon if the button is empty
    }
    // button->setEnabled(false);  //Make the button unavailable for pressing
}

/**
 * @brief initiates a new game with an update of all initial values.
 *
 * Starts the game, displays notifications, and allows the interface to be updated.
 */
void MainWindow::on_newButton_clicked()
{
    resetValues();
    game_started = "1";

    if (game_mode == "ava")
    {
        ui->newButton->setEnabled(false);
        ui->loadButton->setEnabled(false);
        ui->saveButton->setEnabled(false);

        ui->label_pc_msg->setText("AI vs AI");

        while (game_started == "1")
        {
            // QString receivedJSON = sendArduino();
            // parseJSON(receivedJSON);
            parseJSON(sendArduino());

            updateGameBoard();

            QCoreApplication::processEvents(); //Allow Qt to update the interface

            //Sleep(150);
        }

        ui->newButton->setEnabled(true);
        ui->loadButton->setEnabled(true);
        ui->saveButton->setEnabled(true);
    }
    else
    {
        // QString receivedJSON = sendArduino();
        // parseJSON(receivedJSON);
        parseJSON(sendArduino());

        updateGameBoard();
    }


}
void MainWindow::on_loadButton_clicked()
{
    loadGameState("game_state.JSON");
    updateGameBoard();

    if (game_mode == "mva") ui->radioButton_mai->setChecked(true);
    if (game_mode == "mvm") ui->radioButton_mvm->setChecked(true);
    if (game_mode == "ava") ui->radioButton_ava->setChecked(true);

    if (ai_strategy == "rand") ui->radioButton_rm->setChecked(true);
    if (ai_strategy == "win") ui->radioButton_ws->setChecked(true);

}
void MainWindow::on_saveButton_clicked(){
    saveGameState("game_state.JSON");
}

/**
 * @brief loads a list of available COM ports and adds them to the comboBox.
 */
void MainWindow::loadComPorts()
{
    //List for storing port names
    QStringList comPortList;

    //Buffer for storing device names
    char lpTargetPath[5000]; //Buffer to save the device path
    DWORD result;             //Result of function call

    for (int i = 1; i <= 255; ++i) {
        //Form the port name: COM1, COM2,..., COM255
        QString portName = QString("COM%1").arg(i);

        //We get information about the port
        result = QueryDosDeviceA(portName.toStdString().c_str(), lpTargetPath, 5000);

        //If a port exists, add it to the list
        if (result != 0) {
            comPortList.append(portName);
        }
    }

    //Add the received ports to comboBox
    ui->comboBoxPorts->addItems(comPortList);
}

//Slot called when the selected port is changed
void MainWindow::onComboBoxPortChanged(const QString &port)
{
    ui->labelPort->setText(QString("Trying to connect via %1").arg(port));
    ui->labelPort->setStyleSheet("QLabel { color : lightblue; }");
    QCoreApplication::processEvents(); // Дозволяємо Qt оновити інтерфейс

    if (connectArduino(port)) {
        //If the response from Arduino is correct
        ui->labelPort->setText("Arduino is connected");
        ui->labelPort->setStyleSheet("QLabel { color : lightgreen; }");

        //Activate the buttons
        ui->newButton->setEnabled(true);
        ui->loadButton->setEnabled(true);
        ui->saveButton->setEnabled(true);

        portArduino = port;
    }
    else {
        //If the communication with Arduino fails
        ui->labelPort->setText(QString("Failed to connect via %1").arg(port));
        ui->labelPort->setStyleSheet("QLabel { color : red; }");

        //Deactivate the buttons
        ui->newButton->setEnabled(false);
        ui->loadButton->setEnabled(false);
        ui->saveButton->setEnabled(false);
    }
}

/**
 * @brief sends JSON data to the Arduino and receives a response.
 *
 * @return string with JSON response from Arduino.
 */
//Function for sending data to Arduino
QString MainWindow::sendArduino() {

    QString JSONData = buildJSON();

    QString JSONData_trim = JSONData.remove(QChar(' '));
    JSONData_trim.remove(QChar('\n'));
    JSONData_trim.remove(QChar('\t'));

    std::string JSONString = JSONData_trim.toStdString();
    const char *dataToSend = JSONString.c_str();

    // qDebug() << "Sended  :" << dataToSend;

    DWORD bytesWritten;
    if (!WriteFile(hSerial, dataToSend, strlen(dataToSend), &bytesWritten, nullptr)) {
        CloseHandle(hSerial);
        return "";  //Error sending data
    }

    // Чекаємо на відповідь
    char incomingData[256] = {0};
    DWORD bytesRead;
    if (!ReadFile(hSerial, incomingData, sizeof(incomingData), &bytesRead, nullptr)) {
        CloseHandle(hSerial);
        return "";  // Помилка при отриманні даних
    }

    // Закриваємо порт
    // CloseHandle(hSerial);

    // Перевіряємо, чи отримали правильну відповідь
    QString response(incomingData);
    // qDebug() << "Response:" << response;

    return response;
}

/**
 * @brief connects the Arduino to the selected COM port.
 *
 * @param portName name of the port to connect to.
 * @return true if the connection is successful; false if an error occurs.
 */
// Функція для тестування зв'язку з Arduino
bool MainWindow::connectArduino(const QString &portName)
{
    CloseHandle(hSerial);

    // Відкриваємо COM-порт
    hSerial = CreateFileA(portName.toStdString().c_str(),
                          GENERIC_READ | GENERIC_WRITE,
                          0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (hSerial == INVALID_HANDLE_VALUE) {
        return false;  // Не вдалося відкрити порт
    }

    // Налаштування порту
    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        CloseHandle(hSerial);
        return false;  // Помилка отримання стану порту
    }

    dcbSerialParams.BaudRate = CBR_9600;  // Налаштування швидкості
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        CloseHandle(hSerial);
        return false;  // Помилка налаштування порту
    }

    // Налаштування тайм-аутів
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(hSerial, &timeouts)) {
        CloseHandle(hSerial);
        return false;  // Помилка налаштування тайм-аутів
    }

    Sleep(2000);

    resetValues();
    connect_arduino = "0";
    QString receivedJSON = sendArduino();

    // QString conValue = getTagValue(receivedJSON, "con");
    // qDebug() << "conValue:" << conValue;

    parseJSON(receivedJSON);

    return connect_arduino.trimmed() == "1";  // Порівнюємо з відповіддю Arduino
}

/**
 * @brief Додає хід гравця на ігровій дошці у вказаній позиції.
 *
 * @param row Рядок дошки для ходу.
 * @param col Стовпець дошки для ходу.
 */
void MainWindow::add_player_turn(int row, int col)
{
    // connect_arduino, game_started, game_mode, ai_strategy, message, next_turn;
    if (connect_arduino != "1" || game_started != "1" || game_mode == "ava") {
        return;
    }


    if (board[row][col] != "x" && board[row][col] != "o")
    {
        if (next_turn == "x") { board[row][col] = "x"; }
        else { board[row][col] = "o"; }

        ui->label_pc_msg->setText("");
    }
    else {
        ui->label_pc_msg->setText("Select empty cell");
        return;
    }

    updateGameBoard();
    QCoreApplication::processEvents(); // Дозволяємо Qt оновити інтерфейс

    QString receivedJSON = sendArduino();
    parseJSON(receivedJSON);

    updateGameBoard();
}

void MainWindow::on_button_11_clicked(){ add_player_turn(0, 0); }
void MainWindow::on_button_12_clicked(){ add_player_turn(0, 1); }
void MainWindow::on_button_13_clicked(){ add_player_turn(0, 2); }
void MainWindow::on_button_21_clicked(){ add_player_turn(1, 0); }
void MainWindow::on_button_22_clicked(){ add_player_turn(1, 1); }
void MainWindow::on_button_23_clicked(){ add_player_turn(1, 2); }
void MainWindow::on_button_31_clicked(){ add_player_turn(2, 0); }
void MainWindow::on_button_32_clicked(){ add_player_turn(2, 1); }
void MainWindow::on_button_33_clicked(){ add_player_turn(2, 2); }


/**
 * @brief Sets the game mode to "Player vs AI".
 *
 * This function changes the value of the game_mode variable to "mva",
 * indicating that the game mode is set to Player versus AI.
 */
void MainWindow::on_radioButton_mai_clicked(){
    game_mode = "mva";
    // qDebug() << "game_mode:" << game_mode;
}
/**
 * @brief Sets the game mode to "AI vs AI".
 *
 * This function changes the value of the game_mode variable to "mvm",
 * indicating that the game mode is set to AI versus AI.
 */
void MainWindow::on_radioButton_mvm_clicked(){
    game_mode = "mvm";
    // qDebug() << "game_mode:" << game_mode;
}
/**
 * @brief Sets the game mode to "Player vs Player".
 *
 * This function changes the value of the game_mode variable to "ava",
 * indicating that the game mode is set to Player versus Player.
 */
void MainWindow::on_radioButton_ava_clicked(){
    game_mode = "ava";
    // qDebug() << "game_mode:" << game_mode;
}
/**
 * @brief Sets the AI strategy to a random choice.
 *
 * This function changes the value of the ai_strategy variable to "rand",
 * meaning that the AI will select moves randomly.
 */
void MainWindow::on_radioButton_rm_clicked(){
    ai_strategy = "rand";
    // qDebug() << "ai_strategy:" << ai_strategy;
}
/**
 * @brief Sets the AI strategy to a strategic choice.
 *
 * This function changes the value of the ai_strategy variable to "win",
 * meaning that the AI will attempt to choose the most advantageous move.
 */
void MainWindow::on_radioButton_ws_clicked(){
    ai_strategy = "win";
    // qDebug() << "ai_strategy:" << ai_strategy;
}

