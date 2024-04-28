#ifndef FINDWIDGET_H
#define FINDWIDGET_H

#include <QWidget>

class QLineEdit;
class QCheckBox;

class FindWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FindWidget(QWidget *parent = nullptr);

    /// Show and focus the finder
    void showFinder();

Q_SIGNALS:
    void findRequested(const QString &text, bool next, bool caseSensitive);

public Q_SLOTS:
    void findNext();
    void findPrev();
    void stopFinding();

protected:
    void keyPressEvent(QKeyEvent *e) override;

private:
    QLineEdit *m_input;
    QCheckBox *m_caseSensitive;
};

#endif // FINDWIDGET_H
