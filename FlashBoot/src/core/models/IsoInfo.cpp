#include "IsoInfo.h"

IsoInfo IsoInfo::fromFile(const QString& path) {
    IsoInfo info;
    info.filePath = path;
    info.fileName = QFileInfo(path).fileName();
    info.fileSize = QFileInfo(path).size();

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        info.errorString = QObject::tr("Cannot open file: %1").arg(file.errorString());
        return info;
    }

    file.seek(16 * 2048);
    QByteArray sector = file.read(2048);

    if (sector.size() < 2048) {
        info.errorString = QObject::tr("File too small to be a valid ISO");
        return info;
    }

    if (sector[0] != 0x01 || sector[1] != 'C' || sector[2] != 'D' ||
        sector[3] != '0' || sector[4] != '0' || sector[5] != '1') {
        info.errorString = QObject::tr("Invalid ISO 9660 signature");
        return info;
    }

    QByteArray volumeLabel = sector.mid(40, 32);
    int endPos = volumeLabel.indexOf('\x00');
    if (endPos < 0) endPos = volumeLabel.size();
    while (endPos > 0 && volumeLabel[endPos - 1] == ' ') endPos--;
    info.displayName = QString::fromLatin1(volumeLabel.left(endPos));

    file.seek(17 * 2048);
    QByteArray bootRecord = file.read(2048);

    if (bootRecord.size() >= 8 &&
        bootRecord.mid(0, 8) == "EL TORITO SPECIFICATION") {
        file.seek(0);
        QByteArray header = file.read(65536);

        bool hasUefi = header.contains("efi/boot/bootx64.efi") ||
                        header.contains("EFI/BOOT/BOOTX64.EFI") ||
                        header.contains("efi/boot/bootia32.efi");

        bool hasBios = header.contains("isolinux.bin") ||
                       header.contains("bootmgr") ||
                       header.contains("ntldr");

        if (hasUefi && hasBios) info.bootMode = BootMode::Dual;
        else if (hasUefi) info.bootMode = BootMode::UEFI;
        else if (hasBios) info.bootMode = BootMode::BIOS;
    }

    file.seek(0);
    QByteArray header = file.read(262144);

    if (header.contains("amd64") || header.contains("x64") ||
        header.contains("BOOTX64.EFI")) {
        info.arch = ArchType::x64;
    } else if (header.contains("x86") || header.contains("i386") ||
               header.contains("BOOTIA32.EFI")) {
        info.arch = ArchType::x86;
    } else if (header.contains("arm64") || header.contains("aarch64") ||
               header.contains("BOOTAA64.EFI")) {
        info.arch = ArchType::ARM64;
    }

    QRegularExpression versionRegex(R"(Windows (\d{1,2})\s*(\d{4})?.*Build (\d+))");
    QRegularExpressionMatch match = versionRegex.match(info.displayName);
    if (match.hasMatch()) {
        QString yearStr = match.captured(2);
        info.version = yearStr.isEmpty() ? QString("Windows %1").arg(match.captured(1))
                                          : QString("Windows %1 %2").arg(match.captured(1)).arg(yearStr);
        info.buildNumber = match.captured(3);
    }

    info.isValid = true;
    return info;
}