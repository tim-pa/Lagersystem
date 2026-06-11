#include "LogicController.h"
#include <QMessageBox>
#include <QSqlError>
#include <QDateTime>

LogicController::LogicController(){
}

LogicController::~LogicController() {
}

QString LogicController::set_query_sql_text(QSqlDatabase database, QSqlQueryModel* query_model, QString sql_text) {
    query_model->setQuery(sql_text, database);
    if (query_model->lastError().isValid()) {
        return "Fehler: " + query_model->lastError().text();
    }
    return "";    
}

QString LogicController::search_in_lagerbestand(QSqlDatabase database, QSqlQueryModel* query_model, QString artikelnummer, QString kategorie,
    QString bezeichnung, QString menge, QString regal, QString mindestmenge) {

    QSqlQuery query(database);

    QStringList bedingungen;

    if (artikelnummer != "") {
        bedingungen << QString("Artikelnummer LIKE :artikelnummer");
    }
    if (kategorie != "") {
        bedingungen << QString("Kategorie LIKE :kategorie");
    }
    if (bezeichnung != "") {
        bedingungen << QString("Bezeichnung LIKE :bezeichnung");
    }
    if (menge != "") {
        bedingungen << QString("Menge LIKE :menge");
    }
    if (regal != "") {
        bedingungen << QString("Regal LIKE :regal");
    }
    if (mindestmenge != "") {
        bedingungen << QString("Mindestmenge LIKE :mindestmenge");
    }

    QString anweisung = "SELECT * FROM lagerbestand";

    if (!bedingungen.isEmpty()) {
        anweisung += " WHERE ";
        anweisung += bedingungen.join(" AND ");
    }

    query.prepare(anweisung);


    if (artikelnummer != "") {
        query.bindValue(":artikelnummer", "%" + artikelnummer + "%");
    }
    if (kategorie != ""){
        query.bindValue(":kategorie", "%" + kategorie + "%");
    }
    if (bezeichnung != "") {
        query.bindValue(":bezeichnung", "%" + bezeichnung + "%");
    }
    if (menge != "") {
        query.bindValue(":menge", "%" + menge + "%");
    }
    if (regal != "") {
        query.bindValue(":regal", "%" + regal + "%");
    }
    if (mindestmenge != "") {
        query.bindValue(":mindestmenge", "%" + mindestmenge + "%");
    }

    if (!query.exec()) {
        return "Fehler: " + query.lastError().text();
    }

    query_model->setQuery(std::move(query));
    return "";
}

QString LogicController::search_in_buchungsverlauf(QSqlDatabase database, QSqlQueryModel* query_model, QString zeitpunkt, QString artikelnummer, QString bezeichnung, QString buchungsart) {
    QSqlQuery query(database);

    QStringList bedingungen;

    if (zeitpunkt != "") {
        bedingungen << QString("Zeitpunkt LIKE :zeitpunkt");
    }
    if (artikelnummer != "") {
        bedingungen << QString("Artikelnummer LIKE :artikelnummer");
    }
    if (bezeichnung != "") {
        bedingungen << QString("Bezeichnung LIKE :bezeichnung");
    }
    if (buchungsart != "") {
        bedingungen << QString("Buchungsart LIKE :buchungsart");
    }

    QString anweisung = "SELECT * FROM buchungsverlauf";

    if (!bedingungen.isEmpty()) {
        anweisung += " WHERE ";
        anweisung += bedingungen.join(" AND ");
    }

    anweisung += " ORDER BY -Buchungs_ID";

    query.prepare(anweisung);

    if (zeitpunkt != "") {
        query.bindValue(":zeitpunkt", "%" + zeitpunkt + "%");
    }
    if (artikelnummer != "") {
        query.bindValue(":artikelnummer", "%" + artikelnummer + "%");
    }
    if (bezeichnung != "") {
        query.bindValue(":bezeichnung", "%" + bezeichnung + "%");
    }
    if (buchungsart != "") {
        query.bindValue(":buchungsart", "%" + buchungsart + "%");
    }


    if (!query.exec()) {
        return "Fehler: " + query.lastError().text();
    }

    query_model->setQuery(std::move(query));
    return "";
}

