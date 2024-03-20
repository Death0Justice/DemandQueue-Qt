#include "demandqueue.h"
#include "popup.h"
#include "qboxlayout.h"
#include "qdatetime.h"
#include "qevent.h"
#include "qlabel.h"
#include "qmenu.h"
#include "qshortcut.h"
#include "ui_demandqueue.h"
#include <filesystem>
#include <iostream>
#include <sstream>
#include <chrono>
#include <OpenXLSX.hpp>

DemandQueue::DemandQueue(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DemandQueue)
{
    ui->setupUi(this);
    initShortcut();
    initHistory();
    initTable(queue);
}

void DemandQueue::initShortcut()
{
    shortcut = new QShortcut(QKeySequence("Ctrl+S"), this);
    connect(shortcut, &QShortcut::activated, this, &DemandQueue::updateXLSX);
}

void DemandQueue::initHistory()
{
    queue = new std::deque<std::vector<QString>>();
    OpenXLSX::XLDocument doc;
    if (std::filesystem::exists("./history.xlsx")) {
        doc.open("./history.xlsx");
    }
    else {
        doc.create("./history.xlsx");
        doc.workbook().worksheet("Sheet1").setName("待完成点播");
        doc.workbook().addWorksheet("已完成点播");
        doc.save();
    }
//    std::cout << "There are " << doc.workbook().worksheetCount() << " sheets in new workbook" << std::endl;
//    for (auto& name : doc.workbook().worksheetNames()) {
//        std::cout << name << std::endl;
//    }
    auto incomplete = doc.workbook().worksheet("待完成点播");
    auto history = doc.workbook().worksheet("已完成点播");
    for (uint32_t i = 2; i <= incomplete.rowCount(); i++) {
        QString name = QString::fromStdString(incomplete.cell(i, 0).value());
        QString date;
        auto dateCell = incomplete.cell(i, 1);
        std::string dateType = dateCell.value().typeAsString();
        if (dateCell.value().typeAsString() == "string")
            date = QString::fromStdString(dateCell.value());
        else {
            auto timeInfo = dateCell.value().get<OpenXLSX::XLDateTime>().tm();
            std::ostringstream oss;
            oss << std::put_time(&timeInfo, "%d-%m-%Y %H-%M-%S");
            date = QString::fromStdString(oss.str());
        }
        QString desc = QString::fromStdString(incomplete.cell(i, 2).value());
        vector<QString> tuple{name, date, desc};
        queue->emplace_back(tuple);
    }
}

void DemandQueue::initTable(const deque<vector<QString>>* queue) {
    ui->table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    ui->table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    ui->table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->table->setColumnWidth(1, 170);
    ui->table->setRowCount(queue->size());
    for (int i = 0; i < queue->size(); i++) {
        ui->table->setItem(i, 0, new QTableWidgetItem(queue->at(i)[0]));
        ui->table->setItem(i, 1, new QTableWidgetItem(queue->at(i)[1]));
        ui->table->setItem(i, 2, new QTableWidgetItem(queue->at(i)[2]));
    }
    // Initialize ui with this connect would trigger falsely due to population
    connect(ui->table, &QTableWidget::itemChanged, this, &DemandQueue::updateHistory);
}

void DemandQueue::initHeaders(OpenXLSX::XLDocument* h, bool first)
{
    auto incomplete = h->workbook().worksheet("待完成点播");

    incomplete.cell("A1").value() = "老板名称";
    incomplete.cell("A2").value() = "点播时间";
    incomplete.cell("A3").value() = "点播内容";
//    h['待完成点播'].freeze_panes = 'A2'
//    h['待完成点播'].row_dimensions[1].height = 20
//    h['待完成点播'][1][0].alignment = Alignment(horizontal='center', vertical='center')
//    h['待完成点播'][1][1].alignment = Alignment(horizontal='center', vertical='center')
//    h['待完成点播'][1][2].alignment = Alignment(horizontal='center', vertical='center')
//    h['待完成点播'].column_dimensions['A'].width = 25
//    h['待完成点播'].column_dimensions['B'].width = 18
//    h['待完成点播'].column_dimensions['C'].width = 80

//    if first:
//        # Because completed list is not truncated
//        # after creation, do not append the headers
//        h['已完成点播'].append([
//            "老板名称",
//            "点播时间",
//            "点播内容",
//            "完成时间"
//        ])
//    h['已完成点播'].freeze_panes = 'A2'
//    h['已完成点播'].row_dimensions[1].height = 20
//    h['已完成点播'][1][0].alignment = Alignment(horizontal='center', vertical='center')
//    h['已完成点播'][1][1].alignment = Alignment(horizontal='center', vertical='center')
//    h['已完成点播'][1][2].alignment = Alignment(horizontal='center', vertical='center')
//    h['已完成点播'][1][3].alignment = Alignment(horizontal='center', vertical='center')
//    h['已完成点播'].column_dimensions['A'].width = 25
//    h['已完成点播'].column_dimensions['B'].width = 18
//    h['已完成点播'].column_dimensions['C'].width = 80
//    h['已完成点播'].column_dimensions['D'].width = 18
}

