#include <QPlainTextEdit>

#include "valueview.h"

ValueView::ValueView(const QVariant& value, QWidget *parent) :
	KDialog(parent)
{
	setAttribute(Qt::WA_DeleteOnClose, true);

	setButtons(KDialog::Close);

	// TODO support viewing of binary data too
	m_value = value.toString();

	QPlainTextEdit *editor = new QPlainTextEdit(m_value, this);
	editor->setReadOnly(true);

	setMainWidget(editor);
}