QString LogicController::store_lagerbestand(QSqlDatabase database, QSqlQueryModel* query_model, QString artikelnummer, QString amount, bool storetype) {
    if (amount.toInt() > 0 && artikelnummer != "") {
        QSqlQuery query(database);
        query.prepare("SELECT * FROM lagerbestand WHERE Artikelnummer = :artikelnummer");
        query.bindValue(":artikelnummer", artikelnummer);
        if (!query.exec()) {
            return "Fehler: " + query.lastError().text();
        }

        if (query.next()) {            
            int new_amount = 0;

            int old_amount = query.value(3).toInt();
            if (storetype) {
                new_amount = old_amount - amount.toInt();
            }
            else {                
                new_amount = old_amount + amount.toInt();
            }

            if (new_amount >= 0) {
                QString new_amount_string = QString::number(new_amount);


                // Buchungsverlauf
                QString artikelnummer_db = query.value(0).toString();
                QString kategorie = query.value(1).toString();
                QString bezeichnung = query.value(2).toString();
                QString regal = query.value(4).toString();
                QString mindestmenge = query.value(5).toString();

                //Store vorgang
                query.prepare("UPDATE lagerbestand SET menge = :menge WHERE Artikelnummer = :artikelnummer");
                query.bindValue(":menge", new_amount_string);
                query.bindValue(":artikelnummer", artikelnummer);
                if (!query.exec()) {
                    return "Fehler: " + query.lastError().text();
                }


                //Buchungsverlauf
                QSqlQuery query_verlauf(database);
                query_verlauf.prepare(R"(
                    INSERT INTO buchungsverlauf (Zeitpunkt, Artikelnummer, Kategorie, Bezeichnung, Regal, Mindestmenge, Buchungsart, Buchungsmenge, alte_Menge, neue_Menge)   
                    VALUES (:zeitpunkt, :artikelnummer, :kategorie, :bezeichnung, :regal, :mindestmenge, :buchungsart, :buchungsmenge, :alte_menge, :neue_menge) 
                )");

                query_verlauf.bindValue(":zeitpunkt", QDateTime::currentDateTime().toString(Qt::ISODate));
                query_verlauf.bindValue(":artikelnummer", artikelnummer_db);
                query_verlauf.bindValue(":kategorie", kategorie);
                query_verlauf.bindValue(":bezeichnung", bezeichnung);
                query_verlauf.bindValue(":regal", regal);
                query_verlauf.bindValue(":mindestmenge", mindestmenge);

                if (storetype) {
                    query_verlauf.bindValue(":buchungsart", "Ausbuchung");
                }
                else {
                    query_verlauf.bindValue(":buchungsart", "Einbuchung");
                }

                query_verlauf.bindValue(":buchungsmenge", amount);
                query_verlauf.bindValue(":alte_menge", old_amount);
                query_verlauf.bindValue(":neue_menge", new_amount);
                if (!query_verlauf.exec()) {
                    return "Fehler: " + query_verlauf.lastError().text();
                }


                //Store vorgang
                query.prepare("SELECT * FROM lagerbestand WHERE Artikelnummer = :artikelnummer");
                query.bindValue(":artikelnummer", artikelnummer);
                if (!query.exec()) {
                    return "Fehler: " + query.lastError().text();
                }

                query_model->setQuery(std::move(query));

                return "";
            }
            else {
                return "Es kann nicht mehr ausgebucht werden, als vorhanden ist!";
            }

        }
        else {
            return "Keinen Artikel gefunden!";
        }

    }
    else {
        if (!(amount.toInt() > 0)) {
            return "Buchungsmenge muss größer 0 sein!";
        }
        else if (artikelnummer == "") {
            return "Keinen Artikel ausgewählt! Bitte Artikel markieren!";
        }
    }
    return "Unbekannter Fehler!";
}

