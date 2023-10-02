#include "popup.h"
#include "qpushbutton.h"

Popup::Popup(
    const QString& title,
    const QString& text,
    const vector<QString>& buttons,
    const QIcon& icon
    ) : buttons(buttons)
{
    this->setWindowTitle(title);
    this->setText(text);
    this->setWindowIcon(icon);
    this->buttons = vector<QString>(buttons);
    for(const auto& txt : this->buttons) {
        auto b = new QPushButton(txt);
        this->addButton(b, QMessageBox::NoRole);
    }
}

QString Popup::execute()
{
    int answer = this->exec();
    QString text = this->buttons[answer];
    return text;
}
