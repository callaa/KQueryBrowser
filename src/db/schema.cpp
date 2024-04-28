#include "schema.h"

using namespace Qt::StringLiterals;

namespace dbschema {

ForeignKeyAction foreignKeyActionFromString(const QString &name) {
    if(name == u"NO ACTION"_s)
        return ForeignKeyAction::NoAction;
    if(name == u"CASCADE"_s)
        return ForeignKeyAction::Cascade;
    if(name == u"SET NULL"_s)
        return ForeignKeyAction::SetNull;
    if(name == u"SET DEFAULT"_s)
        return ForeignKeyAction::SetDefault;
    if(name == u"NO ACTION"_s)
        return ForeignKeyAction::NoAction;
    if(name == u"RESTRICT"_s)
        return ForeignKeyAction::Restrict;
    return ForeignKeyAction::Unknown;
}

QString foreignKeyActionToString(ForeignKeyAction action)
{
    switch(action) {
    case ForeignKeyAction::Unknown: break;
    case ForeignKeyAction::NoAction: return u"NO ACTION"_s;
    case ForeignKeyAction::Cascade: return u"CASCADE"_s;
    case ForeignKeyAction::SetNull: return u"SET NULL"_s;
    case ForeignKeyAction::SetDefault: return u"SET DEFAULT"_s;
    case ForeignKeyAction::Restrict: return u"RESTRICT"_s;
    }
    return u"???"_s;
}

QString Column::foreignKeyString() const {
    return u"REFERENCES %1(%2) ON DELETE %3 ON UPDATE %4"_s.arg(
        foreignKeyToTable,
        foreignKeyToColumn,
        foreignKeyActionToString(onDelete),
        foreignKeyActionToString(onUpdate)
        );
}

}
