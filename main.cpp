#include <QCoreApplication>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QTextStream out(stdout);

    out << "Lab_1 started" << Qt::endl;
    out << "Current stage: minimal Qt console project" << Qt::endl;
    out << "Arguments count: " << argc << Qt::endl;

    return 0;
}