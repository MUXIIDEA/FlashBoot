#ifndef CAPACITYBAR_H
#define CAPACITYBAR_H

#include "pch.h"

class CapacityBar : public QProgressBar {
    Q_OBJECT
    Q_PROPERTY(QColor lowColor READ lowColor WRITE setLowColor)
    Q_PROPERTY(QColor mediumColor READ mediumColor WRITE setMediumColor)
    Q_PROPERTY(QColor highColor READ highColor WRITE setHighColor)

public:
    explicit CapacityBar(QWidget* parent = nullptr);

    QColor lowColor() const { return m_lowColor; }
    QColor mediumColor() const { return m_mediumColor; }
    QColor highColor() const { return m_highColor; }

    void setLowColor(const QColor& c) { m_lowColor = c; update(); }
    void setMediumColor(const QColor& c) { m_mediumColor = c; update(); }
    void setHighColor(const QColor& c) { m_highColor = c; update(); }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QColor m_lowColor = QColor("#107C10");
    QColor m_mediumColor = QColor("#FFB900");
    QColor m_highColor = QColor("#D13438");
};

#endif // CAPACITYBAR_H