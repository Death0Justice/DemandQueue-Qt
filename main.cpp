#include "demandqueue.h"

#include <QApplication>
#include <QtWidgets>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    auto dq = new DemandQueue();
    dq->setWindowTitle("点播队列");
    dq->setWindowIcon(QIcon("isaac.ico"));
    dq->setFixedSize(960, 960);
    // Center the window
    QRect qr = dq->frameGeometry();
    QPoint cp = QApplication::primaryScreen()->availableGeometry().center();
    qr.moveCenter(cp);
    dq->move(qr.topLeft());
    dq->show();
    return a.exec();
}