QString LogicController::delete_lagerbestand_check(QSqlDatabase database, QString artikelnummer) {
    if (artikelnummer != "") {
        QSqlQuery query(database);
        query.prepare("SELECT * FROM lagerbestand WHERE Artikelnummer = :artikelnummer");
        query.bindValue(":artikelnummer", artikelnummer);
        if (!query.exec()) {
            return "Fehler: " + query.lastError().text();
        }
        if (!query.next()) {
            return "Keinen Artikel gefunden!";
        }
            
        return "";
 
    }
    else {
        return "Keinen Artikel ausgewählt! Bitte Artikel markieren!";
    }
}

QString LogicController::delete_lagerbestand(QSqlDatabase database, QSqlQueryModel* query_model, QString artikelnummer) {
    QSqlQuery query(database);

    //Aenderungsverlauf vorbereiten
    query.prepare("SELECT * FROM lagerbestand WHERE Artikelnummer = :artikelnummer");
    query.bindValue(":artikelnummer", artikelnummer);
    if (!query.exec()) {
        return "Fehler: " + query.lastError().text();
    }
    if (!query.next()) {
        return "Keinen Artikel gefunden!";
    }
    QString artikelnummer_old_str = artikelnummer;
    QString kategorie_old_str = query.value(1).toString();
    QString bezeichnung_old_str = query.value(2).toString();
    QString menge_old_str = query.value(3).toString();
    QString regal_old_str = query.value(4).toString();
    QString mindestmenge_old_str = query.value(5).toString();


    //Delete Vorgang
    query.prepare("DELETE FROM lagerbestand WHERE Artikelnummer = :artikelnummer");
    query.bindValue(":artikelnummer", artikelnummer);
    if (!query.exec()) {
        return "Fehler: " + query.lastError().text();
    }

    //Aenderungsverlauf abspeichern
    QSqlQuery query_verlauf(database);
    query_verlauf.prepare(R"(
                    INSERT INTO aenderungsverlauf (Zeitpunkt, alte_Artikelnummer, alte_Kategorie, alte_Bezeichnung, alte_Menge, altes_Regal, alte_Mindestmenge, Aenderungsart, 
                    neue_Artikelnummer, neue_Kategorie, neue_Bezeichnung, neue_Menge, neues_Regal, neue_Mindestmenge)
                    VALUES (:zeitpunkt, :alte_artikelnummer, :alte_kategorie, :alte_bezeichnung, :alte_menge, :altes_regal, :alte_mindestmenge, :aenderungsart,
                    :neue_artikelnummer, :neue_kategorie, :neue_bezeichnung, :neue_menge, :neues_regal, :neue_mindestmenge) 
                )");

    query_verlauf.bindValue(":zeitpunkt", QDateTime::currentDateTime().toString(Qt::ISODate));

    query_verlauf.bindValue(":alte_artikelnummer", artikelnummer_old_str);
    query_verlauf.bindValue(":alte_kategorie", kategorie_old_str);
    query_verlauf.bindValue(":alte_bezeichnung", bezeichnung_old_str);
    query_verlauf.bindValue(":alte_menge", menge_old_str);
    query_verlauf.bindValue(":altes_regal", regal_old_str);
    query_verlauf.bindValue(":alte_mindestmenge", mindestmenge_old_str);

    query_verlauf.bindValue(":aenderungsart", "Loeschung");

    query_verlauf.bindValue(":neue_artikelnummer", "");
    query_verlauf.bindValue(":neue_kategorie", "");
    query_verlauf.bindValue(":neue_bezeichnung", "");
    query_verlauf.bindValue(":neue_menge", "");
    query_verlauf.bindValue(":neues_regal", "");
    query_verlauf.bindValue(":neue_mindestmenge", "");

    if (!query_verlauf.exec()) {
        return "Fehler: " + query_verlauf.lastError().text();
    }

    //Anzeige
    query.prepare("SELECT * FROM lagerbestand");
    if (!query.exec()) {
        return "Fehler: " + query.lastError().text();
    }

    query_model->setQuery(std::move(query));

    return "";

}

