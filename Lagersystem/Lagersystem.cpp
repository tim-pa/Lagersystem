#include "Lagersystem.h"


#include <database_sql.h>
#include <qtableview.h>
#include <qcombobox.h>
#include <iostream>
#include <QMessageBox>

Lagersystem::Lagersystem(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    ui.stackedWidget->setCurrentIndex(0);

	creating_databases();
	query_model = new QSqlQueryModel(this);

    //Seite 0
    init_page_0();
	ui.tableView_0->setModel(query_model);
	ui.tableView_0->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //Seite 1
    init_page_1();
	ui.tableView_1->setModel(query_model);
	ui.tableView_1->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.tableView_1->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableView_1->setSelectionMode(QAbstractItemView::SingleSelection);
	//Seite 2
	init_page_2();
	ui.tableView_2->setModel(query_model);
	ui.tableView_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
	//Seite 3
	init_page_3();
	ui.tableView_3->setModel(query_model);
	ui.tableView_3->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.tableView_3->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableView_3->setSelectionMode(QAbstractItemView::SingleSelection);
	// Seite 4
	init_page_4();
	ui.tableView_4->setModel(query_model);
	ui.tableView_4->setEditTriggers(QAbstractItemView::NoEditTriggers);

	query_model->setQuery("SELECT * FROM lagerbestand", db);

	


	
}

Lagersystem::~Lagersystem()
{}

// Seite 0

void Lagersystem::search_button_0_clicked() {
	
	QString articlenumber = ui.lineEdit_articlenumber_0->text();
	QString category = ui.lineEdit_category_0->text();
	QString articlename = ui.lineEdit_articlename_0->text();
	QString amount = ui.lineEdit_amount_0->text();
	QString shelf = ui.lineEdit_shelf_0->text();
	QString minamount = ui.lineEdit_minamount_0->text();

	QString error = logiccontroller.search_in_lagerbestand(db, query_model, articlenumber, category, articlename, amount, shelf, minamount);
	if (!error.isEmpty()) {
		QMessageBox::warning(this, "Warnung", error);
	}

	selected_articlenumber = "";
	ui.tableView_0->clearSelection();
}

void Lagersystem::store_button_clicked() {
    ui.stackedWidget->setCurrentIndex(1);
	selected_articlenumber = "";
	query_model->setQuery("SELECT * FROM lagerbestand", db);
	ui.tableView_1->clearSelection();
 
}

void Lagersystem::store_history_button_clicked() {
	ui.stackedWidget->setCurrentIndex(2);
	selected_articlenumber = "";
	query_model->setQuery("SELECT * FROM buchungsverlauf ORDER BY -Buchungs_ID", db);
}

void Lagersystem::change_button_clicked() {
	ui.stackedWidget->setCurrentIndex(3);
	selected_articlenumber = "";
	query_model->setQuery("SELECT * FROM lagerbestand", db);
	ui.tableView_1->clearSelection();
}

void Lagersystem::change_history_button_clicked() {
	ui.stackedWidget->setCurrentIndex(4);
	selected_articlenumber = "";
	query_model->setQuery("SELECT * FROM aenderungsverlauf ORDER BY -Aenderungs_ID", db);
}


// Seite 1

void Lagersystem::search_button_1_clicked() {

	QString articlenumber = ui.lineEdit_articlenumber_1->text();
	QString articlename = ui.lineEdit_articlename_1->text();
	
	QString error = logiccontroller.search_in_lagerbestand(db, query_model, articlenumber, "", articlename);
	if (!error.isEmpty()) {
		QMessageBox::warning(this, "Warnung", error);
	}

	selected_articlenumber = "";
	ui.tableView_1->clearSelection();
}

void Lagersystem::store_in_button_1_clicked() {
	QString add_amount = ui.spinBox_amount_1->text();
	
	QString error = logiccontroller.store_lagerbestand(db, query_model, selected_articlenumber, add_amount, false);
	if (!error.isEmpty()) {
		QMessageBox::warning(this, "Warnung", error);
	}
	ui.spinBox_amount_1->setValue(0);
	selected_articlenumber = "";
	ui.tableView_1->clearSelection();
}

