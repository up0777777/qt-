#include "swindow.h"
#include "ui_swindow.h"
#include "rdialog.h"
#include "sldialog.h"
#include "rldialog.h"
#include <QPainter>
#include <QCompleter>
#include <QMessageBox>
#include <QDebug>

SWindow::SWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::SubwayWindow)
{
    ui->setupUi(this);
    // 控件事件过滤，使能够在控件上绘图
    ui->labelCanvas->installEventFilter(this);
    ui->horizontalSliderZoom->setRange(100, 1000);
    ui->horizontalSliderZoom->setValue((int)runtime.m_R * 100);
    ui->scrollAreaWidgetContents->resize(runtime.width(), runtime.height());
    ui->comboBoxCategory->addItems(QStringList() << "最少时间"
                                                 << "最少换乘");
    ui->comboBoxCategory->setCurrentIndex(0);
    ui->checkBoxName->setChecked(true);

    connect(ui->lineEditStart, SIGNAL(returnPressed()), ui->pushButtonNavigation, SIGNAL(clicked()), Qt::UniqueConnection);
    connect(ui->lineEditEnd, SIGNAL(returnPressed()), ui->pushButtonNavigation, SIGNAL(clicked()), Qt::UniqueConnection);

    updateInputPrompt();
    checkNotFoundStations();
}

SWindow::~SWindow()
{
    delete ui;
}

void SWindow::on_pushButtonStation_clicked()
{
    SLDialog dlg(this);
    dlg.exec();
    runtime.update();
    updateInputPrompt();
    checkNotFoundStations();
    update();
}

void SWindow::on_pushButtonRoute_clicked()
{
    RLDialog dlg(this);
    dlg.exec();
    runtime.update();
    updateInputPrompt();
    checkNotFoundStations();
    update();
}

void SWindow::on_pushButtonNavigation_clicked()
{
    QString start = ui->lineEditStart->text().trimmed();
    if (start.isEmpty())
    {
        QMessageBox::information(this, "操作提示", "请输入起点站名称");
        return;
    }

    QString end = ui->lineEditEnd->text().trimmed();
    if (end.isEmpty())
    {
        QMessageBox::information(this, "操作提示", "请输入终点站名称");
        return;
    }
    if (!runtime.hasSS(start))
    {
        QMessageBox::information(this, "操作提示", "输入的起点站名称不存在");
        return;
    }
    if (!runtime.hasSS(end))
    {
        QMessageBox::information(this, "操作提示", "输入的终点站名称不存在");
        return;
    }
    if (start == end)
    {
        QMessageBox::information(this, "操作提示", "输入的起点站和终点站不能相同");
        return;
    }
    switch (ui->comboBoxCategory->currentIndex())
    {
    case 0:
        runtime.nD(start, end);
        break;
    case 1:
        runtime.nT(start, end);
        break;
    }

    ui->listWidgetOutput->clear();
    if (!runtime.m_AT.empty())
    {
        int transfer = 0;
        double km = 0;
        RR *last = NULL;
        for (int index = 0; index < runtime.m_AT.size(); ++index)
        {
            auto &segment = runtime.m_AT[index];
            RR *route = runtime.getRR(std::get<0>(segment), std::get<1>(segment));
            if (last && last != route)
            {
                ui->listWidgetOutput->addItem(QString(" >> 在 [%1] 换乘 [%2] >>").arg(std::get<0>(segment)).arg(route->m_name));
                ++transfer;
            }

            if (index == 0)
            {
                ui->listWidgetOutput->addItem(QString("[起点站] %1 (%2)").arg(std::get<0>(segment)).arg(route->m_name));
            }

            if (index == runtime.m_AT.size() - 1)
            {
                ui->listWidgetOutput->addItem(QString("[终点站] -> %1  (%2km)").arg(std::get<1>(segment)).arg(std::get<2>(segment)));
            }
            else
            {
                ui->listWidgetOutput->addItem(QString("[第%1站] -> %2  (%3km)").arg(index + 1).arg(std::get<1>(segment)).arg(std::get<2>(segment)));
            }
            km += std::get<2>(segment);
            last = route;
        }
        ui->listWidgetOutput->addItem(QString("【站点数：%1，换乘数：%2】").arg(runtime.m_AT.size()).arg(transfer));
        ui->listWidgetOutput->addItem(QString("【总里程：%1km，费用：%2元】").arg(km).arg(runtime.countC(km)));
    }
    else
    {
        ui->listWidgetOutput->addItem(QString("未找到可行的方案！"));
    }
    update();
}