QString LogicController::add_lagerbestand_check(QSqlDatabase database, QString artikelnummer, QString kategorie,
    QString bezeichnung, QString menge, QString regal, QString mindestmenge) {
    if (artikelnummer != "" && kategorie != "" && bezeichnung != "" && menge != "" && regal != "" && mindestmenge != "") {

        QSqlQuery query(database);
        query.prepare("SELECT * FROM lagerbestand WHERE Artikelnummer = :artikelnummer");
        query.bindValue(":artikelnummer", artikelnummer);
        if (!query.exec()) {
            return "Fehler: " + query.lastError().text();
        }
        if (query.next()) {
            return "Artikelnummer existiert bereits!";
        }

        bool ok;
        int artikelnummer_int = artikelnummer.toInt(&ok);
        if (!ok) {
            return "Artikelnummer muss eine Zahl sein";
        }
        if (artikelnummer_int < 0) {
            return "Artikelnummer muss größer gleich Null sein";
        }

        int menge_int = menge.toInt(&ok);
        if (!ok) {
            return "Menge muss eine Zahl sein";
        }
        if (menge_int < 0) {
            return "Menge muss größer gleich Null sein";
        }

        int mindestmenge_int = mindestmenge.toInt(&ok);
        if (!ok) {
            return "Mindestmenge muss eine Zahl sein";
        }
        if (mindestmenge_int < 0) {
            return "Mindestmenge muss größer gleich Null sein";
        }

        return "";

    }
    else {
        return "Alle Felder müssen ausgefüllt sein!";
    }
}

QString LogicController::add_lagerbestand(QSqlDatabase database, QSqlQueryModel* query_model, QString artikelnummer, QString kategorie,
    QString bezeichnung, QString menge, QString regal, QString mindestmenge) {
    QSqlQuery query(database);
    query.prepare(R"(
            INSERT INTO lagerbestand (Artikelnummer, Kategorie ,Bezeichnung, Menge, Regal, Mindestmenge)
            VALUES(:artikelnummer, :kategorie, :bezeichnung, :menge, :regal, :mindestmenge)
        )");
    query.bindValue(":artikelnummer", artikelnummer);
    query.bindValue(":kategorie", kategorie);
    query.bindValue(":bezeichnung", bezeichnung);
    query.bindValue(":menge", menge);
    query.bindValue(":regal", regal);
    query.bindValue(":mindestmenge", mindestmenge);
    if (!query.exec()) {
        return "Fehler: " + query.lastError().text();
    }

    //Aenderungsverlauf abspeichern
    QSqlQuery query_verlauf(database);
    query_verlauf.prepare(R"(
                    INSERT INTO aenderungsverlauf (Zeitpunkt, alte_Artikelnummer, alte_Kategorie, alte_Bezeichnung, alte_Menge, altes_Regal, alte_Mindestmenge, Aenderungsart, 
                    neue_Artikelnummer, neue_Kategorie, neue_Bezeichnung, neue_Menge, neues_Regal, neue_Mindestmenge)
                    VALUES (:zeitpunkt, :alte_artikelnummer, :alte_kategorie, :alte_bezeichnung, :alte_menge, :altes_regal, :alte_mindestmenge, :aenderungsart,
                    :neue_artikelnummer, :neue_kategorie, :neue_bezeichnung, :neue_menge, :neues_regal, :neue_mindestmenge) 
                )");

    query_verlauf.bindValue(":zeitpunkt", QDateTime::currentDateTime().toString(Qt::ISODate));

    query_verlauf.bindValue(":alte_artikelnummer", "");
    query_verlauf.bindValue(":alte_kategorie", "");
    query_verlauf.bindValue(":alte_bezeichnung", "");
    query_verlauf.bindValue(":alte_menge", "");
    query_verlauf.bindValue(":altes_regal", "");
    query_verlauf.bindValue(":alte_mindestmenge", "");

    query_verlauf.bindValue(":aenderungsart", "Hinzufuegung");

    query_verlauf.bindValue(":neue_artikelnummer", artikelnummer);
    query_verlauf.bindValue(":neue_kategorie", kategorie);
    query_verlauf.bindValue(":neue_bezeichnung", bezeichnung);
    query_verlauf.bindValue(":neue_menge", menge);
    query_verlauf.bindValue(":neues_regal", regal);
    query_verlauf.bindValue(":neue_mindestmenge", mindestmenge);

    if (!query_verlauf.exec()) {
        return "Fehler: " + query_verlauf.lastError().text();
    }



    query.prepare("SELECT * FROM lagerbestand WHERE Artikelnummer = :artikelnummer");
    query.bindValue(":artikelnummer", artikelnummer);
    if (!query.exec()) {
        return "Fehler: " + query.lastError().text();
    }

    query_model->setQuery(std::move(query));

    return "";

}

