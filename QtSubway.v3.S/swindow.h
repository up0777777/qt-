#ifndef SUBWAYWINDOW_H
#define SUBWAYWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include "my.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class SubwayWindow;
}
QT_END_NAMESPACE

class SWindow : public QMainWindow
{
    Q_OBJECT

public:
    SWindow(QWidget *parent = nullptr);
    ~SWindow();

private slots:
    void on_pushButtonStation_clicked();
    void on_pushButtonRoute_clicked();
    void on_pushButtonNavigation_clicked();
    void on_horizontalSliderZoom_sliderMoved(int position);
    void on_horizontalSliderZoom_valueChanged(int value);
    void on_checkBoxName_toggled(bool checked);
    void on_checkBoxDistance_toggled(bool checked);
    void on_checkBoxCoordinates_toggled(bool checked);

private:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual bool eventFilter(QObject *obj, QEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;

private:
    void rawCanvas();

private:
    void updateInputPrompt();
    void checkNotFoundStations();

private:
    Ui::SubwayWindow *ui;
};
#endif