void Lagersystem::store_out_button_1_clicked() {

	QString remove_amount = ui.spinBox_amount_1->text();

	QString error = logiccontroller.store_lagerbestand(db, query_model, selected_articlenumber, remove_amount, true);
	if (!error.isEmpty()) {
		QMessageBox::warning(this, "Warnung", error);		
	}
	ui.spinBox_amount_1->setValue(0);
	selected_articlenumber = "";
	ui.tableView_1->clearSelection();
}

void Lagersystem::tableview_1_clicked(const QModelIndex& index) {

	selected_articlenumber = query_model->data(query_model->index(index.row(), 0)).toString();
	
	QString error = logiccontroller.search_in_lagerbestand(db, query_model, selected_articlenumber);
	if (!error.isEmpty()) {
		QMessageBox::warning(this, "Warnung", error);
	}

	ui.tableView_1->selectRow(0);

}

void Lagersystem::exit_button_1_clicked() {
	ui.stackedWidget->setCurrentIndex(0);
	query_model->setQuery("SELECT * FROM lagerbestand", db);
}

// Seite 2

void Lagersystem::search_button_2_clicked() {
	QString timestring = ui.lineEdit_time_2->text();
	QString articlenumber = ui.lineEdit_articlenumber_2->text();
	QString articlename = ui.lineEdit_articlename_2->text();
	QString storecategory = ui.lineEdit_storecategory_2->text();

	QString error = logiccontroller.search_in_buchungsverlauf(db, query_model, timestring, articlenumber, articlename, storecategory);
	if (!error.isEmpty()) {
		QMessageBox::warning(this, "Warnung", error);
	}

	selected_articlenumber = "";
}

void Lagersystem::exit_button_2_clicked() {
	ui.stackedWidget->setCurrentIndex(0);
	query_model->setQuery("SELECT * FROM lagerbestand", db);
}

// Seite 3

void Lagersystem::search_button_3_clicked() {
	QString articlenumber = ui.lineEdit_articlenumber_3_1->text();
	QString articlename = ui.lineEdit_articlename_3_1->text();

	QString error = logiccontroller.search_in_lagerbestand(db, query_model, articlenumber, "", articlename);
	if (!error.isEmpty()) {
		QMessageBox::warning(this, "Warnung", error);
	}

	selected_articlenumber = "";
	ui.tableView_3->clearSelection();
}

void Lagersystem::tableview_3_clicked(const QModelIndex& index) {

	selected_articlenumber = query_model->data(query_model->index(index.row(), 0)).toString();

	QString error = logiccontroller.search_in_lagerbestand(db, query_model, selected_articlenumber);
	if (!error.isEmpty()) {
		QMessageBox::warning(this, "Warnung", error);
	}

	ui.tableView_3->selectRow(0);

}

void Lagersystem::exit_button_3_clicked() {
	ui.stackedWidget->setCurrentIndex(0);
	query_model->setQuery("SELECT * FROM lagerbestand", db);
}

void Lagersystem::delete_button_3_clicked() {

	QString error = logiccontroller.delete_lagerbestand_check(db, selected_articlenumber);
	if (!error.isEmpty()) {
		QMessageBox::warning(this, "Warnung", error);
	}
	else {
		QMessageBox box(this);
		box.setWindowTitle("Löschen bestätigen");
		box.setText(QString("Möchten Sie den markierten Artikel (Artikelnummer: %1) wirklich löschen?").arg(selected_articlenumber));
		box.setIcon(QMessageBox::Warning);
		QPushButton* bestaetigen = box.addButton("Bestätigen", QMessageBox::AcceptRole);
		box.addButton("Abbrechen", QMessageBox::RejectRole);
		box.exec();

		if (box.clickedButton() == bestaetigen) {
			QString error = logiccontroller.delete_lagerbestand(db, query_model, selected_articlenumber);
			if (!error.isEmpty()) {
				QMessageBox::warning(this, "Warnung", error);
			}
		}
	}

	selected_articlenumber = "";
	ui.tableView_3->clearSelection();
}

