#include "CryptoManager.h"

#include <openssl/evp.h>
#include <openssl/rand.h>

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

    const EVP_CIPHER* cipher = EVP_aes_256_cbc();
    Q_UNUSED(cipher);

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