QString LogicController::change_lagerbestand_check(QSqlDatabase database, QString artikelnummer_old, QString artikelnummer_new, QString kategorie_new,
    QString bezeichnung_new, QString menge_new, QString regal_new, QString mindestmenge_new) {
    if (artikelnummer_old != "") {
        if ( !(artikelnummer_new == "" && kategorie_new == "" && bezeichnung_new == "" && menge_new == "" && regal_new == "" && mindestmenge_new == "") ) {
            bool ok;
            if (artikelnummer_new != "") {
                QSqlQuery query(database);
                query.prepare("SELECT * FROM lagerbestand WHERE Artikelnummer = :artikelnummer");
                query.bindValue(":artikelnummer", artikelnummer_new);
                if (!query.exec()) {
                    return "Fehler: " + query.lastError().text();
                }
                if (query.next()) {
                    return "Artikelnummer existiert bereits!";
                }
            }

            

            if (artikelnummer_new != "") {
                int artikelnummer_int = artikelnummer_new.toInt(&ok);
                if (!ok) {
                    return "Artikelnummer muss eine Zahl sein";
                }
                if (artikelnummer_int < 0) {
                    return "Artikelnummer muss größer gleich Null sein";
                }
                
            }
            

            
            if (menge_new != "") {
                int menge_int = menge_new.toInt(&ok);
                if (!ok) {
                    return "Menge muss eine Zahl sein";
                }
                if (menge_int < 0) {
                    return "Menge muss größer gleich Null sein";
                }
            }
           
            
            if (mindestmenge_new != "") {
                int mindestmenge_int = mindestmenge_new.toInt(&ok);
                if (!ok) {
                    return "Mindestmenge muss eine Zahl sein";
                }
                if (mindestmenge_int < 0) {
                    return "Mindestmenge muss größer gleich Null sein";
                }
            }
            
            QSqlQuery query(database);
            query.prepare("SELECT * FROM lagerbestand WHERE Artikelnummer = :artikelnummer");
            query.bindValue(":artikelnummer", artikelnummer_old);
            if (!query.exec()) {
                return "Fehler: " + query.lastError().text();
            }
            if (!query.next()) {
                return "Keinen Artikel gefunden!";
            }
            QString kategorie_old = query.value(1).toString();
            QString bezeichnung_old = query.value(2).toString();
            QString menge_old = query.value(3).toString();
            QString regal_old = query.value(4).toString();
            QString mindestmenge_old = query.value(5).toString();

            if (kategorie_new != "" && kategorie_old == kategorie_new) {
                return "Die neue Kategorie muss abweichen von der aktuellen Kategorie!";
            }
            if (bezeichnung_new != "" && bezeichnung_old == bezeichnung_new) {
                return "Die neue Bezeichnung muss abweichen von der aktuellen Bezeichnung!";
            }
            if (menge_new != "" && menge_old == menge_new) {
                return "Die neue Menge muss abweichen von der aktuellen Menge!";
            }
            if (regal_new != "" && regal_old == regal_new) {
                return "Das neue Regal muss abweichen von dem aktuellen Regal!";
            }
            if (mindestmenge_new != "" && mindestmenge_old == mindestmenge_new) {
                return "Die neue Mindestmenge muss abweichen von der aktuellen Mindestmenge!";
            }
            

            return "";

        }
        else {
            return "Mindestens ein Feld muss nicht leer sein!";
        }
    }
    else {
        return "Keinen Artikel ausgewählt! Bitte Artikel markieren!";
    }
}