void Lagersystem::add_button_3_clicked() {

	QString articlenumber = ui.lineEdit_articlenumber_3->text();
	QString category = ui.lineEdit_category_3->text();
	QString articlename = ui.lineEdit_articlename_3->text();
	QString amount = ui.lineEdit_amount_3->text();
	QString shelf = ui.lineEdit_shelf_3->text();
	QString minamount = ui.lineEdit_minamount_3->text();



	QString error = logiccontroller.add_lagerbestand_check(db, articlenumber, category, articlename, amount, shelf, minamount);
	if (!error.isEmpty()) {
		QMessageBox::warning(this, "Warnung", error);
	}
	else {
		QMessageBox box(this);
		box.setWindowTitle("Hinzufügen bestätigen");
		box.setText(QString("Möchten Sie den Artikel wirklich hinzufügen?"));
		box.setIcon(QMessageBox::Warning);
		QPushButton* bestaetigen = box.addButton("Bestätigen", QMessageBox::AcceptRole);
		box.addButton("Abbrechen", QMessageBox::RejectRole);
		box.exec();

		if (box.clickedButton() == bestaetigen) {
			QString error = logiccontroller.add_lagerbestand(db, query_model, articlenumber, category, articlename, amount, shelf, minamount);
			if (!error.isEmpty()) {
				QMessageBox::warning(this, "Warnung", error);
			}else {
				ui.lineEdit_articlenumber_3->clear();
				ui.lineEdit_category_3->clear();
				ui.lineEdit_articlename_3->clear();
				ui.lineEdit_amount_3->clear();
				ui.lineEdit_shelf_3->clear();
				ui.lineEdit_minamount_3->clear();
			}
		}
	}

	

	selected_articlenumber = "";
	ui.tableView_3->clearSelection();
}

void Lagersystem::change_button_3_clicked() {
	QString articlenumber = ui.lineEdit_articlenumber_3->text();
	QString category = ui.lineEdit_category_3->text();
	QString articlename = ui.lineEdit_articlename_3->text();
	QString amount = ui.lineEdit_amount_3->text();
	QString shelf = ui.lineEdit_shelf_3->text();
	QString minamount = ui.lineEdit_minamount_3->text();



	QString error = logiccontroller.change_lagerbestand_check(db, selected_articlenumber, articlenumber, category, articlename, amount, shelf, minamount);
	if (!error.isEmpty()) {
		QMessageBox::warning(this, "Warnung", error);
	}
	else {
		QMessageBox box(this);
		box.setWindowTitle("Änderung bestätigen");
		box.setText(QString("Möchten Sie den Artikel wirklich ändern?"));
		box.setIcon(QMessageBox::Warning);
		QPushButton* bestaetigen = box.addButton("Bestätigen", QMessageBox::AcceptRole);
		box.addButton("Abbrechen", QMessageBox::RejectRole);
		box.exec();

		if (box.clickedButton() == bestaetigen) {
			QString error = logiccontroller.change_lagerbestand(db, query_model, selected_articlenumber, articlenumber, category, articlename, amount, shelf, minamount);
			if (!error.isEmpty()) {
				QMessageBox::warning(this, "Warnung", error);
			}
			else {
				ui.lineEdit_articlenumber_3->clear();
				ui.lineEdit_category_3->clear();
				ui.lineEdit_articlename_3->clear();
				ui.lineEdit_amount_3->clear();
				ui.lineEdit_shelf_3->clear();
				ui.lineEdit_minamount_3->clear();
			}
		}
	}

	selected_articlenumber = "";
	ui.tableView_3->clearSelection();
}

// Seite 4

void Lagersystem::exit_button_4_clicked(){
	ui.stackedWidget->setCurrentIndex(0);
	query_model->setQuery("SELECT * FROM lagerbestand", db);
}

void Lagersystem::search_button_4_clicked() {
}

// Inits

void Lagersystem::init_page_0() {
	connect(ui.search_button_0, &QPushButton::clicked, this, &Lagersystem::search_button_0_clicked);
	connect(ui.store_button, &QPushButton::clicked, this, &Lagersystem::store_button_clicked);
	connect(ui.store_history_button, &QPushButton::clicked, this, &Lagersystem::store_history_button_clicked);
	connect(ui.change_button, &QPushButton::clicked, this, &Lagersystem::change_button_clicked);
	connect(ui.change_history_button, &QPushButton::clicked, this, &Lagersystem::change_history_button_clicked);
}

