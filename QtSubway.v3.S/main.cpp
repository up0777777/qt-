#include "swindow.h"
#include <QApplication>
#include <QApplication>
#include <QFile>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile qss(":/qss/style2.qss");
    if (!qss.open(QFile::ReadOnly))
    {
        QMessageBox::critical(nullptr, "错误提示", "未能成功加载qss");
    }
    a.setStyleSheet(qss.readAll());
    SWindow w;
    w.show();
    return a.exec();
}
