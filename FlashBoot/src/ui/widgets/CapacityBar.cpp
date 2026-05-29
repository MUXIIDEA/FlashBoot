#include "CapacityBar.h"

CapacityBar::CapacityBar(QWidget* parent) : QProgressBar(parent) {
    setRange(0, 100);
    setTextVisible(false);
}

void CapacityBar::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = this->rect().adjusted(1, 1, -1, -1);
    int radius = rect.height() / 2;

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(230, 230, 230));
    painter.drawRoundedRect(rect, radius, radius);

    int value = this->value();
    QColor barColor;
    if (value < 20) barColor = m_highColor;
    else if (value < 50) barColor = m_mediumColor;
    else barColor = m_lowColor;

    int width = rect.width() * value / 100;
    if (width > 0) {
        QRect progressRect(rect.left(), rect.top(), width, rect.height());
        painter.setBrush(barColor);
        painter.drawRoundedRect(progressRect, radius, radius);
    }
}