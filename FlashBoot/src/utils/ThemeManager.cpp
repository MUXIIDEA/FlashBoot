#include "ThemeManager.h"

ThemeManager* ThemeManager::m_instance = nullptr;

ThemeManager::ThemeManager(QObject* parent) : QObject(parent), m_currentTheme(System) {
    loadStyleSheet();
}

ThemeManager::~ThemeManager() {
}

ThemeManager* ThemeManager::instance() {
    if (!m_instance) {
        m_instance = new ThemeManager();
    }
    return m_instance;
}

void ThemeManager::setTheme(Theme theme) {
    m_currentTheme = theme;
    loadStyleSheet();
    emit themeChanged(theme);
}

ThemeManager::Theme ThemeManager::currentTheme() const {
    return m_currentTheme;
}

void ThemeManager::applyTheme(QWidget* widget) {
    if (widget) {
        widget->setStyleSheet(m_styleSheet);
    }
}

void ThemeManager::toggleTheme() {
    switch (m_currentTheme) {
        case Light: setTheme(Dark); break;
        case Dark: setTheme(Light); break;
        case System: setTheme(isSystemDark() ? Light : Dark); break;
    }
}

void ThemeManager::loadStyleSheet() {
    QString bgPrimary, bgSecondary, bgCard;
    QString borderDefault, borderFocus;
    QString textPrimary, textSecondary;
    QString accent, accentHover;
    QString success, warning, error;

    Theme effectiveTheme = m_currentTheme;
    if (m_currentTheme == System) {
        effectiveTheme = isSystemDark() ? Dark : Light;
    }

    if (effectiveTheme == Dark) {
        bgPrimary = "#202020";
        bgSecondary = "#2D2D2D";
        bgCard = "#323232";
        borderDefault = "#424242";
        borderFocus = "#4CC2FF";
        textPrimary = "#FFFFFF";
        textSecondary = "#A0A0A0";
        accent = "#4CC2FF";
        accentHover = "#47B5F0";
        success = "#54B054";
        warning = "#FFC107";
        error = "#FF5252";
    } else {
        bgPrimary = "#F0F0F0";
        bgSecondary = "#FFFFFF";
        bgCard = "#FAFAFA";
        borderDefault = "#D4D4D4";
        borderFocus = "#0078D4";
        textPrimary = "#1A1A1A";
        textSecondary = "#616161";
        accent = "#0078D4";
        accentHover = "#006CBE";
        success = "#107C10";
        warning = "#FFB900";
        error = "#D13438";
    }

    m_styleSheet = QString(R"(
        QMainWindow {
            background-color: %1;
        }
        QWidget {
            color: %2;
            background-color: %3;
        }
        QGroupBox {
            border: 1px solid %4;
            border-radius: 4px;
            margin-top: 6px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 8px;
            padding: 0 4px 0 4px;
        }
        QPushButton {
            background-color: %5;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 6px 12px;
        }
        QPushButton:hover {
            background-color: %6;
        }
        QPushButton:pressed {
            background-color: %6;
        }
        QLineEdit {
            border: 1px solid %4;
            border-radius: 4px;
            padding: 4px;
            background-color: %7;
        }
        QLineEdit:focus {
            border-color: %8;
        }
        QComboBox {
            border: 1px solid %4;
            border-radius: 4px;
            padding: 4px;
            background-color: %7;
        }
        QComboBox:focus {
            border-color: %8;
        }
        QCheckBox {
            spacing: 4px;
        }
        QListWidget {
            border: 1px solid %4;
            border-radius: 4px;
            background-color: %7;
        }
        QListWidget::item:selected {
            background-color: #E5F3FF;
            color: %2;
        }
        QProgressBar {
            border: 1px solid %4;
            border-radius: 4px;
            text-align: center;
        }
        QProgressBar::chunk {
            background-color: %5;
            border-radius: 4px;
        }
        QPlainTextEdit {
            border: 1px solid %4;
            border-radius: 4px;
            font-family: Consolas;
            font-size: 9pt;
        }
    )").arg(bgPrimary).arg(textPrimary).arg(bgSecondary).arg(borderDefault)
      .arg(accent).arg(accentHover).arg(bgCard).arg(borderFocus);
}

bool ThemeManager::isSystemDark() const {
#ifdef Q_OS_WIN
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, 
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                      0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD value;
        DWORD size = sizeof(value);
        if (RegQueryValueExW(hKey, L"AppsUseLightTheme", nullptr, nullptr, 
                             reinterpret_cast<LPBYTE>(&value), &size) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return value == 0;
        }
        RegCloseKey(hKey);
    }
#endif
    return false;
}