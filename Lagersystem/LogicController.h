#pragma once

#include <database_sql.h>
#include <QMessageBox>

class LogicController
{

public:

	LogicController();
	~LogicController();



	QString search_in_lagerbestand(QSqlDatabase database, QSqlQueryModel* query_model, QString artikelnummer = "", QString kategorie = "",
		QString bezeichnung = "", QString menge = "", QString regal = "", QString mindestmenge = "");

	QString search_in_buchungsverlauf(QSqlDatabase database, QSqlQueryModel* query_model, QString zeitpunkt = "", QString artikelnummer = "", QString bezeichnung = "", QString buchungsart = "");

	QString store_lagerbestand(QSqlDatabase database, QSqlQueryModel* query_model, QString artikelnummer, QString amount, bool storetype); //storetype == false: Einbuchen | storetype == true: Ausbuchen

	QString delete_lagerbestand_check(QSqlDatabase database, QString artikelnummer);

	QString delete_lagerbestand(QSqlDatabase database, QSqlQueryModel* query_model, QString artikelnummer);

	QString add_lagerbestand_check(QSqlDatabase database, QString artikelnummer, QString kategorie,
		QString bezeichnung, QString menge, QString regal, QString mindestmenge);

	QString add_lagerbestand(QSqlDatabase database, QSqlQueryModel* query_model, QString artikelnummer, QString kategorie,
		QString bezeichnung, QString menge, QString regal, QString mindestmenge);

	QString change_lagerbestand_check(QSqlDatabase database, QString artikelnummer_old, QString artikelnummer_new, QString kategorie_new,
		QString bezeichnung_new, QString menge_new, QString regal_new, QString mindestmenge_new);

	QString change_lagerbestand(QSqlDatabase database, QSqlQueryModel* query_model, QString artikelnummer_old, QString artikelnummer_new, QString kategorie_new,
		QString bezeichnung_new, QString menge_new, QString regal_new, QString mindestmenge_new);

	QString search_in_aenderungsverlauf(QSqlDatabase database, QSqlQueryModel* query_model, QString zeitpunkt = "", QString artikelnummer = "", QString bezeichnung = "", QString aenderungsart = "");

	QString set_query_sql_text(QSqlDatabase database, QSqlQueryModel* query_model, QString sql_text);

private:

};
