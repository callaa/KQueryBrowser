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

#include "sqllineedit.h"

#include <QKeyEvent>

SqlLineEdit::SqlLineEdit(QWidget *parent) :
    KLineEdit(parent), m_historypos(0)
{
}

void SqlLineEdit::pushHistory(const QString &text)
{
    // Disallow consecutive duplicates
    if(!m_history.isEmpty() && m_history.last() == text)
        return;

    // Add to history list while limiting its size
    m_history.append(text);
    if(m_history.count() > 100) /* TODO configurable limit */
        m_history.removeFirst();

    // History position is not updated if it's not at the end.
    // This is so that if the user is browsing through the history and executes
    // a query from somewhere else than the query box, (e.g. from the table list widget)
    // they won't lose their spot.
    if(m_historypos==m_history.count()-1)
        ++m_historypos;
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
            pushHistory(txt);
            m_historypos = m_history.count();
            setText(QString());
        }
        Q_EMIT returnPressed(txt);
    } else {
        KLineEdit::keyPressEvent(event);
    }
}

bool SqlLineEdit::isEmpty() const
{
    return text().trimmed().isEmpty();
}
