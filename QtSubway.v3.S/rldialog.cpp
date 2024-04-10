#include "rldialog.h"
#include "ui_rldialog.h"
#include "rdialog.h"
#include <QMessageBox>

RLDialog::RLDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::RouteListDialog)
{
    ui->setupUi(this);
    setTableWidget();
    updateTableWidget();
    updateTableWidgetStations();
}

RLDialog::~RLDialog()
{
    delete ui;
}

void RLDialog::on_pushButtonAdd_clicked()
{
    RDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted)
    {
        if (runtime.m_RRL.add(dlg.getData()))
        {
            runtime.m_RRL.save();
            updateTableWidget();
            updateTableWidgetStations();
            QMessageBox::information(this, "操作提示", "添加成功");
        }
        else
        {
            QMessageBox::information(this, "操作提示", "名称重复，添加失败");
        }
    }
}

void RLDialog::on_pushButtonRemove_clicked()
{
    QList<QTableWidgetItem *> items = ui->tableWidget->selectedItems();
    int count = items.count();
    if (count > 0)
    {
        if (runtime.m_RRL.remove(items.at(0)->text()))
        {
            runtime.m_RRL.save();
            updateTableWidget();
            updateTableWidgetStations();
            QMessageBox::information(this, "操作提示", "删除成功");
        }
        else
        {
            QMessageBox::information(this, "操作提示", "删除失败");
        }
    }
    else
    {
        QMessageBox::information(this, "操作提示", "删除失败，未选中要删除的数据项");
    }
}

void RLDialog::on_pushButtonModify_clicked()
{
    QList<QTableWidgetItem *> items = ui->tableWidget->selectedItems();
    int count = items.count();
    if (count > 0)
    {
        RR *route = runtime.m_RRL.find(items.at(0)->text());
        if (route)
        {
            RDialog dlg(this);
            dlg.setData(route);
            if (dlg.exec() == QDialog::Accepted)
            {
                *route = dlg.getData();
                runtime.m_RRL.save();
                updateTableWidget();
                updateTableWidgetStations();
                QMessageBox::information(this, "操作提示", "修改成功");
            }
        }
        else
        {
            QMessageBox::information(this, "操作提示", "修改失败");
        }
    }
    else
    {
        QMessageBox::information(this, "操作提示", "修改失败，未选中要修改的数据项");
    }
}

void RLDialog::on_tableWidget_cellDoubleClicked(int row, int column)
{
    QTableWidgetItem *item = ui->tableWidget->item(row, 0);
    RR *route = runtime.m_RRL.find(item->text());
    if (route)
    {
        RDialog dlg(this);
        dlg.setData(route);
        if (dlg.exec() == QDialog::Accepted)
        {
            *route = dlg.getData();
            runtime.m_RRL.save();
            updateTableWidget();
            updateTableWidgetStations();
            QMessageBox::information(this, "操作提示", "修改成功");
        }
    }
    else
    {
        QMessageBox::information(this, "操作提示", "修改失败");
    }
}

void RLDialog::on_lineEditKeyWord_textChanged(const QString &arg1)
{
    updateTableWidget();
    updateTableWidgetStations();
}

void RLDialog::updateTableWidget()
{
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setSortingEnabled(false);
    QString keyword = ui->lineEditKeyWord->text().trimmed();
    for (auto &item : runtime.m_RRL.m_data)
    {
        if (keyword.isEmpty() || item.matching(keyword))
        {
            addTableWidget(item);
        }
    }
    ui->tableWidget->setSortingEnabled(true);
}

void RLDialog::setTableWidget()
{
    ui->tableWidget->setColumnCount(6);
    QStringList header;
    header << "线路编号"
           << "线路名称"
           << "是否环路"
           << "是否启用"
           << "线路颜色"
           << "站点数量";
    ui->tableWidget->setHorizontalHeaderLabels(header);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    // 不可编辑模式
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // 行选定模式
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSortingEnabled(true);
    ui->tableWidget->setColumnWidth(1, 150);
    ui->tableWidget->setColumnWidth(2, 150);
    ui->tableWidget->setColumnWidth(4, 80);
}

void RLDialog::addTableWidget(const RR &route)
{
    QTableWidgetItem *item = NULL;
    int rowCount = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(rowCount);
    ui->tableWidget->setItem(rowCount, 0, new QTableWidgetItem(route.m_id));
    ui->tableWidget->setItem(rowCount, 1, new QTableWidgetItem(route.m_name));
    ui->tableWidget->setItem(rowCount, 2, new QTableWidgetItem(route.m_ring));
    ui->tableWidget->setItem(rowCount, 3, new QTableWidgetItem(route.m_enabled));
    item = new QTableWidgetItem();
    item->setBackground(route.getColor());
    ui->tableWidget->setItem(rowCount, 4, item);
    // 按数值类型排序
    item = new QTableWidgetItem();
    item->setData(Qt::DisplayRole, route.m_stations.size());
    ui->tableWidget->setItem(rowCount, 5, item);
}

void RLDialog::updateTableWidgetStations()
{
    ui->tableWidgetStations->setRowCount(0);
    ui->tableWidgetStations->setColumnCount(4);
    QStringList header;
    header << "线路编号"
           << "线路名称"
           << "站点编号"
           << "站点名称";
    ui->tableWidgetStations->setHorizontalHeaderLabels(header);
    ui->tableWidgetStations->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidgetStations->setSelectionBehavior(QAbstractItemView::SelectRows);
    // 不可编辑模式
    ui->tableWidgetStations->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // 行选定模式
    ui->tableWidgetStations->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->tableWidgetStations->setSortingEnabled(false);
    QString keyword = ui->lineEditKeyWord->text().trimmed();
    for (auto route : runtime.m_RRL.m_data)
    {
        int index = 0;
        for (auto &station : route.m_stations)
        {
            if (keyword.isEmpty() || route.m_id.indexOf(keyword) != -1 || route.m_name.indexOf(keyword) != -1 || station.indexOf(keyword) != -1)
            {
                QTableWidgetItem *item = NULL;
                int rowCount = ui->tableWidgetStations->rowCount();
                ui->tableWidgetStations->insertRow(rowCount);
                ui->tableWidgetStations->setItem(rowCount, 0, new QTableWidgetItem(route.m_id));
                ui->tableWidgetStations->setItem(rowCount, 1, new QTableWidgetItem(route.m_name));
                // 按数值类型排序
                item = new QTableWidgetItem();
                item->setData(Qt::DisplayRole, index + 1);
                ui->tableWidgetStations->setItem(rowCount, 2, item);
                ui->tableWidgetStations->setItem(rowCount, 3, new QTableWidgetItem(station));
            }
            ++index;
        }
    }

    ui->tableWidgetStations->setSortingEnabled(true);
}