void Lagersystem::init_page_1() {
	connect(ui.search_button_1, &QPushButton::clicked, this, &Lagersystem::search_button_1_clicked);
	connect(ui.store_in_button_1, &QPushButton::clicked, this, &Lagersystem::store_in_button_1_clicked);
	connect(ui.store_out_button_1, &QPushButton::clicked, this, &Lagersystem::store_out_button_1_clicked);
	connect(ui.tableView_1, &QTableView::clicked, this, &Lagersystem::tableview_1_clicked);
	connect(ui.EXIT_button_1, &QPushButton::clicked, this, &Lagersystem::exit_button_1_clicked);
}

void Lagersystem::init_page_2() {
	connect(ui.search_button_2, &QPushButton::clicked, this, &Lagersystem::search_button_2_clicked);
	connect(ui.EXIT_button_2, &QPushButton::clicked, this, &Lagersystem::exit_button_2_clicked);
}

void Lagersystem::init_page_3() {
	connect(ui.search_button_3, &QPushButton::clicked, this, &Lagersystem::search_button_3_clicked);
	connect(ui.EXIT_button_3, &QPushButton::clicked, this, &Lagersystem::exit_button_3_clicked);
	connect(ui.delete_button_3, &QPushButton::clicked, this, &Lagersystem::delete_button_3_clicked);
	connect(ui.add_button_3, &QPushButton::clicked, this, &Lagersystem::add_button_3_clicked);
	connect(ui.change_button_3, &QPushButton::clicked, this, &Lagersystem::change_button_3_clicked);
	connect(ui.tableView_3, &QTableView::clicked, this, &Lagersystem::tableview_3_clicked);
}

void Lagersystem::init_page_4() {
	connect(ui.search_button_4, &QPushButton::clicked, this, &Lagersystem::search_button_4_clicked);
	connect(ui.EXIT_button_4, &QPushButton::clicked, this, &Lagersystem::exit_button_4_clicked);
}




void Lagersystem::creating_databases() {
	db = QSqlDatabase::addDatabase("QSQLITE", "lager_db");
    db.setDatabaseName("Lagerdaten");

	if (!db.open()) {
		QMessageBox::critical(this, "Fehler", "Datenbank konnte nicht geöffnet werden!");
	}
    



    QSqlQuery query(db);
	
    query.exec(R"(
        CREATE TABLE IF NOT EXISTS lagerbestand(
	        Artikelnummer int,
			Kategorie varchar(50),
            Bezeichnung varchar(50),
            Menge int,
            Regal varchar(50),
			Mindestmenge int,
            PRIMARY KEY (artikelnummer)
        );
    )");

	query.exec(R"(
        CREATE TABLE IF NOT EXISTS buchungsverlauf(
			Buchungs_ID INTEGER PRIMARY KEY AUTOINCREMENT,
			Zeitpunkt varchar(50),
	        Artikelnummer int,
			Kategorie varchar(50),
            Bezeichnung varchar(50),
            Regal varchar(50),
			Mindestmenge int,
			Buchungsart varchar(50),
			Buchungsmenge int,
            alte_Menge int,
			neue_Menge int
        );
    )");

	query.exec(R"(
        CREATE TABLE IF NOT EXISTS aenderungsverlauf(
			Aenderungs_ID INTEGER PRIMARY KEY AUTOINCREMENT,
			Zeitpunkt varchar(50),

	        alte_Artikelnummer int,
			alte_Kategorie varchar(50),
            alte_Bezeichnung varchar(50),
			alte_Menge int,
            altes_Regal varchar(50),
			alte_Mindestmenge int,

			Aenderungsart varchar(50),
			
			neue_Artikelnummer int,
			neue_Kategorie varchar(50),
            neue_Bezeichnung varchar(50),
			neue_Menge int,
            neues_Regal varchar(50),
			neue_Mindestmenge int

        );
    )");


    query.exec(R"(
        INSERT OR IGNORE INTO lagerbestand VALUES
        (1, "Haushaltsgerät", "Staubsauger", 1, "A1", 10), 
        (2, "Haushaltsgerät", "TV", 1, "A1", 5), 
        (3, "Haushaltsgerät", "Föhn", 1, "B1", 0);
    )");
}