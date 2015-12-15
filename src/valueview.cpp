#include "valueview.h"
#include "ui_valueviewwidget.h"

#include <cstdio>

#include <QPlainTextEdit>
#include <QFileDialog>
#include <QSaveFile>
#include <QLabel>

#include <KMessageBox>

#include <Okteta/ByteArrayColumnView>
#include <Okteta/ByteArrayModel>

ValueView::ValueView(const QVariant& value, QWidget *parent) :
	QDialog(parent), m_value(value), m_decoded(value), m_ui(new Ui::ValueViewWidget), m_dirtytext(true), m_dirtyhex(true)
{
	// Initialize dialog
	setAttribute(Qt::WA_DeleteOnClose, true);

	// Create UI
	m_ui->setupUi(this);

	m_ui->modeText->setChecked(true);

	connect(m_ui->buttonBox, &QDialogButtonBox::clicked, [this](QAbstractButton *b) {
		if(m_ui->buttonBox->standardButton(b) == QDialogButtonBox::Save)
			saveValue();
	});

	connect(m_ui->decodeb64, &QAbstractButton::toggled, this, &ValueView::decodeBase64);
	connect(m_ui->modeText, &QAbstractButton::clicked, this, &ValueView::showTextView);
	connect(m_ui->modeHex, &QAbstractButton::clicked, this, &ValueView::showHexView);

	//  Text view
	m_textview = new QPlainTextEdit(this);
	m_textview->setReadOnly(true);
	m_ui->stackedWidget->addWidget(m_textview);

	// Hex view
	m_hexview = new Okteta::ByteArrayColumnView(this);
	m_hexview->setReadOnly(true);
	
	Okteta::ByteArrayModel *hexmodel = new Okteta::ByteArrayModel();
	hexmodel->setAutoDelete(true);
	m_hexview->setByteArrayModel(hexmodel);
	m_ui->stackedWidget->addWidget(m_hexview);

	updateView();
}

void ValueView::saveValue()
{
	QString filename = QFileDialog::getSaveFileName();
	if(!filename.isEmpty()) {
		QSaveFile file(filename);
		if(!file.open(QSaveFile::WriteOnly)) {
			KMessageBox::error(this, file.errorString());
		} else {
			if(m_decoded.type() == QVariant::ByteArray)
				file.write(m_decoded.toByteArray());
			else
				file.write(m_decoded.toString().toUtf8());

			if(!file.commit()) {
				KMessageBox::error(this, file.errorString());
			}
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
		QByteArray data = m_decoded.toByteArray();
		Okteta::Byte *bytes = new Okteta::Byte[data.length()];
		memcpy(bytes, data.constData(), data.length());
		static_cast<Okteta::ByteArrayModel*>(m_hexview->byteArrayModel())->setData(bytes, data.length());
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
