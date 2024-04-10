#ifndef STATIONDIALOG_H
#define STATIONDIALOG_H

#include <QDialog>
#include "my.h"

namespace Ui
{
    class StationDialog;
}

class SDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SDialog(QWidget *parent = nullptr);
    ~SDialog();

private slots:
    void on_pushButton_clicked();

public:
    void setData(const SS *station);
    const SS &getData() const;

private:
    Ui::StationDialog *ui;
    SS m_station;
};

#endif
