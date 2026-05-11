#include "CryptoManager.h"

#include <QFile>
#include <QByteArray>
#include <QIODevice>

#include <openssl/evp.h>
#include <openssl/rand.h>

namespace
{
    // Служебная сигнатура, по которой программа определяет,
    // что файл был зашифрован именно этим приложением.
    const QByteArray FILE_MAGIC = "FPLAB1";

    // Основные параметры криптографической обработки.
    const int SALT_SIZE = 16;
    const int IV_SIZE = 16;
    const int KEY_SIZE = 32;
    const int BUFFER_SIZE = 4096;
    const int PBKDF2_ITERATIONS = 100000;
}

CryptoManager::CryptoManager()
{
}

CryptoManager& CryptoManager::instance()
{
    // Локальная static-переменная создаётся один раз
    // и обеспечивает реализацию Singleton без ручного new/delete.
    static CryptoManager manager;
    return manager;
}

bool CryptoManager::encryptFile(const QString& filePath,
                                const QString& password,
                                QTextStream& out) const
{
    // Результат шифрования сначала записывается во временный файл.
    // Это позволяет не менять имя исходного файла и не повредить его при ошибке.
    const QString temporaryFilePath = filePath + ".tmp";

    if (QFile::exists(temporaryFilePath)) {
        out << "Error: temporary file already exists: " << temporaryFilePath << Qt::endl;
        return false;
    }

    QFile inputFile(filePath);

    if (!inputFile.open(QIODevice::ReadOnly)) {
        out << "Error: cannot open file for reading: " << filePath << Qt::endl;
        return false;
    }

    QFile outputFile(temporaryFilePath);

    if (!outputFile.open(QIODevice::WriteOnly)) {
        out << "Error: cannot create temporary encrypted file: " << temporaryFilePath << Qt::endl;
        inputFile.close();
        return false;
    }

    // Соль и IV генерируются случайно для каждого файла.
    // Ключ будет получен из пароля и соли через PBKDF2.
    QByteArray salt(SALT_SIZE, 0);
    QByteArray iv(IV_SIZE, 0);
    QByteArray key(KEY_SIZE, 0);

    if (RAND_bytes(reinterpret_cast<unsigned char*>(salt.data()), SALT_SIZE) != 1) {
        out << "Error: cannot generate salt." << Qt::endl;
        inputFile.close();
        outputFile.close();
        outputFile.remove();
        return false;
    }

    if (RAND_bytes(reinterpret_cast<unsigned char*>(iv.data()), IV_SIZE) != 1) {
        out << "Error: cannot generate IV." << Qt::endl;
        inputFile.close();
        outputFile.close();
        outputFile.remove();
        return false;
    }

    const QByteArray passwordBytes = password.toUtf8();

    // Получение AES-ключа из пароля.
    // PBKDF2-HMAC-SHA256 делает подбор пароля более трудоёмким.
    if (PKCS5_PBKDF2_HMAC(passwordBytes.constData(),
                          passwordBytes.size(),
                          reinterpret_cast<const unsigned char*>(salt.constData()),
                          salt.size(),
                          PBKDF2_ITERATIONS,
                          EVP_sha256(),
                          KEY_SIZE,
                          reinterpret_cast<unsigned char*>(key.data())) != 1) {
        out << "Error: cannot create encryption key from password." << Qt::endl;
        inputFile.close();
        outputFile.close();
        outputFile.remove();
        return false;
    }

    // Контекст OpenSSL хранит состояние операции AES-256-CBC.
    EVP_CIPHER_CTX* context = EVP_CIPHER_CTX_new();

    if (context == nullptr) {
        out << "Error: cannot create OpenSSL context." << Qt::endl;
        inputFile.close();
        outputFile.close();
        outputFile.remove();
        return false;
    }

    bool success = true;

    // Настройка контекста на шифрование AES-256-CBC с полученным ключом и IV.
    if (EVP_EncryptInit_ex(context,
                           EVP_aes_256_cbc(),
                           nullptr,
                           reinterpret_cast<const unsigned char*>(key.constData()),
                           reinterpret_cast<const unsigned char*>(iv.constData())) != 1) {
        out << "Error: cannot initialize encryption." << Qt::endl;
        success = false;
    }

    // В начало файла записываются служебные данные,
    // необходимые для последующего дешифрования.
    if (success && outputFile.write(FILE_MAGIC) != FILE_MAGIC.size()) {
        out << "Error: cannot write file magic." << Qt::endl;
        success = false;
    }

    if (success && outputFile.write(salt) != salt.size()) {
        out << "Error: cannot write salt." << Qt::endl;
        success = false;
    }

    if (success && outputFile.write(iv) != iv.size()) {
        out << "Error: cannot write IV." << Qt::endl;
        success = false;
    }

    const int blockSize = EVP_CIPHER_block_size(EVP_aes_256_cbc());

    // Файл читается частями, чтобы не загружать его полностью в память.
    while (success && !inputFile.atEnd()) {
        const QByteArray inputBuffer = inputFile.read(BUFFER_SIZE);

        QByteArray encryptedBuffer(inputBuffer.size() + blockSize, 0);
        int encryptedLength = 0;

        if (EVP_EncryptUpdate(context,
                              reinterpret_cast<unsigned char*>(encryptedBuffer.data()),
                              &encryptedLength,
                              reinterpret_cast<const unsigned char*>(inputBuffer.constData()),
                              inputBuffer.size()) != 1) {
            out << "Error: encryption failed for file: " << filePath << Qt::endl;
            success = false;
            break;
        }

        if (outputFile.write(encryptedBuffer.constData(), encryptedLength) != encryptedLength) {
            out << "Error: cannot write encrypted data." << Qt::endl;
            success = false;
            break;
        }
    }

    // Финальный этап шифрования добавляет padding и завершает операцию.
    if (success) {
        QByteArray finalBuffer(blockSize, 0);
        int finalLength = 0;

        if (EVP_EncryptFinal_ex(context,
                                reinterpret_cast<unsigned char*>(finalBuffer.data()),
                                &finalLength) != 1) {
            out << "Error: final encryption step failed." << Qt::endl;
            success = false;
        }

        if (success && outputFile.write(finalBuffer.constData(), finalLength) != finalLength) {
            out << "Error: cannot write final encrypted data." << Qt::endl;
            success = false;
        }
    }

    // Освобождение ресурсов OpenSSL и закрытие файлов перед заменой.
    EVP_CIPHER_CTX_free(context);

    inputFile.close();
    outputFile.close();

    if (!success) {
        QFile::remove(temporaryFilePath);
        return false;
    }

    // Безопасная замена: временный файл становится исходным файлом.
    // Имя и расширение файла при этом не меняются.
    if (!QFile::remove(filePath)) {
        QFile::remove(temporaryFilePath);
        out << "Error: cannot remove original file after encryption: " << filePath << Qt::endl;
        return false;
    }

    if (!QFile::rename(temporaryFilePath, filePath)) {
        out << "Error: cannot replace original file after encryption: " << filePath << Qt::endl;
        return false;
    }

    out << "Encrypted file: " << filePath << Qt::endl;

    return true;
}

