#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>

HANDLE hSerial;
QString portArduino;

QString connect_arduino, game_started, game_mode, ai_strategy, message, next_turn;
QString board[3][3];


void resetValues() {
    // connect_arduino = "0";
    game_started = "0";
    // game_mode = "mva";
    // ai_strategy = "rand";
    message = "";

    // Заповнюємо масив board значенням "-"
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            board[i][j] = "-";
        }
    }
}


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
    QString JSONData = buildJSON();  // Викликаємо функцію, яка формує JSON
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
    QString JSONData = in.readAll();  // Читаємо весь файл у строку
    file.close();

    parseJSON(JSONData);  // Парсимо JSON та відновлюємо стан гри
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

void MainWindow::updateButtonIcon(QPushButton *button, const QString &value){
    if (value == "x") {
        button->setIcon(QIcon(":/resources/assets/x2c.png"));  // Іконка хрестика
    } else if (value == "o") {
        button->setIcon(QIcon(":/resources/assets/o2c.png"));  // Іконка круга
    } else {
        button->setIcon(QIcon());  // Очищаємо іконку, якщо кнопка порожня
    }
    // button->setEnabled(false);  // Робимо кнопку недоступною для натискання
}


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

            QCoreApplication::processEvents(); // Дозволяємо Qt оновити інтерфейс

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


void MainWindow::loadComPorts()
{
    // Список для збереження імен портів
    QStringList comPortList;

    // Буфер для збереження імен пристроїв
    char lpTargetPath[5000]; // Буфер для збереження шляху до пристрою
    DWORD result;             // Результат виклику функції

    for (int i = 1; i <= 255; ++i) {
        // Формуємо ім'я порту: COM1, COM2, ..., COM255
        QString portName = QString("COM%1").arg(i);

        // Отримуємо інформацію про порт
        result = QueryDosDeviceA(portName.toStdString().c_str(), lpTargetPath, 5000);

        // Якщо порт існує, додаємо його до списку
        if (result != 0) {
            comPortList.append(portName);
        }
    }

    // Додаємо отримані порти в comboBox
    ui->comboBoxPorts->addItems(comPortList);
}

// Слот, який викликається при зміні вибраного порту
void MainWindow::onComboBoxPortChanged(const QString &port)
{
    ui->labelPort->setText(QString("Trying to connect via %1").arg(port));
    ui->labelPort->setStyleSheet("QLabel { color : lightblue; }");
    QCoreApplication::processEvents(); // Дозволяємо Qt оновити інтерфейс

    if (connectArduino(port)) {
        // Якщо відповідь від Arduino коректна
        ui->labelPort->setText("Arduino is connected");
        ui->labelPort->setStyleSheet("QLabel { color : lightgreen; }");

        // Активуємо кнопки
        ui->newButton->setEnabled(true);
        ui->loadButton->setEnabled(true);
        ui->saveButton->setEnabled(true);

        portArduino = port;
    }
    else {
        // Якщо помилка при зв'язку з Arduino
        ui->labelPort->setText(QString("Failed to connect via %1").arg(port));
        ui->labelPort->setStyleSheet("QLabel { color : red; }");

        // Деактивуємо кнопки
        ui->newButton->setEnabled(false);
        ui->loadButton->setEnabled(false);
        ui->saveButton->setEnabled(false);
    }
}



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
        return "";  // Помилка при відправці даних
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



void MainWindow::on_radioButton_mai_clicked(){
    game_mode = "mva";
    // qDebug() << "game_mode:" << game_mode;
}
void MainWindow::on_radioButton_mvm_clicked(){
    game_mode = "mvm";
    // qDebug() << "game_mode:" << game_mode;
}
void MainWindow::on_radioButton_ava_clicked(){
    game_mode = "ava";
    // qDebug() << "game_mode:" << game_mode;
}

void MainWindow::on_radioButton_rm_clicked(){
    ai_strategy = "rand";
    // qDebug() << "ai_strategy:" << ai_strategy;
}
void MainWindow::on_radioButton_ws_clicked(){
    ai_strategy = "win";
    // qDebug() << "ai_strategy:" << ai_strategy;
}

