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
#ifndef SQLLINEEDIT_H
#define SQLLINEEDIT_H

#include <QStringList>
#include <KLineEdit>

/**
  \brief A specialized line edit widget for SQL entry, with history.
  */
class SqlLineEdit : public KLineEdit
{
    Q_OBJECT
public:
    explicit SqlLineEdit(QWidget *parent=nullptr);

    bool isEmpty() const;

    //! Push text to history
    void pushHistory(const QString& text);

Q_SIGNALS:
    void returnPressed(const QString &text);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    QStringList m_history;
    QString m_current;
    int m_historypos;
};

#endif
