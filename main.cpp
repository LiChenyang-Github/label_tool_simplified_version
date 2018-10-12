#include "collector.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    collector w;
    w.show();

    return a.exec();
}