void SWindow::on_horizontalSliderZoom_sliderMoved(int position)
{
    runtime.m_R = (double)position / 100;
    runtime.update();
    ui->scrollAreaWidgetContents->resize(runtime.width(), runtime.height());
    update();
}

void SWindow::on_horizontalSliderZoom_valueChanged(int value)
{
    runtime.m_R = (double)value / 100;
    runtime.update();
    ui->scrollAreaWidgetContents->resize(runtime.width(), runtime.height());
    update();
}

void SWindow::on_checkBoxName_toggled(bool checked)
{
    update();
}

void SWindow::on_checkBoxDistance_toggled(bool checked)
{
    update();
}

void SWindow::on_checkBoxCoordinates_toggled(bool checked)
{
    update();
}

void SWindow::paintEvent(QPaintEvent *event)
{
}

void SWindow::resizeEvent(QResizeEvent *event)
{
    update();
}

bool SWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->labelCanvas)
    {
        if (event->type() == QEvent::Paint)
        {
            rawCanvas();
        }
    }
    return QWidget::eventFilter(obj, event);
}

void SWindow::mouseMoveEvent(QMouseEvent *event)
{
}

void SWindow::mousePressEvent(QMouseEvent *event)
{
    //    QPoint point = ui->labelCanvas->mapFromGlobal(event->globalPos());
}

void SWindow::mouseReleaseEvent(QMouseEvent *event)
{
}

void SWindow::wheelEvent(QWheelEvent *event)
{
    //    QPoint point = ui->labelCanvas->mapFromGlobal(event->globalPos());
    //    if(point.x() >=0 && point.x() < ui->labelCanvas->width() && point.y() >= 0 && point.y() < ui->labelCanvas->height()) {

    //        double last_x = point.x() + runtime.m_offset_x;
    //        double last_y = point.y() + runtime.m_offset_y;

    //        int radio = ui->horizontalSliderZoom->value();
    //        int delta = event->delta();
    //        if (delta > 0) {
    //            if(ui->horizontalSliderZoom->value() < ui->horizontalSliderZoom->maximum()) {
    //                radio += 5;
    //            }
    //        } else if (delta < 0) {
    //            if(ui->horizontalSliderZoom->value() > ui->horizontalSliderZoom->minimum()) {
    //                radio -= 5;
    //            }
    //        }

    //        double new_x = last_x * ((double)radio / 100);
    //        double new_y = last_y * ((double)radio / 100);

    //        runtime.m_offset_x = (last_x - new_x);
    //        runtime.m_offset_y = (last_y - new_y);

    //        ui->horizontalSliderZoom->setValue(radio);

    //        qDebug() << last_x << " " << last_y << " | " << new_x << " " << new_y << " | " << runtime.m_offset_x << " " << runtime.m_offset_y;
    //    }

    //    QWidget::wheelEvent(event);
}

