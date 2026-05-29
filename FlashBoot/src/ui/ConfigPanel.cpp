#include "ConfigPanel.h"
#include <QRandomGenerator>

ConfigPanel::ConfigPanel(QWidget* parent) : QWidget(parent) {
    setupUI();
}

void ConfigPanel::setupUI() {
    auto* layout = new QVBoxLayout(this);
    layout->setSpacing(12);
    layout->setContentsMargins(8, 8, 8, 8);

    auto* isoGroup = new QGroupBox("ISO 镜像", this);
    auto* isoLayout = new QFormLayout(isoGroup);

    auto* isoPathRow = new QHBoxLayout();
    m_isoPathEdit = new QLineEdit(this);
    m_isoPathEdit->setReadOnly(true);
    m_isoPathEdit->setPlaceholderText("请选择 ISO 镜像文件");
    isoPathRow->addWidget(m_isoPathEdit, 1);

    m_browseBtn = new QPushButton("浏览...", this);
    m_browseBtn->setStyleSheet("padding: 4px 12px;");
    connect(m_browseBtn, &QPushButton::clicked, this, &ConfigPanel::onBrowseClicked);
    isoPathRow->addWidget(m_browseBtn);

    isoLayout->addRow("镜像路径:", isoPathRow);

    auto* infoGrid = new QGridLayout();
    m_isoNameLabel = new QLabel("-", this);
    m_isoVersionLabel = new QLabel("-", this);
    m_isoArchLabel = new QLabel("-", this);
    m_isoSizeLabel = new QLabel("-", this);
    m_isoBootModeLabel = new QLabel("-", this);
    m_isoHashLabel = new QLabel("-", this);

    infoGrid->addWidget(new QLabel("名称:"), 0, 0);
    infoGrid->addWidget(m_isoNameLabel, 0, 1);
    infoGrid->addWidget(new QLabel("版本:"), 1, 0);
    infoGrid->addWidget(m_isoVersionLabel, 1, 1);
    infoGrid->addWidget(new QLabel("架构:"), 2, 0);
    infoGrid->addWidget(m_isoArchLabel, 2, 1);
    infoGrid->addWidget(new QLabel("大小:"), 3, 0);
    infoGrid->addWidget(m_isoSizeLabel, 3, 1);
    infoGrid->addWidget(new QLabel("启动模式:"), 4, 0);
    infoGrid->addWidget(m_isoBootModeLabel, 4, 1);
    infoGrid->addWidget(new QLabel("SHA256:"), 5, 0);
    infoGrid->addWidget(m_isoHashLabel, 5, 1);

    isoLayout->addRow(infoGrid);

    layout->addWidget(isoGroup);

    auto* configGroup = new QGroupBox("创建配置", this);
    auto* configLayout = new QFormLayout(configGroup);

    m_partitionCombo = new QComboBox(this);
    m_partitionCombo->addItems({"GPT (推荐)", "MBR"});
    configLayout->addRow("分区方案:", m_partitionCombo);

    m_targetCombo = new QComboBox(this);
    m_targetCombo->addItems({"UEFI + BIOS", "仅 UEFI", "仅 BIOS"});
    configLayout->addRow("目标系统:", m_targetCombo);

    auto* labelRow = new QHBoxLayout();
    m_volumeLabelEdit = new QLineEdit("FLASHBOOT", this);
    labelRow->addWidget(m_volumeLabelEdit, 1);

    m_randomLabelBtn = new QPushButton("随机", this);
    m_randomLabelBtn->setStyleSheet("padding: 2px 8px; font-size: 11px;");
    connect(m_randomLabelBtn, &QPushButton::clicked, this, &ConfigPanel::onRandomLabel);
    labelRow->addWidget(m_randomLabelBtn);

    configLayout->addRow("卷标:", labelRow);

    m_quickFormatCheck = new QCheckBox("快速格式化", this);
    m_quickFormatCheck->setChecked(true);
    configLayout->addRow(m_quickFormatCheck);

    m_extendedLabelCheck = new QCheckBox("创建扩展卷标", this);
    m_extendedLabelCheck->setChecked(true);
    configLayout->addRow(m_extendedLabelCheck);

    layout->addWidget(configGroup);

    auto* formatGroup = new QGroupBox("高级选项", this);
    auto* formatLayout = new QVBoxLayout(formatGroup);

    m_badBlocksCheck = new QCheckBox("检查坏块", this);
    formatLayout->addWidget(m_badBlocksCheck);

    m_fixedDriveLetterCheck = new QCheckBox("使用固定驱动器号", this);
    formatLayout->addWidget(m_fixedDriveLetterCheck);

    layout->addWidget(formatGroup);

    auto* actionRow = new QHBoxLayout();
    auto* aboutBtn = new QPushButton("ⓘ 关于", this);
    aboutBtn->setStyleSheet("padding: 6px 12px;");
    actionRow->addWidget(aboutBtn);

    auto* settingsBtn = new QPushButton("⚙ 设置", this);
    settingsBtn->setStyleSheet("padding: 6px 12px;");
    connect(settingsBtn, &QPushButton::clicked, this, &ConfigPanel::onSettingsClicked);
    actionRow->addWidget(settingsBtn);

    actionRow->addStretch(1);

    m_startClicked = new QPushButton("开始 ▶", this);
    m_startClicked->setStyleSheet("padding: 8px 32px; background-color: #0078D4; color: white; border: none; border-radius: 4px; font-weight: bold;");
    m_startClicked->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(m_startClicked, &QPushButton::clicked, this, &ConfigPanel::onStartClicked);
    actionRow->addWidget(m_startClicked);

    auto* closeBtn = new QPushButton("✕ 关闭", this);
    closeBtn->setStyleSheet("padding: 6px 12px;");
    connect(closeBtn, &QPushButton::clicked, [this]() {
        window()->close();
    });
    actionRow->addWidget(closeBtn);

    layout->addLayout(actionRow);

    setLayout(layout);
}

