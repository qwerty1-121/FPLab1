#include "FileProcessor.h"
#include "CryptoManager.h"

#include <QDir>
#include <QDirIterator>

bool FileProcessor::processFiles(const QString& folderPath,
                                 const QString& mode,
                                 const QString& password,
                                 QTextStream& out) const
{
    // Проверяем, что пользователь передал путь именно к существующей папке.
    QDir directory(folderPath);

    if (!directory.exists()) {
        out << "Error: folder does not exist." << Qt::endl;
        return false;
    }

    out << Qt::endl;
    out << "Files processing started:" << Qt::endl;

    // Рекурсивный обход всех файлов в указанной папке и её подпапках.
    QDirIterator iterator(
        directory.absolutePath(),
        QDir::Files,
        QDirIterator::Subdirectories
    );

    int fileCount = 0;

    // Получаем единственный экземпляр CryptoManager, реализованный как Singleton.
    CryptoManager& cryptoManager = CryptoManager::instance();

    while (iterator.hasNext()) {
        const QString filePath = iterator.next();

        // Пропускаем файлы, которые не должны участвовать в обработке.
        if (!shouldProcessFile(filePath, mode)) {
            continue;
        }

        bool result = false;

        // Для каждого найденного файла выполняется операция согласно выбранному режиму.
        if (mode == "encrypt") {
            result = cryptoManager.encryptFile(filePath, password, out);
        } else if (mode == "decrypt") {
            result = cryptoManager.decryptFile(filePath, password, out);
        } else {
            out << "Error: unknown processing mode: " << mode << Qt::endl;
            return false;
        }

        // При ошибке одного файла останавливаем обработку всей папки.
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
    // Временные файлы используются для безопасной замены и не должны обрабатываться.
    if (filePath.endsWith(".tmp")) {
        return false;
    }

    // Обычные файлы обрабатываются только в допустимых режимах.
    if (mode == "encrypt" || mode == "decrypt") {
        return true;
    }

    return false;
}