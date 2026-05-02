#include <QTextStream>

#include "ConsoleInput.h"
#include "FileProcessor.h"

int main()
{
    QTextStream out(stdout);

    out << "Lab_1 started" << Qt::endl;

    ConsoleInput inputReader;
    const InputData userInput = inputReader.read();

    if (!inputReader.validate(userInput, out)) {
        return 1;
    }

    out << Qt::endl;
    out << "Input data:" << Qt::endl;
    out << "Mode: " << userInput.mode << Qt::endl;
    out << "Folder path: " << userInput.folderPath << Qt::endl;
    out << "Password was entered" << Qt::endl;

    FileProcessor folderProcessor;

    if (!folderProcessor.printFiles(userInput.folderPath, out)) {
        return 1;
    }

    return 0;
}