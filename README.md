# 💸 QT Compte Bancaire MySQL
Ce projet permet d'effectuer les tâches suivantes : 
* S'authentifier Client
  * Opérations
    * Crédit
    * Débit
    * Virement
    * Consulter Solde (Solde, Code Secret, RIB)
  * Compte
    * Consulter Informations (Nom, Prénom, CIN, Téléphone, Solde, RIB)
    * Modifier CIN
    * Modifier Code Secret
  * Options
    * Se déconnecter
    * Crédit

## 💡 Prérequis
* [G++ Compileur](https://sourceforge.net/projects/mingw/)
  * `sudo apt install g++`
* [VisualStudio Code](https://code.visualstudio.com/) ou autres IDE
* [QT Creator](https://doc.ubuntu-fr.org/qt-creator)
* Avoir ceci d'installé :
  * `sudo apt-get install libmysqlclient-dev`
  * `sudo apt-get install libqt5sql5-mysql`

## 💡 Prérequis MySQL
* Avoir MySQL
* Télécharger bdd-banque-qt.sql
* Importer la base de données dans votre serveur MySQL :
  * mysql -u username* -p db_name* < bdd-banque-qt.sql
    
*username = votre nom d'utilisateur.
*db_name = le nom de la bdd où vous souhaitez l'importer.

* N'oubliez pas de changer les informations de connexions par vos propres valeurs !
    
*user = votre nom d'utilisateur.
*pw = votre mot de passe.
*DB = le nom de votre BDD.
    
## 🛠️ Utilisation
Vous devez compiler et exécuter avec ceci : 

![image](https://github.com/user-attachments/assets/d2719e62-494d-430c-87ec-111cb52d9595)

## 📚 Interface / Exemple

Petite idée de l'interface.

* Connexion :

![image](https://github.com/user-attachments/assets/ba002066-63c4-4d89-9d83-cf543e18927f)

* Menu Client :

![image](https://github.com/user-attachments/assets/bd7dae3b-4059-415c-8120-ea7184fb35eb)

* BDD Sql :

![image](https://github.com/user-attachments/assets/2b113231-8b8b-44c8-a2f0-41d824e2e97b)


## 📧 Crédits
* [Etem](https://github.com/Etem-Source)

Merci d'avoir pris le temps de lire tout ceci.
Vous avez le droit de republier, réutiliser tout ce projet uniquement si vous créditez mon nom GitHub.
