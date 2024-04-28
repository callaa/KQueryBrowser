#ifndef BIGVALUEDIALOG_H
#define BIGVALUEDIALOG_H

#include <QDialog>

class Ui_bigValueDialog;
class QStandardItemModel;

class BigValueDialog : public QDialog
{
    Q_OBJECT
public:
    BigValueDialog(QWidget *parent);
    ~BigValueDialog();

    /**
     * Set a big text value.
     *
     * A text value can be decoded into a binary
     */
    void setTextValue(const QString &value);

    /**
     * Set a big binary value
     *
     * Since this is already in binary format, decoding options will not be available
     */
    void setBinaryValue(const QByteArray &value);

private Q_SLOTS:
    void decodeText();
    void exportValue();

private:
    void updateViews();

    Ui_bigValueDialog *m_ui;

    QString m_textValue;
    QByteArray m_binaryValue;
};

#endif // BIGVALUEDIALOG_H
