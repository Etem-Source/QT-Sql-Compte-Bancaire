#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>

class LoginWindow : public QWidget
{
    Q_OBJECT

public:
    LoginWindow(QWidget *parent = nullptr);

private slots:
    void login();
    void togglePasswordVisibility();

private:
    QLineEdit *cinEdit;
    QLineEdit *codeSecretEdit;
    QCheckBox *showPasswordCheckBox;
    QPushButton *loginButton;

    void centerWindow();
};

#endif // LOGINWINDOW_H
