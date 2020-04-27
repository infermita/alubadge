#include "dao.h"
#include <QDir>
#include <iostream>
#include <typeinfo>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

Dao::Dao()
{
    QString path = "/tmp/alubadge.db";
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);
    if(db.open()){
        //qDebug() << "Open database";
        QSqlQuery query(db);
        bool res;
        QString sql = "PRAGMA synchronous = OFF;";
        res = query.exec(sql);
        //if(res)
        //    qDebug() << "PRAGMA synchronous = OFF OK";

        sql = "PRAGMA journal_mode = MEMORY;";
        res = query.exec(sql);

        sql = "SELECT * FROM workers;";
        res = query.exec(sql);

        QSqlRecord record = query.record();

        if(record.count()){

            qDebug() << "db esiste";

        }else{

            sql = "CREATE TABLE workers (id INT PRIMARY KEY NOT NULL,cardkey char(30),name char(60),autocomplete char(60));";
            res = query.exec(sql);
            sql = "CREATE TABLE `giorni` (`id`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,`id_commesse`	INTEGER NOT NULL DEFAULT 5,`id_worker`	INTEGER NOT NULL DEFAULT 0,`data`	NUMERIC,`indt`	NUMERIC,`outdt`	NUMERIC);";
            res = query.exec(sql);

            qDebug() << "db non esiste";

        }
        //if(res)
        //    qDebug() << "PRAGMA journal_mode = MEMORY OK";
    }else{
        qDebug() << "Error Open database";
    }

}
Dao::~Dao(){

    close();

}
QHash<QString, QString> Dao::singleRow(QString table,QString where)
{

    QHash<QString,QString> tmpHash;

    QSqlQuery query(db);

    QString sql = "SELECT * FROM "+table;

    if(where.length())
        sql += " WHERE "+where;

    //qDebug() << sql;
    query.exec(sql);
    QSqlRecord record = query.record();
    int fieldcount = record.count();


    for( int r=0; query.next(); r++ ){
        for (int i = 0; i < fieldcount; i++) {

            tmpHash.insert(record.fieldName(i),query.value(i).toString());

        }
    }


    //close();
    return tmpHash;

}
QList< QHash<QString, QString> > Dao::listRow(QString table,QString select,QString where){

    QList< QHash<QString, QString> > list;
    QSqlRecord record;
    int fieldcount;
    //QSqlRecord record = db.record(table);

    if(where.length())
        where = " WHERE "+where;

    QSqlQuery query(db);
    QString queryS = "SELECT "+select+" FROM "+table+" "+where+";";
    //qDebug() << "Query" << queryS;
    query.exec(queryS);

    record = query.record();
    fieldcount = record.count();

    while ( query.next() ) {


        QHash<QString,QString> tmpHash;

        for (int i = 0; i < fieldcount; i++) {

            tmpHash.insert(record.fieldName(i),query.value(i).toString());

        }

        list.append(tmpHash);


    }
    //close();
    return list;

}
bool Dao::updateRow(QString table,QStringList values,QString where){

    bool ret = false;
    QSqlQuery query(db);

    QString sql = "UPDATE "+table+" SET "+values.join(",");

    if(where!=NULL)
        sql += " WHERE "+where;

    qDebug() << sql;

    if(query.exec(sql))
        ret = true;

    //close();

    return ret;

}
bool Dao::insertRow(QString table,QHash<QString, QString> fieldsVals){

    bool ret = false;
    int i;
    QSqlQuery query(db);
    QString val;

    QStringList fields,values;
    foreach (QString k, fieldsVals.keys()) {

        fields << k;
        val = fieldsVals.value(k);
        val.replace("'","''");
        values << val;

    }
    QString sql = "INSERT INTO "+table+" ("+fields.join(",")+") VALUES('"+values.join("','")+"')";
    qDebug() << sql;

    if(query.exec(sql))
        ret = true;


    //close();

    return ret;

}
QHash<QString, QString> Dao::boolVal(QString select){

    QHash<QString,QString> tmpHash;

    QSqlQuery query(db);

    QString sql = "SELECT "+select;

    //qDebug() << sql;
    query.exec(sql);
    QSqlRecord record = query.record();
    int fieldcount = record.count();


    for( int r=0; query.next(); r++ ){
        for (int i = 0; i < fieldcount; i++) {

            tmpHash.insert(record.fieldName(i),query.value(i).toString());

        }
    }


    return tmpHash;

}
bool Dao::replaceRow(QString table,QHash<QString, QString> fieldsVals){

    bool ret = false;
    int i;
    QSqlQuery query(db);

    QStringList fields,values;
    foreach (QString k, fieldsVals.keys()) {

        fields << k;
        values << fieldsVals.value(k);

    }
    QString sql = "REPLACE INTO "+table+" ("+fields.join(",")+") VALUES('"+values.join("','")+"');";
    //qDebug() << sql;

    if(query.exec(sql))
        ret = true;

    return ret;

}

bool Dao::deleteRow(QString table, QString where){

    bool ret = false;
    QSqlQuery query(db);

    QString sql = "DELETE FROM "+table+" WHERE "+where;
    //qDebug() << sql;

    if(query.exec(sql))
        ret = true;


    //close();

    return ret;

}

void Dao::close()
{

    QString connection;
    connection = db.connectionName();
    db.close();
    db = QSqlDatabase();
    db.removeDatabase(connection);


}
