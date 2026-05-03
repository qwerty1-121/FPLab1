#include "FileProcessor.h"
#include "CryptoManager.h"

#include <QDir>
#include <QDirIterator>

bool FileProcessor::processFiles(const QString& folderPath,
                                 const QString& mode,
                                 const QString& password,
                                 QTextStream& out) const
{
    QDir directory(folderPath);

    if (!directory.exists()) {
        out << "Error: folder does not exist." << Qt::endl;
        return false;
    }

    out << Qt::endl;
    out << "Files processing started:" << Qt::endl;

    QDirIterator iterator(
        directory.absolutePath(),
        QDir::Files,
        QDirIterator::Subdirectories
    );

    int fileCount = 0;

    CryptoManager& cryptoManager = CryptoManager::instance();

    while (iterator.hasNext()) {
        const QString filePath = iterator.next();

        if (!shouldProcessFile(filePath, mode)) {
            continue;
        }

        bool result = false;

        if (mode == "encrypt") {
            result = cryptoManager.encryptFile(filePath, password, out);
        } else {
            result = cryptoManager.decryptFile(filePath, password, out);
        }

        if (!result) {
            out << "Error: file processing failed: " << filePath << Qt::endl;
            return false;
        }

        ++fileCount;
    }

    out << Qt::endl;
    out << "Total processed files: " << fileCount << Qt::endl;

    return true;
}

bool FileProcessor::shouldProcessFile(const QString& filePath, const QString& mode) const
{
    const bool isEncryptedFile = filePath.endsWith(".enc");

    if (mode == "encrypt") {
        return !isEncryptedFile;
    }

    if (mode == "decrypt") {
        return isEncryptedFile;
    }

    return false;
}