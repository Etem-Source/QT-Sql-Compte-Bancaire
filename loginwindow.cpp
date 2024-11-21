#include "loginwindow.h"
#include "mainwindow.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>
#include <QScreen>
#include <QApplication>

LoginWindow::LoginWindow(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("Connexion");
    resize(400, 300);
    setFixedSize(400, 300);

    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel("Connexion à votre compte", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    cinEdit = new QLineEdit(this);
    cinEdit->setPlaceholderText("CIN");
    layout->addWidget(cinEdit);

    codeSecretEdit = new QLineEdit(this);
    codeSecretEdit->setPlaceholderText("Code Secret");
    codeSecretEdit->setEchoMode(QLineEdit::Password);
    layout->addWidget(codeSecretEdit);

    showPasswordCheckBox = new QCheckBox("Afficher le mot de passe", this);
    layout->addWidget(showPasswordCheckBox);
    connect(showPasswordCheckBox, &QCheckBox::stateChanged, this, &LoginWindow::togglePasswordVisibility);

    loginButton = new QPushButton("Connexion", this);
    layout->addWidget(loginButton);
    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::login);

    setLayout(layout);

    // Centrer la fenêtre
    centerWindow();
}

void LoginWindow::centerWindow()
{
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
}

void LoginWindow::togglePasswordVisibility()
{
    if (showPasswordCheckBox->isChecked()) {
        codeSecretEdit->setEchoMode(QLineEdit::Normal);
    } else {
        codeSecretEdit->setEchoMode(QLineEdit::Password);
    }
}

void LoginWindow::login()
{
    QString cin = cinEdit->text();
    QString codeSecret = codeSecretEdit->text();

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("QTBDD");
    db.setUserName("qtsql");
    db.setPassword("Qtsql?99");

    if (!db.open()) {
        QMessageBox::critical(this, "Erreur", "Connexion à la base de données échouée: " + db.lastError().text());
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT cl.id, cl.nom, cl.prenom FROM clients cl JOIN comptes c ON cl.compte_id = c.id WHERE cl.cin = :cin AND c.code_secret = :code_secret");
    query.bindValue(":cin", cin);
    query.bindValue(":code_secret", codeSecret);

    if (!query.exec()) {
        qDebug() << "Erreur d'exécution de la requête: " << query.lastError().text();
        QMessageBox::critical(this, "Erreur", "Erreur d'exécution de la requête: " + query.lastError().text());
        return;
    }

    if (query.next()) {
        int clientId = query.value(0).toInt();
        QString nom = query.value(1).toString();
        QString prenom = query.value(2).toString();

        MainWindow *mainWindow = new MainWindow(clientId);
        mainWindow->show();
        this->close();
    } else {
        loginButton->setStyleSheet("QPushButton { background-color: #ff0000; }"); // Rougir le bouton
        QMessageBox::warning(this, "Erreur", "CIN ou Code Secret incorrect");
    }
}
