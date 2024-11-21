#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "loginwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Charger le fichier QSS
    QFile file("/home/etem/QT-BDD-Banque/style.qss");
    if (!file.exists()) {
        qDebug() << "Le fichier style.qss n'existe pas";
    }
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);
        a.setStyleSheet(stream.readAll());
        qDebug() << "Fichier style.qss chargé avec succès";
    } else {
        qDebug() << "Erreur de chargement du fichier style.qss";
    }

    LoginWindow w;
    w.show();
    return a.exec();
}
