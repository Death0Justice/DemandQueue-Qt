#ifndef POPUP_H
#define POPUP_H
#include "qmessagebox.h"
#include <QWidget>

using namespace std;

class Popup : public QMessageBox {

    Q_OBJECT

public:
    Popup(
        const QString& title,
        const QString& text,
        const vector<QString>& buttons = {tr("确认")},
        const QIcon& icon = QIcon()
        );

    QString execute();

private:
    vector<QString> buttons;
};

#endif // POPUP_H
