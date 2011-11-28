#ifndef VALUEVIEW_H
#define VALUEVIEW_H

#include <KDialog>
#include <QVariant>


/**
  \brief A dialog for viewing result values that were too large to show in the table.
  */
class ValueView : public KDialog
{
    Q_OBJECT
public:
	explicit ValueView(const QVariant &value, QWidget *parent = 0);

signals:

public slots:

private:
	QString m_value;
};

#endif // VALUEVIEW_H
