#include "FileProcessor.h"

#include <QDir>
#include <QDirIterator>

bool FileProcessor::printFiles(const QString& folderPath, QTextStream& out) const
{
    QDir directory(folderPath);

    if (!directory.exists()) {
        out << "Error: folder does not exist." << Qt::endl;
        return false;
    }

    out << Qt::endl;
    out << "Files found:" << Qt::endl;

    QDirIterator iterator(
        directory.absolutePath(),
        QDir::Files,
        QDirIterator::Subdirectories
    );

    int fileCount = 0;

    while (iterator.hasNext()) {
        const QString filePath = iterator.next();
        out << filePath << Qt::endl;
        ++fileCount;
    }

    out << Qt::endl;
    out << "Total files: " << fileCount << Qt::endl;

    return true;
}