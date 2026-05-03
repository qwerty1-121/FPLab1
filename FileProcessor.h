#ifndef FILEPROCESSOR_H
#define FILEPROCESSOR_H

#include <QString>
#include <QTextStream>

class FileProcessor
{
public:
    bool processFiles(const QString& folderPath,
                      const QString& mode,
                      const QString& password,
                      QTextStream& out) const;

private:
    bool shouldProcessFile(const QString& filePath, const QString& mode) const;
};

#endif // FILEPROCESSOR_H