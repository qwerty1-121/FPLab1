#include <QTextStream>
#include <QString>
#include <QDir>

int main()
{
    QTextStream out(stdout);
    QTextStream in(stdin);

    out << "Lab_1 started" << Qt::endl;

    out << "Enter mode encrypt/decrypt:" << Qt::endl;
    const QString mode = in.readLine();

    out << "Enter folder path:" << Qt::endl;
    const QString folderPath = in.readLine();

    out << "Enter password:" << Qt::endl;
    const QString password = in.readLine();

    if (mode != "encrypt" && mode != "decrypt") {
        out << "Error: mode must be encrypt or decrypt." << Qt::endl;
        return 1;
    }

    if (folderPath.isEmpty()) {
        out << "Error: folder path must not be empty." << Qt::endl;
        return 1;
    }

    if (password.isEmpty()) {
        out << "Error: password must not be empty." << Qt::endl;
        return 1;
    }

    QDir directory(folderPath);

    if (!directory.exists()) {
        out << "Error: folder does not exist." << Qt::endl;
        return 1;
    }

    out << Qt::endl;
    out << "Input data:" << Qt::endl;
    out << "Mode: " << mode << Qt::endl;
    out << "Folder path: " << directory.absolutePath() << Qt::endl;
    out << "Password was entered" << Qt::endl;

    return 0;
}