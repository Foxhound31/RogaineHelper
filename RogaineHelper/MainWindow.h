#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_MainWindow.h"
#include <opencv2/opencv.hpp>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

private slots:
    void on_selectMapButton_clicked();

    void on_selectMarkButton_clicked();

    void on_findMarksButton_clicked();

private:
    Ui::MainWindow ui;

    cv::Mat mapImage;
    cv::Mat markImage;
};

#endif // MAINWINDOW_H