void SWindow::rawCanvas()
{
    QPainter painter(ui->labelCanvas);
    painter.setRenderHints(QPainter::Antialiasing, true);

    for (auto &route : runtime.m_RRL.m_data)
    {
        if (route.m_enabled == "开启" && route.m_stations.size())
        {
            QStringList stations = route.m_stations;
            if (route.m_ring == "始终相同")
            {
                stations.append(stations[0]);
            }
            SS *from = runtime.m_SSL.find(stations[0]);
            for (int index = 1; index < stations.size(); ++index)
            {
                SS *to = runtime.m_SSL.find(stations[index]);
                if (from && to)
                {
                    painter.setPen(QPen(QColor(route.getColor()), 3));
                    painter.drawLine(QPointF(from->x(), from->y()), QPointF(to->x(), to->y()));
                }
                from = to;
            }
        }
    }

    if (ui->checkBoxDistance->isChecked())
    {
        for (auto &route : runtime.m_RRL.m_data)
        {
            if (route.m_enabled == "开启" && route.m_stations.size())
            {
                QStringList stations = route.m_stations;
                if (route.m_ring == "始终相同")
                {
                    stations.append(stations[0]);
                }
                SS *from = runtime.m_SSL.find(stations[0]);
                for (int index = 1; index < stations.size(); ++index)
                {
                    SS *to = runtime.m_SSL.find(stations[index]);
                    if (from && to)
                    {
                        QFont font;
                        font.setPointSize(9);
                        painter.setFont(font);
                        painter.setPen(QPen(QColor(255, 0, 0)));
                        painter.drawText((from->x() + to->x()) / 2, (from->y() + to->y()) / 2, QString("%1").arg(from->distance(*to)));
                    }
                    from = to;
                }
            }
        }
    }

    for (auto &station : runtime.m_SSL.m_data)
    {
        if (station.m_line_count == 1)
        {
            painter.setPen(Qt::NoPen);
            painter.setBrush(QBrush(QColor(255, 0, 0)));
            painter.drawEllipse(QPointF(station.x(), station.y()), runtime.m_SC, runtime.m_SC);
            painter.setBrush(QBrush(QColor(255, 255, 255)));
            painter.drawEllipse(QPointF(station.x(), station.y()), runtime.m_SC - 2, runtime.m_SC - 2);
        }
        else if (station.m_line_count > 1)
        {
            painter.setPen(Qt::NoPen);
            painter.setBrush(QBrush(QColor(255, 0, 0)));
            painter.drawEllipse(QPointF(station.x(), station.y()), runtime.m_SC + 1, runtime.m_SC + 1);
            painter.setBrush(QBrush(QColor(255, 255, 0)));
            painter.drawEllipse(QPointF(station.x(), station.y()), runtime.m_SC - 2, runtime.m_SC - 2);
        }

        if (station.m_line_count > 0)
        {
            if (ui->checkBoxName->isChecked())
            {
                QFont font;
                font.setPointSize(9);
                painter.setFont(font);
                painter.setBrush(QBrush(QColor(255, 0, 0, 128)));
                painter.setPen(QPen(QColor(0, 0, 0)));
                painter.drawText(station.x() + runtime.m_SC, station.y() - runtime.m_SC, station.m_name);
            }
            if (ui->checkBoxCoordinates->isChecked())
            {
                double area = runtime.m_SC * 30;
                painter.setPen(QPen(QColor(128, 128, 128)));
                painter.drawText(QRectF(station.x() - area / 2, station.y() - area / 2 + runtime.m_SC, area, area), Qt::AlignHCenter | Qt::AlignVCenter, QString("%1°,%2°").arg(station.m_lng).arg(station.m_lat));
            }
        }
    }

    if (!runtime.m_AT.empty())
    {
        for (auto &segment : runtime.m_AT)
        {
            painter.setPen(QPen(QColor(0, 0, 255, 128), 10));
            SS *from = runtime.m_SSL.find(std::get<0>(segment));
            SS *to = runtime.m_SSL.find(std::get<1>(segment));
            if (from && to)
            {
                painter.drawLine(QPointF(from->x(), from->y()), QPointF(to->x(), to->y()));
            }
        }
        SS *start = runtime.m_SSL.find(std::get<0>(*runtime.m_AT.begin()));
        SS *end = runtime.m_SSL.find(std::get<1>(runtime.m_AT.back()));
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(QColor(255, 0, 128)));
        if (start && end)
        {
            painter.drawEllipse(QPointF(start->x(), start->y()), runtime.m_SC * 1.5, runtime.m_SC * 1.5);
            painter.drawEllipse(QPointF(end->x(), end->y()), runtime.m_SC * 1.5, runtime.m_SC * 1.5);
        }
    }
}

void SWindow::updateInputPrompt()
{
    QStringList stations;
    for (auto &station : runtime.m_SSL.m_data)
    {
        stations << station.m_name;
    }

    QCompleter *start = new QCompleter(stations, this);
    start->setFilterMode(Qt::MatchContains);
    ui->lineEditStart->setCompleter(start);
    QCompleter *end = new QCompleter(stations, this);
    end->setFilterMode(Qt::MatchContains);
    ui->lineEditEnd->setCompleter(end);
}

void SWindow::checkNotFoundStations()
{
    QStringList list = runtime.NotSS();
    if (!list.empty())
    {
        qDebug() << "**未找到的站点信息**";
        for (auto &item : list)
        {
            qDebug() << item;
        }
    }
}
