//
// This file is part of KQueryBrowser.
//
// KQueryBrowser is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// KQueryBrowser is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with KQueryBrowser.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef MYSQLCONNECTION_H
#define MYSQLCONNECTION_H

#include "../serverconnection.h"

namespace db {

/**
 \brief Connection handler for MySQL

 */
class MysqlConnection : public ServerConnection
{
    Q_OBJECT
public:
    MysqlConnection(const QUrl& url, QObject *parent=nullptr);

	bool isCapable(Capability capability) const;

protected:
	QString type() const { return QString("QMYSQL"); }
	int defaultPort() const { return 3306; }

	void doGetDbStructure();
	void doGetDbList();
	void doGetCreateScript(const QString& table);
	void doSwitchDatabase(const QString& database) override;
};

}

#endif // MYSQLCONNECTION_H

