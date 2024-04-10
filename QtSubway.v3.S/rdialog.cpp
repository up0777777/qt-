#include "rdialog.h"
#include "ui_rdialog.h"
#include <QMessageBox>
#include <QColorDialog>
#include <QDebug>

RDialog::RDialog(QWidget *parent) : QDialog(parent),
                                    ui(new Ui::RouteDialog)
{
    ui->setupUi(this);
    ui->lineEditColor->setReadOnly(true);
    ui->comboBoxRing->addItem("始终不同");
    ui->comboBoxRing->addItem("始终相同");
    ui->comboBoxRing->setCurrentIndex(0);
    ui->comboBoxEnabled->addItem("开启");
    ui->comboBoxEnabled->addItem("关闭");
    ui->comboBoxEnabled->setCurrentIndex(0);
}

RDialog::~RDialog()
{
    delete ui;
}

void RDialog::on_pushButton_clicked()
{
    m_route.m_id = ui->lineEditID->text().trimmed();
    if (m_route.m_id.isEmpty())
    {
        QMessageBox::information(this, "操作提示", "请输入线路编号");
        return;
    }

    m_route.m_name = ui->lineEditName->text().trimmed();
    if (m_route.m_name.isEmpty())
    {
        QMessageBox::information(this, "操作提示", "请输入线路名称");
        return;
    }

    m_route.m_ring = ui->comboBoxRing->currentText();
    m_route.m_enabled = ui->comboBoxEnabled->currentText();
    m_route.m_color = ui->lineEditColor->text().trimmed();
    if (m_route.m_color.isEmpty())
    {
        QMessageBox::information(this, "操作提示", "请输入线路颜色");
        return;
    }

    m_route.setStations(ui->textEditStations->toPlainText().trimmed());

    accept();
}

void RDialog::on_pushButtonColor_clicked()
{
    QColorDialog dlg(m_route.getColor(), this);
    dlg.setWindowTitle("选择颜色");
    if (dlg.exec() == QDialog::Accepted)
    {
        QColor color = dlg.currentColor();
        m_route.setColor(color);
        ui->lineEditColor->setText(m_route.m_color);
        ui->pushButtonColor->setStyleSheet(QString("background-color:rgb(%1)").arg(m_route.m_color));
    }
}

void RDialog::setData(const RR *route)
{
    m_route = *route;
    ui->lineEditID->setText(m_route.m_id);
    ui->lineEditID->setReadOnly(true);
    ui->lineEditName->setText(m_route.m_name);
    ui->comboBoxRing->setCurrentText(m_route.m_ring);
    ui->comboBoxEnabled->setCurrentText(m_route.m_enabled);
    ui->lineEditColor->setText(m_route.m_color);
    ui->textEditStations->setText(m_route.getStations());
    ui->pushButtonColor->setStyleSheet(QString("background-color:rgb(%1)").arg(m_route.m_color));
    if (route->m_enabled == "开启")
    {
    }
}

const RR &RDialog::getData() const
{
    return m_route;
}
