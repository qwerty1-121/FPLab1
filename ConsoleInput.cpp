#include "ConsoleInput.h"

InputData ConsoleInput::read() const
{
    QTextStream out(stdout);
    QTextStream in(stdin);

    InputData data;

    out << "Enter mode encrypt/decrypt:" << Qt::endl;
    data.mode = in.readLine();

    out << "Enter folder path:" << Qt::endl;
    data.folderPath = in.readLine();

    out << "Enter password:" << Qt::endl;
    data.password = in.readLine();

    return data;
}

bool ConsoleInput::validate(const InputData& data, QTextStream& out) const
{
    if (data.mode != "encrypt" && data.mode != "decrypt") {
        out << "Error: mode must be encrypt or decrypt." << Qt::endl;
        return false;
    }

    if (data.folderPath.isEmpty()) {
        out << "Error: folder path must not be empty." << Qt::endl;
        return false;
    }

    if (data.password.isEmpty()) {
        out << "Error: password must not be empty." << Qt::endl;
        return false;
    }

    return true;
}