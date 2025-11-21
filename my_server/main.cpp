#include <QApplication>
#include "server.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QScopedPointer<Server>server(new Server());

    return a.exec();
}
