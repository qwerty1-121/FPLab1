#ifndef FILEPROCESSOR_H
#define FILEPROCESSOR_H

#include <QString>
#include <QTextStream>

class FileProcessor
{
public:
    bool printFiles(const QString& folderPath, QTextStream& out) const;
};

#endif // FILEPROCESSOR_H