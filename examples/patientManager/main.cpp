//=============================================================================================================
/**
* @file     main.cpp
* @author   Lorenz Esch <Lorenz.Esch@tu-ilmenau.de>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>
* @version  1.0
* @date     June, 2015
*
* @section  LICENSE
*
* Copyright (C) 2015, Lorenz Esch and Matti Hamalainen. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that
* the following conditions are met:
*     * Redistributions of source code must retain the above copyright notice, this list of conditions and the
*       following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
*       the following disclaimer in the documentation and/or other materials provided with the distribution.
*     * Neither the name of MNE-CPP authors nor the names of its contributors may be used
*       to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*
* @brief    Example for making a patient information manager
*
*/

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================


//*************************************************************************************************************
//=============================================================================================================
// QT INCLUDES
//=============================================================================================================

#include <QCoreApplication>
#include <QtSql>
#include <QDebug>
#include <QtWidgets>


//*************************************************************************************************************
//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================


//*************************************************************************************************************
//=============================================================================================================
// MAIN
//=============================================================================================================
QSqlDatabase db;

void initializeModel(QSqlRelationalTableModel *model)
{
    model->setTable("Patients");

    model->setEditStrategy(QSqlTableModel::OnFieldChange);
//    model->setRelation(2, QSqlRelation("city", "id", "name"));

//    model->setRelation(3, QSqlRelation("country", "id", "name"));

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Gender"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Birthdate"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Name"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Handedness"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Comments"));

    model->select();
}

QTableView *createView(const QString &title, QSqlTableModel *model)
{
    QTableView *view = new QTableView;
    view->setModel(model);
    view->setItemDelegate(new QSqlRelationalDelegate(view));
    view->setWindowTitle(title);
    return view;
}

void createRelationalTables()
{
//    QSqlQuery query;
//    query.exec("create table employee(id int primary key, name varchar(20), city int, country int)");
//    query.exec("insert into employee values(1, 'Espen', 5000, 47)");
//    query.exec("insert into employee values(2, 'Harald', 80000, 49)");
//    query.exec("insert into employee values(3, 'Sam', 100, 1)");

//    query.exec("create table city(id int, name varchar(20))");
//    query.exec("insert into city values(100, 'San Jose')");
//    query.exec("insert into city values(5000, 'Oslo')");
//    query.exec("insert into city values(80000, 'Munich')");

//    query.exec("create table country(id int, name varchar(20))");
//    query.exec("insert into country values(1, 'USA')");
//    query.exec("insert into country values(47, 'Norway')");
//    query.exec("insert into country values(49, 'Germany')");
}

bool createConnection()
{
    db = QSqlDatabase::addDatabase("QODBC");

    QString serverName = "LORENZ-PC";
    QString dbName = "Patients";
    QString dsn = QString("DRIVER={SQL Server};SERVER=%1;DATABASE=%2;UID=superadmin;PWD=lollo1").arg(serverName).arg(dbName);

    db.setDatabaseName(dsn);

    if(db.open()) {
        qDebug()<<"database opened";
    } else {
        qDebug()<< "Error: " <<db.lastError().text();
        return false;
    }

    qDebug()<<db.port();

    return true;
}


//=============================================================================================================
/**
* The function main marks the entry point of the program.
* By default, main has the storage class extern.
*
* @param [in] argc (argument count) is an integer that indicates how many arguments were entered on the command line when the program was started.
* @param [in] argv (argument vector) is an array of pointers to arrays of character objects. The array objects are null-terminated strings, representing the arguments that were entered on the command line when the program was started.
* @return the value that was set to exit() (which is 0 if exit() is called via quit()).
*/
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    if (!createConnection())
        return 1;

    createRelationalTables();

    QSqlRelationalTableModel model;

    initializeModel(&model);

    QTableView *view = createView(QObject::tr("Relational Table Model"), &model);
    view->show();

    return app.exec();

//    QCoreApplication a(argc, argv);

//    qDebug() << "Drivers: "<<QSqlDatabase::drivers();

//    QString serverName = "LORENZ-PC";
//    QString dbName = "Patients";

//    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");

//    QString dsn = QString("DRIVER={SQL Server};SERVER=%1;DATABASE=%2;UID=superadmin;PWD=lollo1").arg(serverName).arg(dbName);

//    db.setDatabaseName(dsn);

//    if(db.open()) {
//        qDebug()<<"database opened";

//        QString sQuery = "INSERT INTO [Patients].[dbo].[Patients] ([ID],[Name])VALUES(:ID,:name)";
//        //QString sQuery = "SELECT TOP 1000 [Name],[NumberPatients],[Comments]FROM [Patients].[dbo].[Groups]";

//        int ID = 123455;
//        QSqlQuery qry;
//        qry.prepare(sQuery);
//        qry.bindValue(":ID",ID);
//        qry.bindValue(":name","Heather");

//        if(qry.exec()) {
//            while(qry.next()){
//                qDebug()<<qry.value(0).toString()<<qry.value(1).toString()<<qry.value(2).toString();
//            }
//        } else {
//            qDebug()<< "Error: " <<db.lastError().text();
//        }

//        qDebug()<<"Closing database";

//        db.close();
//    } else
//        qDebug()<< "Error: " <<db.lastError().text();

//    return a.exec();
}
