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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_newButton_clicked();
    void on_loadButton_clicked();
    void on_saveButton_clicked();

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

    void loadComPorts();
    bool connectArduino(const QString &port); // Функція для тестування Arduino
    QString sendArduino();

    void updateGameBoard();
    void updateButtonIcon(QPushButton *button, const QString &value);

    void parseJSON(QString input);

    void add_player_turn(int row, int col);

    void saveGameState(const QString& filePath);
    void loadGameState(const QString& filePath);
    // QString getTagValue(const QString& response, const QString& tagName);
};
#endif // MAINWINDOW_H
