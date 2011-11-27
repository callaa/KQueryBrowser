#ifndef STRINGBUILDER_H
#define STRINGBUILDER_H

#include <QStringList>

class StringBuilder {
public:
	StringBuilder();

	QString toString() const;

	StringBuilder& operator<<(const QString& str);
private:
	QStringList m_list;
	int m_len;
};

#endif

