/**
 * @file main.cpp
 * @brief Main entry point for the TicTacToe application.
 *
 * This file contains the main function, which initializes the application,
 * sets the main window title, and starts the event loop.
 */
#include "mainwindow.h"

#include <QApplication>
/**
 * @brief The main function that starts the TicTacToe application.
 *
 * This function initializes the QApplication object, sets up the MainWindow,
 * assigns a title to the window, and executes the main event loop.
 *
 * @param argc Argument count for command line arguments.
 * @param argv Argument vector for command line arguments.
 * @return Application execution result.
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("TicTacToe");
    w.show();
    return a.exec();
}
