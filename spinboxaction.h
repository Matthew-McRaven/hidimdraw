#ifndef SPINBOXACTION_H
#define SPINBOXACTION_H
#include <QWidget>
#include <QWidgetAction>
#include <QSpinBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QApplication>
#include <tuple>
std::tuple<const QFont, const QPalette> getSystemFonts();
class SpinBoxAction : public QWidgetAction {
public:
    SpinBoxAction (const QString& title, QObject *parent = nullptr) :
      QWidgetAction (parent) {
        pLayout = new QHBoxLayout();
        pWidget = new QWidget (nullptr);
        pLabel = new QLabel (title);
        pLayout->addWidget (pLabel);
        pSpinBox = new QSpinBox(nullptr);
        pLayout->addWidget (pSpinBox);
        pWidget->setLayout (pLayout);
        std::tuple<const QFont, const QPalette> x = getSystemFonts();
        pLabel->setPalette(std::get<1>(x));
        pLabel->setFont(std::get<0>(x));
        setDefaultWidget(pWidget);
    }

    QSpinBox * spinBox () {
        return pSpinBox;
    }
    void setFont(const QFont &font) {
        pSpinBox->setFont(font);
        pLabel->setFont(font);
    }

private:
    QWidget* pWidget;
    QSpinBox * pSpinBox;
    QHBoxLayout* pLayout;
    QLabel* pLabel;
};
#endif // SPINBOXACTION_H
