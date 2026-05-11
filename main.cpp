#include <QTextStream>

#include "ConsoleInput.h"
#include "FileProcessor.h"

int main()
{
    QTextStream out(stdout);

    out << "Lab_1 started" << Qt::endl;

    // Считываем входные данные пользователя: режим, путь к папке и пароль.
    ConsoleInput inputReader;
    const InputData userInput = inputReader.read();

    // Проверяем корректность введённых данных перед обработкой файлов.
    if (!inputReader.validate(userInput, out)) {
        return 1;
    }

    out << Qt::endl;
    out << "Input data:" << Qt::endl;
    out << "Mode: " << userInput.mode << Qt::endl;
    out << "Folder path: " << userInput.folderPath << Qt::endl;
    out << "Password was entered" << Qt::endl;

    // Запускаем рекурсивную обработку файлов в выбранной папке.
    FileProcessor folderProcessor;

    if (!folderProcessor.processFiles(userInput.folderPath,
                                      userInput.mode,
                                      userInput.password,
                                      out)) {
        return 1;
    }

    return 0;
}