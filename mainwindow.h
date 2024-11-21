#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(int clientId, QWidget *parent = nullptr);

private slots:
    void credit();
    void debit();
    void transfer();
    void viewInfo();
    void viewFullInfo();
    void majInfo();
    void editCin();
    void editCodeSecret();
    void logout();
    void showCredits();

private:
    int clientId;
    QLabel *welcomeLabel;
    QLabel *balanceLabel;

    bool verifyCodeSecret();

    void centerWindow();
};

#endif // MAINWINDOW_H
