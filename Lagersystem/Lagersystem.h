#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Lagersystem.h"
#include <QtSql/qsqltablemodel.h>
#include "LogicController.h"

class Lagersystem : public QMainWindow
{
    Q_OBJECT

public:
    Lagersystem(QWidget *parent = nullptr);
    ~Lagersystem();

private slots:
    //Allgemein
    void page_changed(int page);
    //Seite 0
    void search_button_0_clicked();
    void store_button_clicked();    
    void store_history_button_clicked();
    void change_button_clicked();
    void change_history_button_clicked();
    //Seite 1
    void search_button_1_clicked();
    void store_in_button_1_clicked();
    void store_out_button_1_clicked();
    void tableview_1_clicked(const QModelIndex& index);
    void exit_button_1_clicked();
    //Seite 2
    void search_button_2_clicked();
    void exit_button_2_clicked();
    //Seite 3
    void search_button_3_clicked();
    void exit_button_3_clicked();
    void delete_button_3_clicked();
    void add_button_3_clicked();
    void change_button_3_clicked();
    void tableview_3_clicked(const QModelIndex& index);
    //Setie 4
    void search_button_4_clicked();
    void exit_button_4_clicked();

private:
    Ui::LagersystemClass ui;
    QSqlQueryModel* query_model;
    QSqlQueryModel* query_model_minamount;
    QString selected_articlenumber;
	QSqlDatabase db;
    //Allgemein
    void creating_databases();
    void init_page_0();
    void init_page_1();
    void init_page_2();
    void init_page_3();
    void init_page_4();

    LogicController logiccontroller;
};

