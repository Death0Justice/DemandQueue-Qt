#ifndef DEMANDQUEUE_H
#define DEMANDQUEUE_H

#include "qshortcut.h"
#include "qtablewidget.h"
#include <QWidget>
#include <deque>
#include <OpenXLSX.hpp>

QT_BEGIN_NAMESPACE
namespace Ui { class DemandQueue; }
QT_END_NAMESPACE

class DemandQueue : public QWidget
{
    Q_OBJECT

public:
    DemandQueue(QWidget *parent = nullptr);
    void closeEvent(QCloseEvent *event) override;
    ~DemandQueue();

private:
    Ui::DemandQueue *ui;
    bool editing = false;
    QShortcut* shortcut;
    OpenXLSX::XLDocument* historyBook;
    OpenXLSX::XLSheet* historySheet;
    std::deque<std::vector<QString>>* queue;
    const QString date_format = "0000-00-00 00:00";
    const QString date_format_c = "0000-00-00 00：00";

    void initShortcut();
    void initHistory();
    void initTable(const std::deque<std::vector<QString>>* queue);
    void initHeaders(OpenXLSX::XLDocument* h, bool first=false);
    bool validate(QString& dateStr);

private slots:
    void showChallenge(const QTableWidgetItem* item);
    void deleteChallenge(const QTableWidgetItem* item);
    void completeChallenge(const QTableWidgetItem* item);
    void updateHistory(const QTableWidgetItem* item);
    bool updateXLSX();
    void contextMenu();
    void appendQueue();
    void cutQueue();
    void insertQueue(int row, bool cut = false);
    void popQueue();
    void delUnformatted();
    void quickAction();
    void sort();
};
#endif // DEMANDQUEUE_H
