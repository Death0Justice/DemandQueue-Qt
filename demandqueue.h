#ifndef DEMANDQUEUE_H
#define DEMANDQUEUE_H

#include "qshortcut.h"
#include "qtablewidget.h"
#include <QWidget>
#include <deque>

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
    bool editing = true;
    QShortcut* shortcut = nullptr;

    QShortcut* initShortcut();
    void initHistory();
    void initUI();
    QTableWidget* constructTable(std::deque<std::vector<QString>> queue);
    QString validate(QString dateStr);

private slots:
    void contextMenu();
    void showChallenge(const QTableWidgetItem& item);
    void deleteChallenge(const QTableWidgetItem& item);
    void completeChallenge(const QTableWidgetItem& item);
    void updateHistory(const QTableWidgetItem& item);
    bool updateXLSX();
    void appendQueue();
    void cutQueue();
    void insertQueue(int row, bool cut = false);
    void popQueue();
    void delUnformatted();
    void quickAction();
    void sort();
};
#endif // DEMANDQUEUE_H
