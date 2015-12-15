#ifndef DATABASELISTWIDGET_H
#define DATABASELISTWIDGET_H

#include <QDockWidget>

class QListWidget;
class QStringList;

/**
 * \brief A dock widget that lists databases available to the logged in user
 */
class DatabaseListWidget : public QDockWidget
{
    Q_OBJECT
public:
	explicit DatabaseListWidget(bool canswitch, QWidget *parent = 0);

signals:
	/**
	 * \brief Request new database list
	 */
	void refresh();

	/**
	 * \brief Request opening of new connection to the named database
	 * \param database the name of the database
	 */
	void newConnection(const QString& database);


	/**
	 * \brief Request switching of active database
	 * \param database the name of the database
	 */
	void switchDatabase(const QString& database);

public slots:
	/**
	 * \brief Update database list
	 * \param databases new list
	 * \param current the currently selected database
	 */
	void refreshList(const QStringList& databases, const QString& current);

protected slots:
	void customContextMenu(const QPoint& point);

private:
	QListWidget *m_view;
	bool m_canswitch;
};

#endif // DATABASELISTWIDGET_H
