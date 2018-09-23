#include "spinboxaction.h"
std::tuple<const QFont, const QPalette> getSystemFonts() {
    if(QSysInfo::productType() == "osx") {
        QPalette pal;
        QFont font = QFont(".SF NS Text", 13, -5);
        pal.setColor(QPalette::Text, QColor("grey").darker(170));
        pal.setColor(QPalette::Background, QColor("grey").darker(170));
        pal.setColor(QPalette::Foreground, QColor("grey").darker(170));
        return std::tuple<const QFont, const QPalette>(font, pal);
    }
    else return std::tuple<const QFont, const QPalette>();
}
