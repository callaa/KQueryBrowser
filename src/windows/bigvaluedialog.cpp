#include "bigvaluedialog.h"
#include "utils/jsonnmodel.h"

#include "ui_bigvalue.h"

#include <QFileDialog>
#include <QSaveFile>
#include <QMessageBox>
#include <QJsonDocument>
#include <KLocalizedString>

using namespace Qt::StringLiterals;

static const int DECODE_RAW = 0;
static const int DECODE_B64 = 1;

static const int PAGE_TEXT = 0; // Also used for hex dump
static const int PAGE_JSON = 1;
static const int PAGE_IMAGE = 2;

BigValueDialog::BigValueDialog(QWidget *parent)
    : QDialog(parent), m_ui(new Ui_bigValueDialog)
{
    m_ui->setupUi(this);

    connect(m_ui->decodeMode, &QComboBox::currentIndexChanged, this, &BigValueDialog::decodeText);
    connect(m_ui->viewText, &QRadioButton::clicked, this, &BigValueDialog::updateViews);
    connect(m_ui->viewHex, &QRadioButton::clicked, this, &BigValueDialog::updateViews);
    connect(m_ui->viewJson, &QRadioButton::clicked, this, &BigValueDialog::updateViews);
    connect(m_ui->viewImage, &QRadioButton::clicked, this, &BigValueDialog::updateViews);
    connect(m_ui->saveButton, &QPushButton::clicked, this, &BigValueDialog::exportValue);
}

BigValueDialog::~BigValueDialog()
{
    delete m_ui;
}

void BigValueDialog::setTextValue(const QString &value)
{
    m_textValue = value;
    m_binaryValue = QByteArray();
    m_ui->decodeMode->setEnabled(true);
    decodeText();
}

void BigValueDialog::setBinaryValue(const QByteArray &value)
{
    m_textValue = QString::fromLatin1(m_binaryValue);
    m_binaryValue = value;
    m_ui->decodeMode->setEnabled(false);
    m_ui->viewHex->setChecked(true);
    m_ui->treeWidget->clear();

    updateViews();
}

void BigValueDialog::decodeText()
{
    switch(m_ui->decodeMode->currentIndex()) {
    case DECODE_RAW:
        m_binaryValue = m_textValue.toUtf8();
        break;
    case DECODE_B64:
        m_binaryValue = QByteArray::fromBase64(m_textValue.toLatin1());
        break;
    }

    m_ui->treeWidget->clear();
    updateViews();
}

void BigValueDialog::updateViews()
{
    if(m_ui->viewText->isChecked()) {
        m_ui->stackedWidget->setCurrentIndex(PAGE_TEXT);
        if(m_ui->decodeMode->currentIndex() == DECODE_B64) {
            // There might be a string inside
            m_ui->plainTextEdit->setPlainText(QString::fromUtf8(m_binaryValue));
        } else {
            m_ui->plainTextEdit->setPlainText(m_textValue);
        }

    } else if(m_ui->viewHex->isChecked()) {
        // Quick & dirty hex dump. We could ues Okteta again once it's ported to KDE 6
        static const QChar HEX[] = {u'0', u'1', u'2', u'3', u'4', u'5', u'6', u'7', u'8', u'9', u'A', u'B', u'C', u'D', u'E', u'F'};

        QString hex;
        hex.reserve(m_binaryValue.length() * 3 + (m_binaryValue.length() / 16 * (7 + 1 + 16)));
        hex.append(u"0000: ");

        for(int i=0,col=0;i<m_binaryValue.length();++i) {
            const char byte = m_binaryValue.at(i);
            hex.append(HEX[(byte & 0xf0) >> 4]);
            hex.append(HEX[byte & 0x0f]);
            hex.append(u' ');
            if(col == 15) {
                col = 0;
                hex.append(u' ');
                for(int j=i-15;j<=i;++j) {
                    const QChar c = QChar::fromLatin1(m_binaryValue.at(j));
                    hex.append(c.isPrint() ? c : u'.');
                }
                hex.append(u"\n%1: "_s.arg(i+1, 4, 16, QChar(u'0')));
            } else {
                ++col;
            }
        }

        m_ui->stackedWidget->setCurrentIndex(PAGE_TEXT);
        m_ui->plainTextEdit->setPlainText(hex);

    } else if(m_ui->viewJson->isChecked()) {
        m_ui->stackedWidget->setCurrentIndex(PAGE_JSON);

        if(m_ui->treeWidget->topLevelItemCount() == 0) {
            const auto jsonDoc = QJsonDocument::fromJson(m_binaryValue);
            if(jsonDoc.isNull()) {
                QTreeWidgetItem *errorItem = new QTreeWidgetItem;
                errorItem->setText(1, u"[invalid json]"_s);
                m_ui->treeWidget->insertTopLevelItem(0, errorItem);

            } else {
                auto items = makeJsonModel(jsonDoc);
                m_ui->treeWidget->insertTopLevelItems(0, items);
            }
        }

    } else {
        QImage img = QImage::fromData(m_binaryValue);

        if(img.isNull()) {
            m_ui->image->setText(i18n("This does not appear to be an image"));
        } else {
            m_ui->image->setPixmap(QPixmap::fromImage(img));
        }
        m_ui->stackedWidget->setCurrentIndex(PAGE_IMAGE);
    }
}

void BigValueDialog::exportValue()
{
    const QString name = QFileDialog::getSaveFileName(this, QString(), QString(), u"All files (*)"_s);
    if(!name.isEmpty()) {
        QSaveFile save(name);
        if(!save.open(QIODevice::WriteOnly)) {
            QMessageBox::warning(nullptr, i18n("New Connection"), save.errorString());
            return;
        }

        if(m_ui->viewText->isChecked()) {
            save.write(m_textValue.toUtf8());
        } else {
            save.write(m_binaryValue);
        }

        save.commit();
    }
}
