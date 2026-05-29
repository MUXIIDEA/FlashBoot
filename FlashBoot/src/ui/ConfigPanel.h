#ifndef CONFIGPANEL_H
#define CONFIGPANEL_H

#include "pch.h"
#include "IsoInfo.h"

class ConfigPanel : public QWidget {
    Q_OBJECT

public:
    explicit ConfigPanel(QWidget* parent = nullptr);

    void setIsoInfo(const IsoInfo& iso);
    IsoInfo currentIsoInfo() const;

    QString volumeLabel() const;
    bool quickFormat() const;
    bool createExtendedLabel() const;
    bool checkBadBlocks() const;
    bool useFixedDriveLetter() const;
    QString partitionScheme() const;
    QString targetSystem() const;

signals:
    void isoBrowseRequested();
    void isoDownloadRequested();
    void isoVerifyRequested();
    void startClicked();
    void settingsClicked();

private slots:
    void onBrowseClicked();
    void onRandomLabel();
    void onStartClicked();
    void onSettingsClicked();

private:
    void setupUI();

    QLineEdit* m_isoPathEdit;
    QPushButton* m_browseBtn;

    QLabel* m_isoNameLabel;
    QLabel* m_isoVersionLabel;
    QLabel* m_isoArchLabel;
    QLabel* m_isoSizeLabel;
    QLabel* m_isoBootModeLabel;
    QLabel* m_isoHashLabel;

    QComboBox* m_partitionCombo;
    QComboBox* m_targetCombo;

    QLineEdit* m_volumeLabelEdit;
    QPushButton* m_randomLabelBtn;

    QCheckBox* m_quickFormatCheck;
    QCheckBox* m_extendedLabelCheck;
    QCheckBox* m_badBlocksCheck;
    QCheckBox* m_fixedDriveLetterCheck;

    QPushButton* m_showAdvancedDeviceBtn;
    QPushButton* m_showAdvancedFormatBtn;

    QWidget* m_advancedDeviceGroup;
    QWidget* m_advancedFormatGroup;

    QPushButton* m_startClicked;

    IsoInfo m_currentIso;
};

#endif // CONFIGPANEL_H