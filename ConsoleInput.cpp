#include "ConsoleInput.h"

InputData ConsoleInput::read() const
{
    QTextStream out(stdout);
    QTextStream in(stdin);

    InputData data;

    // Считываем режим работы программы.
    out << "Enter mode encrypt/decrypt:" << Qt::endl;
    data.mode = in.readLine();

    // Считываем путь к папке, которую нужно обработать.
    out << "Enter folder path:" << Qt::endl;
    data.folderPath = in.readLine();

    // Считываем пароль для шифрования или дешифрования.
    out << "Enter password:" << Qt::endl;
    data.password = in.readLine();

    return data;
}

bool ConsoleInput::validate(const InputData& data, QTextStream& out) const
{
    // Проверяем, что выбран допустимый режим работы.
    if (data.mode != "encrypt" && data.mode != "decrypt") {
        out << "Error: mode must be encrypt or decrypt." << Qt::endl;
        return false;
    }

    // Путь к папке не должен быть пустым.
    if (data.folderPath.isEmpty()) {
        out << "Error: folder path must not be empty." << Qt::endl;
        return false;
    }

    // Пароль не должен быть пустым, так как на его основе создаётся ключ.
    if (data.password.isEmpty()) {
        out << "Error: password must not be empty." << Qt::endl;
        return false;
    }

    return true;
}