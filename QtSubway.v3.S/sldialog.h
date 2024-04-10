#ifndef STATIONLISTDIALOG_H
#define STATIONLISTDIALOG_H

#include <QDialog>
#include "my.h"

namespace Ui
{
    class StationListDialog;
}

class SLDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SLDialog(QWidget *parent = nullptr);
    ~SLDialog();

private slots:
    void on_pushButtonAdd_clicked();
    void on_pushButtonRemove_clicked();
    void on_pushButtonModify_clicked();
    void on_tableWidget_cellDoubleClicked(int row, int column);
    void on_lineEditKeyWord_textChanged(const QString &arg1);

private:
    void updateTableWidget();
    void setTableWidget();
    void addTableWidget(const SS &station);

private:
    Ui::StationListDialog *ui;
};

#endif
