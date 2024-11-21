#include "mainwindow.h"
#include "loginwindow.h"  // pour pouvoir revenir à la page de connexion
#include <QMenuBar>
#include <QMessageBox>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QDesktopServices>
#include <QUrl>
#include <QApplication>
#include <QScreen>

MainWindow::MainWindow(int clientId, QWidget *parent)
    : QMainWindow(parent), clientId(clientId)
{
    setWindowTitle("Dashboard");
    resize(400, 300);
    setFixedSize(400, 300);

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QWidget *infoWidget = new QWidget(centralWidget);
    QVBoxLayout *infoLayout = new QVBoxLayout(infoWidget);

    welcomeLabel = new QLabel(infoWidget);
    balanceLabel = new QLabel(infoWidget);
    welcomeLabel->setAlignment(Qt::AlignCenter);
    balanceLabel->setAlignment(Qt::AlignCenter);
    infoLayout->addWidget(welcomeLabel);
    infoLayout->addWidget(balanceLabel);
    infoWidget->setLayout(infoLayout);

    mainLayout->addStretch(); // Ajout de stretch pour centrer verticalement
    mainLayout->addWidget(infoWidget, 0, Qt::AlignCenter);
    mainLayout->addStretch(); // Ajout de stretch pour centrer verticalement

    setCentralWidget(centralWidget);

    QMenuBar *menuBar = new QMenuBar(this);

    QMenu *operationMenu = new QMenu("Opérations", this);
    QAction *creditAction = new QAction("Crédit", this);
    QAction *debitAction = new QAction("Débit", this);
    QAction *transferAction = new QAction("Virement", this);
    QAction *infoAction = new QAction("Consulter Solde", this);

    connect(creditAction, &QAction::triggered, this, &MainWindow::credit);
    connect(debitAction, &QAction::triggered, this, &MainWindow::debit);
    connect(transferAction, &QAction::triggered, this, &MainWindow::transfer);
    connect(infoAction, &QAction::triggered, this, &MainWindow::viewInfo);

    operationMenu->addAction(creditAction);
    operationMenu->addAction(debitAction);
    operationMenu->addAction(transferAction);
    operationMenu->addAction(infoAction);

    QMenu *accountMenu = new QMenu("Compte", this);
    QAction *viewInfoAction = new QAction("Consulter Informations", this);
    QAction *editCinAction = new QAction("Modifier CIN", this);
    QAction *editCodeSecretAction = new QAction("Modifier Code Secret", this);

    connect(viewInfoAction, &QAction::triggered, this, &MainWindow::viewFullInfo);
    connect(editCinAction, &QAction::triggered, this, &MainWindow::editCin);
    connect(editCodeSecretAction, &QAction::triggered, this, &MainWindow::editCodeSecret);

    accountMenu->addAction(viewInfoAction);
    accountMenu->addAction(editCinAction);
    accountMenu->addAction(editCodeSecretAction);

    QMenu *optionsMenu = new QMenu("Options", this);
    QAction *logoutAction = new QAction("Se déconnecter", this);
    QAction *creditsAction = new QAction("Crédit", this);

    connect(logoutAction, &QAction::triggered, this, &MainWindow::logout);
    connect(creditsAction, &QAction::triggered, this, &MainWindow::showCredits);

    optionsMenu->addAction(logoutAction);
    optionsMenu->addAction(creditsAction);

    menuBar->addMenu(operationMenu);
    menuBar->addMenu(accountMenu);
    menuBar->addMenu(optionsMenu);
    setMenuBar(menuBar);

    // Afficher le message de bienvenue et le solde
    majInfo();
    // Centrer la fenêtre
    centerWindow();
}

void MainWindow::centerWindow()
{
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
}

void MainWindow::credit()
{
    bool ok;
    double amount = QInputDialog::getDouble(this, tr("Crédit"), tr("Montant à créditer:"), 0, 0, 100000, 2, &ok);
    if (ok && verifyCodeSecret()) {
        QSqlQuery query;
        query.prepare("UPDATE comptes SET solde = solde + :amount WHERE id = (SELECT compte_id FROM clients WHERE id = :client_id)");
        query.bindValue(":amount", amount);
        query.bindValue(":client_id", clientId);

        if (query.exec()) {
            QMessageBox::information(this, "Crédit", "Montant crédité avec succès.");
            majInfo(); // Mettre à jour l'affichage du solde
        } else {
            QMessageBox::warning(this, "Erreur", "Erreur lors du crédit: " + query.lastError().text());
        }
    }
}

