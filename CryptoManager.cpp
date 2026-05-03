#include "CryptoManager.h"

CryptoManager::CryptoManager()
{
}

CryptoManager& CryptoManager::instance()
{
    static CryptoManager manager;
    return manager;
}

bool CryptoManager::encryptFile(const QString& filePath,
                                const QString& password,
                                QTextStream& out) const
{
    Q_UNUSED(password);

    out << "Encrypt file: " << filePath << Qt::endl;

    return true;
}

bool CryptoManager::decryptFile(const QString& filePath,
                                const QString& password,
                                QTextStream& out) const
{
    Q_UNUSED(password);

    out << "Decrypt file: " << filePath << Qt::endl;

    return true;
}