void DemandQueue::contextMenu()
{
    auto pos = QCursor::pos();
    auto item_pos = ui->table->viewport()->mapFromGlobal(pos);
    auto item = ui->table->itemAt(item_pos);
//    print(f'Dealing with item {item.text()}')
    QMenu menu = QMenu(ui->table);
    QAction show = QAction("详情", &menu);
    connect(&show, &QAction::triggered, this, [=] {showChallenge(item);});
    QAction del = QAction("删除点播", &menu);
    connect(&show, &QAction::triggered, this, [=] {deleteChallenge(item);});
    QAction complete = QAction("完成点播", &menu);
    connect(&show, &QAction::triggered, this, [=] {completeChallenge(item);});
    QAction insert = QAction("插入点播", &menu);
    connect(&show, &QAction::triggered, this, [=] {insertQueue(item->row());});
    menu.addAction(&show);
    menu.addAction(&del);
    menu.addAction(&complete);
    menu.addAction(&insert);
    menu.exec(pos);
}

void DemandQueue::showChallenge(const QTableWidgetItem* item)
{
    if(item == nullptr)
        return;
    int row = item->row();
    std::vector<QString> challenge = queue->at(row);
    QDialog dialog = QDialog(this);
    dialog.setWindowTitle("点播详情");
    dialog.setWindowIcon(QIcon("isaac.ico"));
    dialog.setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    dialog.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QVBoxLayout dialog_layout = QVBoxLayout();
    QLabel name = QLabel(challenge[0] + "的点播");
    QLabel date = QLabel("于" + challenge[1]);
    QLabel desc = QLabel(challenge[2]);
    desc.setWordWrap(true);
    dialog_layout.addWidget(&name, 0, Qt::AlignCenter);
    dialog_layout.addWidget(&desc, 0, Qt::AlignCenter);
    dialog_layout.addWidget(&date, 0, Qt::AlignCenter);
    dialog.setLayout(&dialog_layout);
    dialog.exec();
}

void DemandQueue::completeChallenge(const QTableWidgetItem* item)
{

}

void DemandQueue::deleteChallenge(const QTableWidgetItem* item)
{
    if (queue->size() == 0)
        return;
    int row = item->row();
    queue->erase(queue->begin() + row);
    ui->table->removeRow(row);
    editing = true;
}

void DemandQueue::updateHistory(const QTableWidgetItem* item)
{
    if (item == nullptr)
        return;
    int row = item->row();
    int col = item->column();
    queue->at(row)[col] = item->text();
    editing = true;
}

void DemandQueue::appendQueue()
{
    insertQueue(ui->table->rowCount());
}

void DemandQueue::cutQueue()
{
    insertQueue(0, true);
}

void DemandQueue::popQueue()
{

}

void DemandQueue::delUnformatted()
{
    if (queue->size() == 0)
        return;
    while (queue->size() > 0 && !validate(queue->at(queue->size() - 1)[1])) {
        queue->pop_back();
        ui->table->removeRow(ui->table->rowCount() - 1);
    }
    editing = true;
}

void DemandQueue::insertQueue(int row, bool cut)
{
    QString name = ui->name->toPlainText();
    QString date = ui->date->toPlainText();
    QString desc = ui->desc->toPlainText();
    std::vector<QString> tuple = {name, date, desc};
    if (cut) {
        // Cutting
        tuple[2] = "插播: " + tuple[2];
    }
    if (row == 0) {
        // Pushing
        queue->emplace_front(tuple);
    }
    else if (row == ui->table->rowCount()) {
        // Appending
        queue->emplace_back(tuple);
    }
    else {
        // Inserting
        queue->emplace(queue->begin() + row, tuple);
    }

    ui->table->insertRow(row);
    ui->table->setItem(row, 0, new QTableWidgetItem(tuple[0]));
    ui->table->setItem(row, 1, new QTableWidgetItem(tuple[1]));
    ui->table->setItem(row, 2, new QTableWidgetItem(tuple[2]));

    editing = true;
}

void DemandQueue::quickAction()
{

}

void DemandQueue::sort()
{

}

bool DemandQueue::updateXLSX()
{

    editing = false;
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
        std::vector<QString> buttons = {tr("是"), tr("否"), tr("取消")};
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

bool DemandQueue::validate(QString& dateStr) {
    if (dateStr.size() != date_format.size())
        return false;
    for (int i = 0; i < dateStr.size(); i++) {
        if(date_format[i] == '0') {
            if(!dateStr[i].isDigit())
                return false;
        }
        else {
            if(dateStr[i] != date_format[i]) {
                if(dateStr[i] == date_format_c[i])
                    // Format the full-width colon
                    dateStr[i] = date_format[i];
                else
                    return false;
            }
        }
    }
    return true;
}

DemandQueue::~DemandQueue()
{
    delete ui;
    delete shortcut;
    delete historyBook;
    delete historySheet;
    delete queue;
}

