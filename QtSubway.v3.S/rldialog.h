#ifndef ROUTELISTDIALOG_H
#define ROUTELISTDIALOG_H

#include <QDialog>
#include "my.h"

namespace Ui
{
    class RouteListDialog;
}

class RLDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RLDialog(QWidget *parent = nullptr);
    ~RLDialog();

private slots:
    void on_pushButtonAdd_clicked();
    void on_pushButtonRemove_clicked();
    void on_pushButtonModify_clicked();
    void on_tableWidget_cellDoubleClicked(int row, int column);
    void on_lineEditKeyWord_textChanged(const QString &arg1);

private:
    void updateTableWidget();
    void setTableWidget();
    void addTableWidget(const RR &route);

private:
    void updateTableWidgetStations();

private:
    Ui::RouteListDialog *ui;
};

#endif