void MainWindow::debit()
{
    bool ok;
    double amount = QInputDialog::getDouble(this, tr("Débit"), tr("Montant à débiter:"), 0, 0, 100000, 2, &ok);
    if (ok && verifyCodeSecret()) {
        QSqlQuery query;
        query.prepare("UPDATE comptes SET solde = solde - :amount WHERE id = (SELECT compte_id FROM clients WHERE id = :client_id) AND solde >= :amount - 50");
        query.bindValue(":amount", amount);
        query.bindValue(":client_id", clientId);

        if (query.exec() && query.numRowsAffected() > 0) {
            QMessageBox::information(this, "Débit", "Montant débité avec succès.");
            majInfo(); // Mettre à jour l'affichage du solde
        } else {
            QMessageBox::warning(this, "Erreur", "Erreur lors du débit: Solde insuffisant ou autre erreur.");
        }
    }
}

void MainWindow::transfer()
{
    bool ok;
    QString recipientRIB = QInputDialog::getText(this, tr("Virement"), tr("RIB du bénéficiaire:"), QLineEdit::Normal, "", &ok);
    if (ok && !recipientRIB.isEmpty()) {
        QSqlQuery infoQuery;
        infoQuery.prepare("SELECT cl.nom, cl.prenom FROM clients cl JOIN comptes c ON cl.compte_id = c.id WHERE c.rib = :rib");
        infoQuery.bindValue(":rib", recipientRIB);

        if (!infoQuery.exec() || !infoQuery.next()) {
            QMessageBox::warning(this, "Erreur", "RIB du bénéficiaire introuvable.");
            return;
        }

        QString nom = infoQuery.value(0).toString();
        QString prenom = infoQuery.value(1).toString();

        double amount = QInputDialog::getDouble(this, tr("Virement"), tr("Montant à transférer:"), 0, 0, 100000, 2, &ok);
        if (ok && verifyCodeSecret()) {
            QSqlQuery checkQuery;
            checkQuery.prepare("SELECT solde FROM comptes WHERE id = (SELECT compte_id FROM clients WHERE id = :client_id)");
            checkQuery.bindValue(":client_id", clientId);

            if (!checkQuery.exec() || !checkQuery.next()) {
                QMessageBox::warning(this, "Erreur", "Erreur lors de la vérification du solde.");
                return;
            }

            double solde = checkQuery.value(0).toDouble();

            if (solde - amount < -50) {
                QMessageBox::warning(this, "Erreur", "Solde insuffisant.");
                return;
            }

            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Confirmation", QString("Vous voulez bien transférer %1 € à %2 %3 avec le RIB %4 ?").arg(amount).arg(prenom).arg(nom).arg(recipientRIB), QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                QSqlDatabase::database().transaction();
                QSqlQuery debitQuery;
                debitQuery.prepare("UPDATE comptes SET solde = solde - :amount WHERE id = (SELECT compte_id FROM clients WHERE id = :client_id) AND solde >= :amount - 50");
                debitQuery.bindValue(":amount", amount);
                debitQuery.bindValue(":client_id", clientId);

                if (debitQuery.exec() && debitQuery.numRowsAffected() > 0) {
                    QSqlQuery creditQuery;
                    creditQuery.prepare("UPDATE comptes SET solde = solde + :amount WHERE rib = :rib");
                    creditQuery.bindValue(":amount", amount);
                    creditQuery.bindValue(":rib", recipientRIB);

                    if (creditQuery.exec() && creditQuery.numRowsAffected() > 0) {
                        QSqlDatabase::database().commit();
                        QMessageBox::information(this, "Virement", "Virement effectué avec succès.");
                        majInfo(); // Mettre à jour l'affichage du solde
                    } else {
                        QSqlDatabase::database().rollback();
                        QMessageBox::warning(this, "Erreur", "Erreur lors du crédit au bénéficiaire: " + creditQuery.lastError().text());
                    }
                } else {
                    QSqlDatabase::database().rollback();
                    QMessageBox::warning(this, "Erreur", "Erreur lors du débit: Solde insuffisant ou autre erreur.");
                }
            }
        }
    }
}

bool MainWindow::verifyCodeSecret()
{
    bool ok;
    QString codeSecret = QInputDialog::getText(this, tr("Code Secret"), tr("Entrez votre code secret:"), QLineEdit::Password, "", &ok);
    if (ok) {
        QSqlQuery query;
        query.prepare("SELECT solde FROM comptes WHERE id = (SELECT compte_id FROM clients WHERE id = :client_id) AND code_secret = :code_secret");
        query.bindValue(":client_id", clientId);
        query.bindValue(":code_secret", codeSecret);

        if (query.exec() && query.next()) {
            return true;
        } else {
            QMessageBox::warning(this, "Erreur", "Code secret incorrect.");
            return false;
        }
    }
    return false;
}