QString LogicController::change_lagerbestand(QSqlDatabase database, QSqlQueryModel * query_model, QString artikelnummer_old, QString artikelnummer_new, QString kategorie_new,
    QString bezeichnung_new, QString menge_new, QString regal_new, QString mindestmenge_new) {

    QSqlQuery query(database);

    //Aenderungsverlauf vorbereiten
    query.prepare("SELECT * FROM lagerbestand WHERE Artikelnummer = :artikelnummer");
    query.bindValue(":artikelnummer", artikelnummer_old);
    if (!query.exec()) {
        return "Fehler: " + query.lastError().text();
    }
    if (!query.next()) {
        return "Keinen Artikel gefunden!";
    }
    QString kategorie_old_str = query.value(1).toString();
    QString bezeichnung_old_str = query.value(2).toString();
    QString menge_old_str = query.value(3).toString();
    QString regal_old_str = query.value(4).toString();
    QString mindestmenge_old_str = query.value(5).toString();






    //Lagerbestand ändern
    QStringList bedingungen;

    if (artikelnummer_new != "") {
        bedingungen << QString("Artikelnummer = :artikelnummer_new");
    }
    if (kategorie_new != "") {
        bedingungen << QString("Kategorie = :kategorie_new");
    }
    if (bezeichnung_new != "") {
        bedingungen << QString("Bezeichnung = :bezeichnung_new");
    }
    if (menge_new != "") {
        bedingungen << QString("Menge = :menge_new");
    }
    if (regal_new != "") {
        bedingungen << QString("Regal = :regal_new");
    }
    if (mindestmenge_new != "") {
        bedingungen << QString("Mindestmenge = :mindestmenge_new");
    }

    QString anweisung = "UPDATE lagerbestand SET ";

    if (!bedingungen.isEmpty()) {
        anweisung += bedingungen.join(", ");
        anweisung += " WHERE Artikelnummer = :artikelnummer_old";
    }else {
        return "Keine Änderungen angegeben!";
    }

    query.prepare(anweisung);

    if (artikelnummer_new != "") {
        query.bindValue(":artikelnummer_new", artikelnummer_new);
    }
    if (kategorie_new != "") {
        query.bindValue(":kategorie_new", kategorie_new);
    }
    if (bezeichnung_new != "") {
        query.bindValue(":bezeichnung_new", bezeichnung_new);
    }
    if (menge_new != "") {
        query.bindValue(":menge_new", menge_new);
    }
    if (regal_new != "") {
        query.bindValue(":regal_new", regal_new);
    }
    if (mindestmenge_new != "") {
        query.bindValue(":mindestmenge_new", mindestmenge_new);
    }

    query.bindValue(":artikelnummer_old", artikelnummer_old);


    if (!query.exec()) {
        return "Fehler: " + query.lastError().text();
    }




    //Aenderungsverlauf vorbereiiten
    query.prepare("SELECT * FROM lagerbestand WHERE Artikelnummer = :artikelnummer");
    if (artikelnummer_new != "") {
        query.bindValue(":artikelnummer", artikelnummer_new);
    }
    else {
        query.bindValue(":artikelnummer", artikelnummer_old);
    }
    if (!query.exec()) {
        return "Fehler: " + query.lastError().text();
    }
    if (!query.next()) {
        return "Keinen Artikel gefunden!";
    }
    QString artikelnummer_new_str = query.value(0).toString();
    QString kategorie_new_str = query.value(1).toString();
    QString bezeichnung_new_str = query.value(2).toString();
    QString menge_new_str = query.value(3).toString();
    QString regal_new_str = query.value(4).toString();
    QString mindestmenge_new_str = query.value(5).toString();

    //Aenderungsverlauf abspeichern
    QSqlQuery query_verlauf(database);
    query_verlauf.prepare(R"(
                    INSERT INTO aenderungsverlauf (Zeitpunkt, alte_Artikelnummer, alte_Kategorie, alte_Bezeichnung, alte_Menge, altes_Regal, alte_Mindestmenge, Aenderungsart, 
                    neue_Artikelnummer, neue_Kategorie, neue_Bezeichnung, neue_Menge, neues_Regal, neue_Mindestmenge)
                    VALUES (:zeitpunkt, :alte_artikelnummer, :alte_kategorie, :alte_bezeichnung, :alte_menge, :altes_regal, :alte_mindestmenge, :aenderungsart,
                    :neue_artikelnummer, :neue_kategorie, :neue_bezeichnung, :neue_menge, :neues_regal, :neue_mindestmenge) 
                )");

    query_verlauf.bindValue(":zeitpunkt", QDateTime::currentDateTime().toString(Qt::ISODate));

    query_verlauf.bindValue(":alte_artikelnummer", artikelnummer_old);
    query_verlauf.bindValue(":alte_kategorie", kategorie_old_str);
    query_verlauf.bindValue(":alte_bezeichnung", bezeichnung_old_str);
    query_verlauf.bindValue(":alte_menge", menge_old_str);
    query_verlauf.bindValue(":altes_regal", regal_old_str);
    query_verlauf.bindValue(":alte_mindestmenge", mindestmenge_old_str);

    query_verlauf.bindValue(":aenderungsart", "Aenderung");

    query_verlauf.bindValue(":neue_artikelnummer", artikelnummer_new_str);
    query_verlauf.bindValue(":neue_kategorie", kategorie_new_str);
    query_verlauf.bindValue(":neue_bezeichnung", bezeichnung_new_str);
    query_verlauf.bindValue(":neue_menge", menge_new_str);
    query_verlauf.bindValue(":neues_regal", regal_new_str);
    query_verlauf.bindValue(":neue_mindestmenge", mindestmenge_new_str);

    if (!query_verlauf.exec()) {
        return "Fehler: " + query_verlauf.lastError().text();
    }





    query.prepare("SELECT * FROM lagerbestand WHERE Artikelnummer = :artikelnummer");
    if (artikelnummer_new != "") {
        query.bindValue(":artikelnummer", artikelnummer_new);
    }
    else {
        query.bindValue(":artikelnummer", artikelnummer_old);
    }
    
    if (!query.exec()) {
        return "Fehler: " + query.lastError().text();
    }

    query_model->setQuery(std::move(query));

    return "";
}

