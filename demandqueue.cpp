#include "demandqueue.h"
#include "popup.h"
#include "qevent.h"
#include "qshortcut.h"
#include "ui_demandqueue.h"
#include <iostream>

using namespace std;

DemandQueue::DemandQueue(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DemandQueue)
{
    ui->setupUi(this);
    this->shortcut = initShortcut();
}

QShortcut* DemandQueue::initShortcut()
{
    QShortcut* shortcut = new QShortcut(QKeySequence("Ctrl+S"), this);
    connect(shortcut, &QShortcut::activated, this, &DemandQueue::updateXLSX);
    return shortcut;
}

bool DemandQueue::updateXLSX()
{
    this->editing = false;
    return true;
}

void DemandQueue::closeEvent(QCloseEvent* e)
{

    if(!this->editing) {
        // Already saved or not editted at all
        e->accept();
    }
    else {
        // Unsaved edit content
        vector<QString> buttons = {tr("是"), tr("否"), tr("取消")};
        Popup confirm = Popup("保存","是否保存当前点播队列？", buttons, QIcon("isaac.ico"));
        QString reply = confirm.execute();
        if(reply == tr("是")) {
            cout << "保存中......" << endl;
            if (this->updateXLSX())
                e->accept();
            else
                e->ignore();
        }
        else if(reply == tr("否"))
            e->accept();
        else if(reply == tr("取消"))
            e->ignore();
        else {
            cout << "Unexpected reply " << reply.toUtf8().constData() << " received." << endl;
            e->accept();
        }
    }
}

DemandQueue::~DemandQueue()
{
    delete ui;
    delete shortcut;
}

