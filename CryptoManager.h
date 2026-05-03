#ifndef CRYPTOMANAGER_H
#define CRYPTOMANAGER_H

#include <QString>
#include <QTextStream>

class CryptoManager
{
public:
    static CryptoManager& instance();

    bool encryptFile(const QString& filePath,
                     const QString& password,
                     QTextStream& out) const;

    bool decryptFile(const QString& filePath,
                     const QString& password,
                     QTextStream& out) const;

private:
    CryptoManager();

    CryptoManager(const CryptoManager&) = delete;
    CryptoManager& operator=(const CryptoManager&) = delete;
};

#endif // CRYPTOMANAGER_H