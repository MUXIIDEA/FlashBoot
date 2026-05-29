#ifndef ISOIMAGEMANAGER_H
#define ISOIMAGEMANAGER_H

#include "pch.h"
#include "IsoInfo.h"

class IsoImageManager : public QObject {
    Q_OBJECT

public:
    explicit IsoImageManager(QObject* parent = nullptr);

    IsoInfo parseIso(const QString& filePath);
    void verifyIsoAsync(const QString& filePath);
    bool isBootable(const QString& filePath);
    BootMode detectBootMode(const QString& filePath);

signals:
    void isoParsed(const IsoInfo& info);
    void verificationProgress(int percent);
    void verificationFinished(bool valid, const QString& hash);
    void errorOccurred(const QString& error);

private:
    IsoInfo parseIso9660(const QString& path);
    QString extractVersionFromIso(const QString& path);
    ArchType detectArch(const QString& path);
};

#endif // ISOIMAGEMANAGER_H