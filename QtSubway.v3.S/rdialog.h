#ifndef ROUTEDIALOG_H
#define ROUTEDIALOG_H

#include <QDialog>
#include "my.h"

namespace Ui
{
    class RouteDialog;
}

class RDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RDialog(QWidget *parent = nullptr);
    ~RDialog();

private slots:
    void on_pushButton_clicked();
    void on_pushButtonColor_clicked();

public:
    void setData(const RR *route);
    const RR &getData() const;

private:
    Ui::RouteDialog *ui;
    RR m_route;
};

#endif
