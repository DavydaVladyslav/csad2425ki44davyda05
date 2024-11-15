#include <QApplication>
#include <gtest/gtest.h>
#include <qjsonobject.h>
#include "mainwindow.h"

class MainWindowTest : public ::testing::Test {
protected:
    MainWindow *mainWindow; // Pointer to MainWindow instance
    QApplication *app;      // Pointer to QApplication instance

    void SetUp() override {
        // Create a QApplication instance
        int argc = 0;
        char *argv[] = { nullptr };
        app = new QApplication(argc, argv);

        // Create an instance of MainWindow
        mainWindow = new MainWindow();
    }

    void TearDown() override {
        // Clean up
        delete mainWindow;
        delete app;  // Delete QApplication
    }
};


TEST_F(MainWindowTest, GetTagValue_ValidTag) {
    QJsonObject json;
    json["tag"] = "Hello";
    QString value = mainWindow->getJsonValue(json, "tag");
    EXPECT_EQ(value, "Hello");
}

TEST_F(MainWindowTest, GetTagValue_ValidTag_2) {
    QJsonObject json;
    json["con"] = "1";
    json["gs"] = "1";
    json["gm"] = "mva";
    json["ais"] = "rand";
    json["msg"] = "Hello";
    json["nt"] = "x";

    QString expected = "1";
    QString actual = mainWindow->getJsonValue(json, "con");
    EXPECT_EQ(actual, expected);
}

TEST_F(MainWindowTest, GetTagValue_MissingKey) {
    QJsonObject json;
    json["tag"] = "Hello";
    QString value = mainWindow->getJsonValue(json, "missing");
    EXPECT_EQ(value, ""); // Очікується, що повертається пустий рядок
}

TEST_F(MainWindowTest, GetTagValue_InvalidKey) {
    QJsonObject json;
    json["con"] = "1";
    json["gs"] = "1";
    json["gm"] = "mva";
    json["ais"] = "rand";
    json["msg"] = "Hello";
    json["nt"] = "x";

    QString actual = mainWindow->getJsonValue(json, "invalid");
    EXPECT_EQ(actual, ""); // Очікується, що повертається пустий рядок
}


// Тест для buildJSON
TEST_F(MainWindowTest, BuildJson_ProducesCorrectOutput) {
    // Ініціалізація полів
    mainWindow->game_started = "1";
    mainWindow->connect_arduino = "1";
    mainWindow->game_mode = "mva";
    mainWindow->ai_strategy = "rand";
    mainWindow->message = "Game in progress";
    mainWindow->next_turn = "x";
    mainWindow->board[0][0] = "x";
    mainWindow->board[1][1] = "o";
    mainWindow->board[2][2] = "-";

    // Очікуваний JSON (якщо є проблема з форматуванням або пробілами, ви можете перевірити тут)
    QString expectedJson = R"({"game_started": "1","connect_arduino": "1","game_mode": "mva","ai_strategy": "rand","message": "Game in progress","next_turn": "x","board": [["x", "", ""],["", "o", ""],["", "", "-"]]})";

    // Виклик методу для отримання фактичного JSON
    QString actualJson = mainWindow->buildJSON();

    // Виведення для перевірки
    qDebug() << "Expected JSON: " << expectedJson;
    qDebug() << "Actual JSON: " << actualJson;

}



// Тест для parseJSON
TEST_F(MainWindowTest, ParseJSON_CorrectlyUpdatesValues) {
    QString json = R"({"con": "0","gs": "1","gm": "ava","ais": "win","msg": "test_msg","nt": "o","brd": {"c11": "o","c12": "","c13": "","c21": "","c22": "o","c23": "","c31": "","c32": "","c33": "-"}})";

    mainWindow->parseJSON(json);

    EXPECT_EQ(mainWindow->connect_arduino, "0");
    EXPECT_EQ(mainWindow->game_started, "1");
    EXPECT_EQ(mainWindow->game_mode, "ava");
    EXPECT_EQ(mainWindow->ai_strategy, "win");
    EXPECT_EQ(mainWindow->message, "test_msg");
    EXPECT_EQ(mainWindow->next_turn, "o");
    EXPECT_EQ(mainWindow->board[0][0], "o");
    EXPECT_EQ(mainWindow->board[1][1], "o");
    EXPECT_EQ(mainWindow->board[2][2], "-");
}

TEST_F(MainWindowTest, ParseJSON_InvalidJson_ReturnsEarly) {
    QString invalidJson = R"({"con": "0","gs": "1","gm": "ava","ais": "win","msg": "test_msg","nt": "o","brd": {"c11": "o","c12": ""})";

    mainWindow->parseJSON(invalidJson);

    // Очікується, що значення не зміняться, бо JSON не правильний
    EXPECT_EQ(mainWindow->connect_arduino, "");
    EXPECT_EQ(mainWindow->game_started, "");
    EXPECT_EQ(mainWindow->game_mode, "");
    EXPECT_EQ(mainWindow->ai_strategy, "");
    EXPECT_EQ(mainWindow->message, "");
    EXPECT_EQ(mainWindow->next_turn, "");
    EXPECT_EQ(mainWindow->board[0][0], "");
}

TEST_F(MainWindowTest, ParseJSON_EmptyJson_ReturnsEarly) {
    QString emptyJson = "{}";  // Порожній JSON

    mainWindow->parseJSON(emptyJson);

    // Перевіряється, що значення не змінюються, бо JSON порожній
    EXPECT_EQ(mainWindow->connect_arduino, "");
    EXPECT_EQ(mainWindow->game_started, "");
    EXPECT_EQ(mainWindow->game_mode, "");
    EXPECT_EQ(mainWindow->ai_strategy, "");
    EXPECT_EQ(mainWindow->message, "");
    EXPECT_EQ(mainWindow->next_turn, "");
    EXPECT_EQ(mainWindow->board[0][0], "");
}



// // Тест для resetValues
TEST_F(MainWindowTest, ResetValues_SetsToInitialState) {
    mainWindow->game_started = "1";
    mainWindow->message = "msg here";
    mainWindow->resetValues();

    EXPECT_EQ(mainWindow->game_started, "0");
    EXPECT_EQ(mainWindow->message, "");
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            EXPECT_EQ(mainWindow->board[i][j], "-");
        }
    }
}




