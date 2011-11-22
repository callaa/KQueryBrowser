#include <QKeyEvent>

#include "sqllineedit.h"

SqlLineEdit::SqlLineEdit(QWidget *parent) :
	KLineEdit(parent), m_historypos(0)
{
}


void SqlLineEdit::keyPressEvent(QKeyEvent *event)
{
	if(event->key() == Qt::Key_Up) {
		if(m_historypos>0) {
			if(m_historypos==m_history.count())
				m_current = text();
			--m_historypos;
			setText(m_history[m_historypos]);
		}
	} else if(event->key() == Qt::Key_Down) {
		if(m_historypos<m_history.count()-1) {
			++m_historypos;
			setText(m_history[m_historypos]);
		} else if(m_historypos==m_history.count()-1) {
			++m_historypos;
			setText(m_current);
		}
	} else if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
		QString txt = text().trimmed();
		if(!txt.isEmpty()) {
			if(m_history.isEmpty() || m_history.last() != txt) {
				m_history.append(txt);
				if(m_history.count() > 100) /* TODO configurable limit */
					m_history.removeFirst();
			}
			m_historypos = m_history.count();
			setText(QString());
			emit returnPressed(txt);
		}
	} else {
		KLineEdit::keyPressEvent(event);
	}
}

bool SqlLineEdit::isEmpty() const
{
	return text().trimmed().isEmpty();
}
