#ifndef DAO_H
#define DAO_H

#include <QSqlDatabase>
#include <QVariant>


class Dao
{
public:
    Dao();
    ~Dao();
    QHash<QString, QString> singleRow(QString table,QString where="");
    bool updateRow(QString table,QStringList values,QString where);
    bool deleteRow(QString table,QString where);
    QList< QHash<QString, QString> > listRow(QString table,QString select = "*",QString where = "");
    bool insertRow(QString table,QHash<QString, QString> fieldsVals);
    QHash<QString, QString> boolVal(QString select);
    bool replaceRow(QString table,QHash<QString, QString> fieldsVals);

private:
    QSqlDatabase db;
    void close();

};

#endif // DAO_H
