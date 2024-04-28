#ifndef SCHEMA_H
#define SCHEMA_H

#include <QList>

namespace dbschema {

enum class ForeignKeyAction {
    Unknown,
    NoAction,
    Cascade,
    SetNull,
    SetDefault,
    Restrict,
};

QString foreignKeyActionToString(ForeignKeyAction action);
ForeignKeyAction foreignKeyActionFromString(const QString &action);

struct Column {
    QString name;
    QString type;
    bool isPrimaryKey; // is this a primary key, or part of a composite primary key
    bool isUnique; // does this column have a UNIQUE constraint
    bool notNull;

    // These are set if this column is a foreign key
    QString foreignKeyToTable;
    QString foreignKeyToColumn;
    ForeignKeyAction onUpdate;
    ForeignKeyAction onDelete;
    QString foreignKeyString() const;
};

struct Table {
    enum class Type { Table, View, SystemTable, Virtual };

    QString name;
    QList<Column> columns;
    Type type;
};

struct Schema {
    QString name;
    QList<Table> tables;
};

struct Database {
    QList<Schema> schemas;

    // If false, schemas contains only unnamed one entity
    bool hasSchemas;
};
}

#endif // SCHEMA_H
