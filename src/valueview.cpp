#include <cstdio>

#include <QPlainTextEdit>
#include <QLabel>

#include <KMessageBox>
#include <KFileDialog>
#include <KSaveFile>

#include <khexedit/byteseditinterface.h>

#include "valueview.h"
#include "ui_valueviewwidget.h"

ValueView::ValueView(const QVariant& value, QWidget *parent) :
	KDialog(parent), m_value(value), m_decoded(value), m_ui(new Ui::ValueViewWidget), m_dirtytext(true), m_dirtyhex(true)
{
	// Initialize dialog
	setAttribute(Qt::WA_DeleteOnClose, true);

	setButtons(KDialog::Close | KDialog::User1);
	setButtonText(KDialog::User1, tr("Save"));
	setButtonIcon(KDialog::User1, KIcon("document-save"));

	connect(this, SIGNAL(user1Clicked()), this, SLOT(saveValue()));

	// Create UI
	QWidget *mainwidget = new QWidget(this);
	m_ui->setupUi(mainwidget);
	setMainWidget(mainwidget);

	m_ui->modeText->setChecked(true);

	connect(m_ui->decodeb64, SIGNAL(toggled(bool)),
			this, SLOT(decodeBase64(bool)));

	connect(m_ui->modeText, SIGNAL(clicked(bool)),
			this, SLOT(showTextView()));
	connect(m_ui->modeHex, SIGNAL(clicked(bool)),
			this, SLOT(showHexView()));

	//  Text view
	m_textview = new QPlainTextEdit(this);
	m_textview->setReadOnly(true);
	m_ui->stackedWidget->addWidget(m_textview);

	// Hex view
	m_hexview = KHE::createBytesEditWidget(this);
	if(!m_hexview) {
		m_ui->stackedWidget->addWidget(new QLabel(tr("No hex editor available!"), this));
	} else {
		KHE::BytesEditInterface *edit = KHE::bytesEditInterface(m_hexview);
		Q_ASSERT(edit);

		edit->setReadOnly(true);
		edit->setAutoDelete(true);

		m_ui->stackedWidget->addWidget(m_hexview);
	}

	updateView();
}

void ValueView::saveValue()
{
	QString filename = KFileDialog::getSaveFileName();
	if(!filename.isEmpty()) {
		KSaveFile file(filename);
		if(!file.open()) {
			KMessageBox::error(this, file.errorString());
		} else {
			if(m_decoded.type() == QVariant::ByteArray)
				file.write(m_decoded.toByteArray());
			else
				file.write(m_decoded.toString().toUtf8());
			if(!file.finalize()) {
				KMessageBox::error(this, file.errorString());
			}

			file.close();
		}
	}
}

void ValueView::decodeBase64(bool decode)
{
	if(decode) {
		m_decoded = QByteArray::fromBase64(m_value.toByteArray());
	} else {
		m_decoded = m_value;
	}
	m_dirtytext = true;
	m_dirtyhex = true;
	updateView();
}

void ValueView::updateView()
{
	if(m_ui->stackedWidget->currentIndex()==0) {
		m_textview->document()->setPlainText(m_decoded.toString());
		m_dirtytext = false;
	} else {
		KHE::BytesEditInterface *hex = KHE::bytesEditInterface(m_hexview);
		if(hex) {
			QByteArray data = m_decoded.toByteArray();
			char *cdata = new char[data.length()];
			memcpy(cdata, data.constData(), data.length());
			hex->setData(cdata, data.length());
		}
		m_dirtyhex = false;
	}
}

void ValueView::showTextView()
{
	m_ui->stackedWidget->setCurrentIndex(0);
	if(m_dirtytext)
		updateView();
}

void ValueView::showHexView()
{
	m_ui->stackedWidget->setCurrentIndex(1);
	if(m_dirtyhex)
		updateView();
}
