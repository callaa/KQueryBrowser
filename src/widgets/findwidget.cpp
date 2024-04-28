#include "findwidget.h"

#include <QHBoxLayout>
#include <QToolButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QKeyEvent>
#include <KLocalizedString>

using namespace Qt::StringLiterals;

FindWidget::FindWidget(QWidget *parent)
    : QWidget{parent}
{
    auto *layout = new QHBoxLayout;

    // A button for hiding the finder
    auto closeBtn = new QToolButton(this);
    closeBtn->setIcon(QIcon::fromTheme(u"dialog-close"_s));
    closeBtn->setAutoRaise(true);
    connect(closeBtn, &QToolButton::clicked, this, &FindWidget::stopFinding);
    layout->addWidget(closeBtn);

    // The text input field
    m_input = new QLineEdit(this);
    m_input->setPlaceholderText(i18n("Find..."));
    connect(m_input, &QLineEdit::textChanged, this, &FindWidget::findNext);
    layout->addWidget(m_input);

    // Previous + next buttons
    auto prevBtn = new QToolButton(this);
    prevBtn->setIcon(QIcon::fromTheme(u"go-previous"_s));
    prevBtn->setAutoRaise(true);
    connect(prevBtn, &QToolButton::clicked, this, &FindWidget::findNext);
    layout->addWidget(prevBtn);

    auto nextBtn = new QToolButton(this);
    nextBtn->setIcon(QIcon::fromTheme(u"go-next"_s));
    nextBtn->setAutoRaise(true);
    connect(nextBtn, &QToolButton::clicked, this, &FindWidget::findNext);
    layout->addWidget(nextBtn);

    // Case sensitivity option
    m_caseSensitive = new QCheckBox(i18n("Case sensitive"), this);
    connect(m_caseSensitive, &QCheckBox::toggled, this, &FindWidget::findNext);
    layout->addWidget(m_caseSensitive);

    setLayout(layout);
}

void FindWidget::showFinder()
{
    show();
    m_input->setFocus();
}

void FindWidget::stopFinding()
{
    Q_EMIT findRequested(QString(), true, false);
    hide();
}

void FindWidget::findNext()
{
    Q_EMIT findRequested(m_input->text(), true, m_caseSensitive->isChecked());
}

void FindWidget::findPrev()
{
    Q_EMIT findRequested(m_input->text(), false, m_caseSensitive->isChecked());
}

void FindWidget::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Escape) {
        e->accept();
        stopFinding();
    } else {
        QWidget::keyPressEvent(e);
    }
}
