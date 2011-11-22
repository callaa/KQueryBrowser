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
    explicit SqlLineEdit(QWidget *parent = 0);

	bool isEmpty() const;

signals:
	void returnPressed(const QString &text);

public slots:

protected:
	void keyPressEvent(QKeyEvent *event);

private:
	QStringList m_history;
	QString m_current;
	int m_historypos;
};

#endif // SQLLINEEDIT_H
