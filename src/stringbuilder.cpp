#include "stringbuilder.h"

StringBuilder::StringBuilder()
	: m_len(0)
{
}

QString StringBuilder::toString() const
{
	QString str;
	str.reserve(m_len);
	foreach(const QString& s, m_list) {
		str.append(s);
	}
	return str;
}

StringBuilder &StringBuilder::operator<<(const QString& str)
{
	m_list << str;
	m_len += str.length();
	return *this;
}

