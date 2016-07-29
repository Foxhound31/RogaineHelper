#include "MainWindow.h"
#include <QFileDialog>
#include "opencv2/opencv.hpp"
#include "matToPixmap.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    ui.setupUi(this);
}

void MainWindow::on_selectMapButton_clicked() {
    const auto fileName = QFileDialog::getOpenFileName(nullptr, "Выберите фото карты", QString(),
                                                 QString("Изображения (*.png *.jpg)"));
    auto image = cv::imread(fileName.toStdString().c_str(), cv::IMREAD_COLOR);
    ui.mapLabel->setPixmap(cvMatToQPixmap(image));
}

