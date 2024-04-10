#include "sldialog.h"
#include "ui_sldialog.h"
#include "sdialog.h"
#include <QMessageBox>

SLDialog::SLDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::StationListDialog)
{
    ui->setupUi(this);
    setTableWidget();
    updateTableWidget();
}

SLDialog::~SLDialog()
{
    delete ui;
}

void SLDialog::on_pushButtonAdd_clicked()
{
    SDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted)
    {
        if (runtime.m_SSL.add(dlg.getData()))
        {
            runtime.m_SSL.save();
            updateTableWidget();
            QMessageBox::information(this, "操作提示", "添加成功");
        }
        else
        {
            QMessageBox::information(this, "操作提示", "名称重复，添加失败");
        }
    }
}

void SLDialog::on_pushButtonRemove_clicked()
{
    QList<QTableWidgetItem *> items = ui->tableWidget->selectedItems();
    int count = items.count();
    if (count > 0)
    {
        if (runtime.m_SSL.remove(items.at(0)->text()))
        {
            runtime.m_SSL.save();
            updateTableWidget();
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

void SLDialog::on_pushButtonModify_clicked()
{
    QList<QTableWidgetItem *> items = ui->tableWidget->selectedItems();
    int count = items.count();
    if (count > 0)
    {
        SS *station = runtime.m_SSL.find(items.at(0)->text());
        if (station)
        {
            SDialog dlg(this);
            dlg.setData(station);
            if (dlg.exec() == QDialog::Accepted)
            {
                *station = dlg.getData();
                runtime.m_SSL.save();
                updateTableWidget();
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

void SLDialog::on_tableWidget_cellDoubleClicked(int row, int column)
{
    QTableWidgetItem *item = ui->tableWidget->item(row, 0);
    SS *station = runtime.m_SSL.find(item->text());
    if (station)
    {
        SDialog dlg(this);
        dlg.setData(station);
        if (dlg.exec() == QDialog::Accepted)
        {
            *station = dlg.getData();
            runtime.m_SSL.save();
            updateTableWidget();
            QMessageBox::information(this, "操作提示", "修改成功");
        }
    }
    else
    {
        QMessageBox::information(this, "操作提示", "修改失败");
    }
}

void SLDialog::on_lineEditKeyWord_textChanged(const QString &arg1)
{
    updateTableWidget();
}

void SLDialog::updateTableWidget()
{
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setSortingEnabled(false);
    QString keyword = ui->lineEditKeyWord->text().trimmed();
    for (auto &item : runtime.m_SSL.m_data)
    {
        if (keyword.isEmpty() || item.matching(keyword))
        {
            addTableWidget(item);
        }
    }
    ui->tableWidget->setSortingEnabled(true);
}

void SLDialog::setTableWidget()
{
    ui->tableWidget->setColumnCount(3);
    QStringList header;
    header << "站点名称"
           << "经度(lng)"
           << "纬度(lat)";
    ui->tableWidget->setHorizontalHeaderLabels(header);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    // 不可编辑模式
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // 行选定模式
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSortingEnabled(true);
    ui->tableWidget->setColumnWidth(1, 200);
    ui->tableWidget->setColumnWidth(2, 80);
    ui->tableWidget->setColumnWidth(3, 80);
}

void SLDialog::addTableWidget(const SS &station)
{
    QTableWidgetItem *item = NULL;
    int rowCount = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(rowCount);
    ui->tableWidget->setItem(rowCount, 0, new QTableWidgetItem(station.m_name));
    item = new QTableWidgetItem();
    item->setData(Qt::DisplayRole, station.m_lng);
    ui->tableWidget->setItem(rowCount, 1, item);
    item = new QTableWidgetItem();
    item->setData(Qt::DisplayRole, station.m_lat);
    ui->tableWidget->setItem(rowCount, 2, item);
}
