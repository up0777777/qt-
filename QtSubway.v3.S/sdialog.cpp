#include "sdialog.h"
#include "ui_sdialog.h"
#include <QMessageBox>
#include <QColorDialog>
#include <QDebug>

SDialog::SDialog(QWidget *parent) : QDialog(parent),
                                    ui(new Ui::StationDialog)
{
    ui->setupUi(this);
    ui->doubleSpinBoxLat->setRange(0, 99999);
    ui->doubleSpinBoxLng->setRange(0, 99999);
}

SDialog::~SDialog()
{
    delete ui;
}

void SDialog::on_pushButton_clicked()
{
    m_station.m_name = ui->lineEditName->text().trimmed();
    if (m_station.m_name.isEmpty())
    {
        QMessageBox::information(this, "操作提示", "请输入站点名称");
        return;
    }

    m_station.m_lat = ui->doubleSpinBoxLat->value();
    m_station.m_lng = ui->doubleSpinBoxLng->value();

    accept();
}

void SDialog::setData(const SS *station)
{
    m_station = *station;
    ui->lineEditName->setReadOnly(true);
    ui->lineEditName->setText(m_station.m_name);
    ui->doubleSpinBoxLat->setValue(m_station.m_lat);
    ui->doubleSpinBoxLng->setValue(m_station.m_lng);
}

const SS &SDialog::getData() const
{
    return m_station;
}
