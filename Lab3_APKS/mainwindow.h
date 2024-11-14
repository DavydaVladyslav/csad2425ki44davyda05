/**
 * @file mainwindow.h
 * @brief Declaration of the MainWindow class for GUI management and Arduino integration.
 *
 * This file contains the declaration of the MainWindow class, responsible for managing
 * the graphical user interface (GUI), communication with an Arduino device, and game logic.
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <windows.h>
#include <QComboBox>
#include <QStringList>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE
/**
 * @class MainWindow
 * @brief The MainWindow class handles the main application window, including GUI components and game functions.
 *
 * MainWindow manages the user interface, connects to an Arduino device, sends and receives data,
 * and maintains the game board state.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a MainWindow object with an optional parent widget.
     * @param parent Optional parent widget.
     */
    MainWindow(QWidget *parent = nullptr);
    /**
     * @brief Destroys the MainWindow object.
     */
    ~MainWindow();

    void loadComPorts();
    /**
     * @brief Connects to an Arduino device using the specified COM port.
     * @param port The name of the COM port to connect to.
     * @return True if the connection is successful, otherwise false.
     */
    bool connectArduino(const QString &port); // Функція для тестування Arduino
    QString sendArduino();

    void updateGameBoard();
    void updateButtonIcon(QPushButton *button, const QString &value);

    void parseJSON(QString input);

    void add_player_turn(int row, int col);

    void saveGameState(const QString& filePath);
    void loadGameState(const QString& filePath);
    // QString getTagValue(const QString& response, const QString& tagName);
private slots:
    void on_newButton_clicked();
    void on_loadButton_clicked();
    void on_saveButton_clicked();
    /**
     * @brief Slot triggered when the COM port selection changes in the combo box.
     * @param port The selected COM port.
     */
    void onComboBoxPortChanged(const QString &port);

    void on_button_11_clicked();
    void on_button_12_clicked();
    void on_button_13_clicked();
    void on_button_21_clicked();
    void on_button_22_clicked();
    void on_button_23_clicked();
    void on_button_31_clicked();
    void on_button_32_clicked();
    void on_button_33_clicked();

    void on_radioButton_mai_clicked();
    void on_radioButton_mvm_clicked();
    void on_radioButton_ava_clicked();

    void on_radioButton_rm_clicked();
    void on_radioButton_ws_clicked();


private:
    Ui::MainWindow *ui;


};
#endif // MAINWINDOW_H
