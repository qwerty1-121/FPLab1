#ifndef CONSOLEINPUT_H
#define CONSOLEINPUT_H

#include <QString>
#include <QTextStream>

struct InputData
{
    QString mode;
    QString folderPath;
    QString password;
};

class ConsoleInput
{
public:
    InputData read() const;
    bool validate(const InputData& data, QTextStream& out) const;
};

#endif // CONSOLEINPUT_H