void MainWindow::viewInfo()
{
    QSqlQuery query;
    query.prepare("SELECT c.solde, c.code_secret, c.rib FROM comptes c JOIN clients cl ON c.id = cl.compte_id WHERE cl.id = :client_id");
    query.bindValue(":client_id", clientId);

    if (query.exec() && query.next()) {
        double solde = query.value(0).toDouble();
        int codeSecret = query.value(1).toInt();
        QString rib = query.value(2).toString();

        QString info = QString("Solde: %1 €\nCode Secret: %2\nRIB: %3")
                        .arg(solde)
                        .arg(codeSecret)
                        .arg(rib);

        QMessageBox::information(this, "Informations", info);
    } else {
        QMessageBox::warning(this, "Erreur", "Erreur lors de la récupération des informations du client: " + query.lastError().text());
    }
}

void MainWindow::viewFullInfo()
{
    QSqlQuery query;
    query.prepare("SELECT cl.nom, cl.prenom, cl.cin, cl.telephone, c.solde, c.rib FROM clients cl JOIN comptes c ON cl.compte_id = c.id WHERE cl.id = :client_id");
    query.bindValue(":client_id", clientId);

    if (query.exec() && query.next()) {
        QString nom = query.value(0).toString();
        QString prenom = query.value(1).toString();
        QString cin = query.value(2).toString();
        QString telephone = query.value(3).toString();
        double solde = query.value(4).toDouble();
        QString rib = query.value(5).toString();

        QString info = QString("Nom: %1\nPrénom: %2\nCIN: %3\nTéléphone: +33 %4\nSolde: %5 €\nRIB: %6")
                        .arg(nom)
                        .arg(prenom)
                        .arg(cin)
                        .arg(telephone)
                        .arg(solde)
                        .arg(rib);

        QMessageBox::information(this, "Informations", info);
    } else {
        QMessageBox::warning(this, "Erreur", "Erreur lors de la récupération des informations du client: " + query.lastError().text());
    }
}

void MainWindow::majInfo()
{
    QSqlQuery query;
    query.prepare("SELECT cl.nom, cl.prenom, cl.cin, cl.telephone, c.solde, c.rib FROM clients cl JOIN comptes c ON cl.compte_id = c.id WHERE cl.id = :client_id");
    query.bindValue(":client_id", clientId);

    if (query.exec() && query.next()) {
        QString nom = query.value(0).toString();
        QString prenom = query.value(1).toString();
        QString cin = query.value(2).toString();
        QString telephone = query.value(3).toString();
        double solde = query.value(4).toDouble();
        QString rib = query.value(5).toString();
        welcomeLabel->setText(QString("Bienvenue %1 %2").arg(prenom).arg(nom));
        balanceLabel->setText(QString("Votre solde est de : %1 €").arg(solde));
    }
}

void MainWindow::editCin()
{
    if (verifyCodeSecret()) {
        bool ok;
        QString newCin = QInputDialog::getText(this, tr("Modifier CIN"), tr("Nouveau CIN:"), QLineEdit::Normal, "", &ok);
        if (ok && !newCin.isEmpty()) {
            QSqlQuery query;
            query.prepare("UPDATE clients SET cin = :new_cin WHERE id = :client_id");
            query.bindValue(":new_cin", newCin);
            query.bindValue(":client_id", clientId);

            if (query.exec()) {
                QMessageBox::information(this, "Modifier CIN", "CIN modifié avec succès.");
                viewInfo();
            } else {
                QMessageBox::warning(this, "Erreur", "Erreur lors de la modification du CIN: " + query.lastError().text());
            }
        }
    }
}

void MainWindow::editCodeSecret()
{
    if (verifyCodeSecret()) {
        bool ok;
        QString newCodeSecret = QInputDialog::getText(this, tr("Modifier Code Secret"), tr("Nouveau Code Secret:"), QLineEdit::Password, "", &ok);
        if (ok && !newCodeSecret.isEmpty()) {
            QSqlQuery query;
            query.prepare("UPDATE comptes SET code_secret = :new_code_secret WHERE id = (SELECT compte_id FROM clients WHERE id = :client_id)");
            query.bindValue(":new_code_secret", newCodeSecret);
            query.bindValue(":client_id", clientId);

            if (query.exec()) {
                QMessageBox::information(this, "Modifier Code Secret", "Code Secret modifié avec succès.");
            } else {
                QMessageBox::warning(this, "Erreur", "Erreur lors de la modification du Code Secret: " + query.lastError().text());
            }
        }
    }
}

void MainWindow::logout()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Déconnexion", QString("Êtes-vous sûr de vouloir vous déconnecter ?"), QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        LoginWindow *loginWindow = new LoginWindow();
        loginWindow->show();
        this->close();
    }
    else if (reply == QMessageBox::No)
    {
        QMessageBox::information(this, ":)", "J'ai toujours su que vous vouliez rester !");
    }
}

void MainWindow::showCredits()
{
    QMessageBox::information(this, "Crédit", "Développé par Etem ! \nGitHub : @Etem-Source");
    QDesktopServices::openUrl(QUrl("https://github.com/Etem-Source"));
}
