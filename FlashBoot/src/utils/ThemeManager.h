#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include "pch.h"

class ThemeManager : public QObject {
    Q_OBJECT

public:
    enum Theme {
        Light,
        Dark,
        System
    };

    static ThemeManager* instance();

    void setTheme(Theme theme);
    Theme currentTheme() const;
    void applyTheme(QWidget* widget);
    void toggleTheme();

signals:
    void themeChanged(Theme theme);

private:
    ThemeManager(QObject* parent = nullptr);
    ~ThemeManager();

    void loadStyleSheet();
    bool isSystemDark() const;

    static ThemeManager* m_instance;
    Theme m_currentTheme;
    QString m_styleSheet;
};

#endif // THEMEMANAGER_H