QString LogicController::search_in_aenderungsverlauf(QSqlDatabase database, QSqlQueryModel* query_model, QString zeitpunkt, QString artikelnummer, QString bezeichnung, QString aenderungsart) {
    QSqlQuery query(database);

    QStringList bedingungen;

    if (zeitpunkt != "") {
        bedingungen << QString("Zeitpunkt LIKE :zeitpunkt");
    }
    if (artikelnummer != "") {
        bedingungen << QString("(alte_Artikelnummer LIKE :artikelnummer OR neue_Artikelnummer LIKE :artikelnummer)");
    }
    if (bezeichnung != "") {
        bedingungen << QString("(alte_Bezeichnung LIKE :bezeichnung OR neue_Bezeichnung LIKE :bezeichnung)");
    }
    if (aenderungsart != "") {
        bedingungen << QString("Aenderungsart LIKE :aenderungsart");
    }

    QString anweisung = "SELECT * FROM aenderungsverlauf";

    if (!bedingungen.isEmpty()) {
        anweisung += " WHERE ";
        anweisung += bedingungen.join(" AND ");
    }

    anweisung += " ORDER BY -Aenderungs_ID";

    query.prepare(anweisung);

    if (zeitpunkt != "") {
        query.bindValue(":zeitpunkt", "%" + zeitpunkt + "%");
    }
    if (artikelnummer != "") {
        query.bindValue(":artikelnummer", "%" + artikelnummer + "%");
    }
    if (bezeichnung != "") {
        query.bindValue(":bezeichnung", "%" + bezeichnung + "%");
    }
    if (aenderungsart != "") {
        query.bindValue(":aenderungsart", "%" + aenderungsart + "%");
    }


    if (!query.exec()) {
        return "Fehler: " + query.lastError().text();
    }

    query_model->setQuery(std::move(query));
    return "";
}