bool CryptoManager::decryptFile(const QString& filePath,
                                const QString& password,
                                QTextStream& out) const
{
    // Расшифрованный результат сначала записывается во временный файл.
    const QString temporaryFilePath = filePath + ".tmp";

    if (QFile::exists(temporaryFilePath)) {
        out << "Error: temporary file already exists: " << temporaryFilePath << Qt::endl;
        return false;
    }

    QFile inputFile(filePath);

    if (!inputFile.open(QIODevice::ReadOnly)) {
        out << "Error: cannot open encrypted file for reading: " << filePath << Qt::endl;
        return false;
    }

    QFile outputFile(temporaryFilePath);

    if (!outputFile.open(QIODevice::WriteOnly)) {
        out << "Error: cannot create temporary decrypted file: " << temporaryFilePath << Qt::endl;
        inputFile.close();
        return false;
    }

    // Проверка служебной сигнатуры файла.
    // Если сигнатуры нет, файл не считается зашифрованным этим приложением.
    const QByteArray fileMagic = inputFile.read(FILE_MAGIC.size());

    if (fileMagic != FILE_MAGIC) {
        out << "Error: invalid encrypted file format: " << filePath << Qt::endl;
        inputFile.close();
        outputFile.close();
        outputFile.remove();
        return false;
    }

    // Чтение salt и IV, записанных при шифровании.
    const QByteArray salt = inputFile.read(SALT_SIZE);
    const QByteArray iv = inputFile.read(IV_SIZE);

    if (salt.size() != SALT_SIZE || iv.size() != IV_SIZE) {
        out << "Error: damaged encrypted file: " << filePath << Qt::endl;
        inputFile.close();
        outputFile.close();
        outputFile.remove();
        return false;
    }

    QByteArray key(KEY_SIZE, 0);
    const QByteArray passwordBytes = password.toUtf8();

    // Повторное получение ключа из пароля и salt.
    // При правильном пароле получится тот же ключ, что и при шифровании.
    if (PKCS5_PBKDF2_HMAC(passwordBytes.constData(),
                          passwordBytes.size(),
                          reinterpret_cast<const unsigned char*>(salt.constData()),
                          salt.size(),
                          PBKDF2_ITERATIONS,
                          EVP_sha256(),
                          KEY_SIZE,
                          reinterpret_cast<unsigned char*>(key.data())) != 1) {
        out << "Error: cannot create decryption key from password." << Qt::endl;
        inputFile.close();
        outputFile.close();
        outputFile.remove();
        return false;
    }

    EVP_CIPHER_CTX* context = EVP_CIPHER_CTX_new();

    if (context == nullptr) {
        out << "Error: cannot create OpenSSL context." << Qt::endl;
        inputFile.close();
        outputFile.close();
        outputFile.remove();
        return false;
    }

    bool success = true;

    // Настройка контекста OpenSSL на дешифрование AES-256-CBC.
    if (EVP_DecryptInit_ex(context,
                           EVP_aes_256_cbc(),
                           nullptr,
                           reinterpret_cast<const unsigned char*>(key.constData()),
                           reinterpret_cast<const unsigned char*>(iv.constData())) != 1) {
        out << "Error: cannot initialize decryption." << Qt::endl;
        success = false;
    }

    const int blockSize = EVP_CIPHER_block_size(EVP_aes_256_cbc());

    // Чтение и дешифрование содержимого файла частями.
    while (success && !inputFile.atEnd()) {
        const QByteArray encryptedBuffer = inputFile.read(BUFFER_SIZE);

        QByteArray decryptedBuffer(encryptedBuffer.size() + blockSize, 0);
        int decryptedLength = 0;

        if (EVP_DecryptUpdate(context,
                              reinterpret_cast<unsigned char*>(decryptedBuffer.data()),
                              &decryptedLength,
                              reinterpret_cast<const unsigned char*>(encryptedBuffer.constData()),
                              encryptedBuffer.size()) != 1) {
            out << "Error: decryption failed for file: " << filePath << Qt::endl;
            success = false;
            break;
        }

        if (outputFile.write(decryptedBuffer.constData(), decryptedLength) != decryptedLength) {
            out << "Error: cannot write decrypted data." << Qt::endl;
            success = false;
            break;
        }
    }

    // Финальный этап дешифрования проверяет и удаляет padding.
    // Ошибка здесь часто означает неверный пароль или повреждённый файл.
    if (success) {
        QByteArray finalBuffer(blockSize, 0);
        int finalLength = 0;

        if (EVP_DecryptFinal_ex(context,
                                reinterpret_cast<unsigned char*>(finalBuffer.data()),
                                &finalLength) != 1) {
            out << "Error: final decryption step failed. Wrong password or damaged file." << Qt::endl;
            success = false;
        }

        if (success && outputFile.write(finalBuffer.constData(), finalLength) != finalLength) {
            out << "Error: cannot write final decrypted data." << Qt::endl;
            success = false;
        }
    }

    EVP_CIPHER_CTX_free(context);

    inputFile.close();
    outputFile.close();

    if (!success) {
        QFile::remove(temporaryFilePath);
        return false;
    }

    // После успешного дешифрования временный файл заменяет исходный,
    // поэтому имя файла остаётся прежним.
    if (!QFile::remove(filePath)) {
        QFile::remove(temporaryFilePath);
        out << "Error: cannot remove encrypted file after decryption: " << filePath << Qt::endl;
        return false;
    }

    if (!QFile::rename(temporaryFilePath, filePath)) {
        out << "Error: cannot replace encrypted file after decryption: " << filePath << Qt::endl;
        return false;
    }

    out << "Decrypted file: " << filePath << Qt::endl;

    return true;
}