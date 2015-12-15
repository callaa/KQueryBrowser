#ifndef VALUEVIEW_H
#define VALUEVIEW_H

#include <QDialog>
#include <QVariant>

class QPlainTextEdit;

namespace Ui {
	class ValueViewWidget;
}

namespace Okteta {
	class AbstractByteArrayView;
}

/**
  \brief A dialog for viewing result values that were too large to show in the table.
  */
class ValueView : public QDialog
{
    Q_OBJECT
public:
	explicit ValueView(const QVariant &value, QWidget *parent = 0);

signals:

protected slots:
	void saveValue();
	void decodeBase64(bool decode);
	void showTextView();
	void showHexView();

private:
	void updateView();

	QVariant m_value;
	QVariant m_decoded;
	Ui::ValueViewWidget *m_ui;

	QPlainTextEdit *m_textview;
	Okteta::AbstractByteArrayView *m_hexview;
	bool m_dirtytext, m_dirtyhex;
};

#endif // VALUEVIEW_H