void ConfigPanel::setIsoInfo(const IsoInfo& iso) {
    m_currentIso = iso;
    m_isoPathEdit->setText(iso.filePath);
    m_isoNameLabel->setText(iso.displayName.isEmpty() ? iso.fileName : iso.displayName);
    m_isoVersionLabel->setText(iso.version.isEmpty() ? "-" : iso.version);
    m_isoArchLabel->setText(iso.archString());
    m_isoSizeLabel->setText(iso.fileSizeString());
    m_isoBootModeLabel->setText(iso.bootModeString());
    m_isoHashLabel->setText(iso.sha256String().left(16) + "...");
}

IsoInfo ConfigPanel::currentIsoInfo() const {
    return m_currentIso;
}

QString ConfigPanel::volumeLabel() const {
    return m_volumeLabelEdit->text();
}

bool ConfigPanel::quickFormat() const {
    return m_quickFormatCheck->isChecked();
}

bool ConfigPanel::createExtendedLabel() const {
    return m_extendedLabelCheck->isChecked();
}

bool ConfigPanel::checkBadBlocks() const {
    return m_badBlocksCheck->isChecked();
}

bool ConfigPanel::useFixedDriveLetter() const {
    return m_fixedDriveLetterCheck->isChecked();
}

QString ConfigPanel::partitionScheme() const {
    return m_partitionCombo->currentText().contains("MBR") ? "MBR" : "GPT";
}

QString ConfigPanel::targetSystem() const {
    return m_targetCombo->currentText();
}

void ConfigPanel::onBrowseClicked() {
    emit isoBrowseRequested();
}

void ConfigPanel::onRandomLabel() {
    QString chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    QString label;
    for (int i = 0; i < 8; ++i) {
        label += chars[QRandomGenerator::global()->bounded(chars.size())];
    }
    m_volumeLabelEdit->setText(label);
}

void ConfigPanel::onStartClicked() {
    emit startClicked();
}

void ConfigPanel::onSettingsClicked() {
    emit settingsClicked();
}