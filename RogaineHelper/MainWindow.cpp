#define _USE_MATH_DEFINES
#include <cmath>

#include "MainWindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QFileInfo>
#include <QGraphicsItem>
#include <QDir>
#include "NodeEditorScene.h"
#include "NodeEditorWidget.h"
#include "MapRecognizer.h"
#include "OpencvQtTools.h"
#include "tsp.h"

const QString settingsFileName = "RogaineHelper.ini";
const QString settingsKeyPath = "filesPath";


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    mapScene(this),
    mapWidget(this),
    scaleKmInPoint(1.0)
{
    ui.setupUi(this);

    // Connect view and scene
    mapWidget.setScene(&mapScene);

    // Put NodeEditorWidget on the form
    QHBoxLayout *newLayout = new QHBoxLayout();
    newLayout->setMargin(0);
    newLayout->setSpacing(0);
    ui.mapFrame->setLayout(newLayout);
    newLayout->addWidget(&mapWidget);


    // Configure scene and connect singnals-slots for it
    connect(&mapScene, SIGNAL(itemInserted(QGraphicsItem*)), this, SLOT(itemInserted(QGraphicsItem*)));
    connect(&mapScene, SIGNAL(itemSelected(QGraphicsItem*)), this, SLOT(itemSelected(QGraphicsItem*)));

    // Mark table
    ui.nodesTable->setColumnCount(6);
    ui.nodesTable->setColumnWidth(0, 20);
    ui.nodesTable->setColumnWidth(1, 30);
    ui.nodesTable->setColumnWidth(2, 30);
    ui.nodesTable->setColumnWidth(3, 30);
    ui.nodesTable->setColumnWidth(4, 0);
    ui.nodesTable->setColumnWidth(5, 0);
    ui.nodesTable->setContentsMargins(1,1,1,1);

    QStringList headerList;
    headerList.append(QString(""));
    headerList.append(QString("№"));
    headerList.append(QString("X"));
    headerList.append(QString("Y"));
    ui.nodesTable->setHorizontalHeaderLabels(headerList);
    ui.nodesTable->verticalHeader()->setVisible(false);

    // Check settings
    QSettings settings(settingsFileName, QSettings::IniFormat);
    ui.textBrowser->append("Application settings file: " + settings.fileName());
    if (settings.allKeys().count() == 0) {
        ui.textBrowser->append("No settings in file");
    }
}

//--------------------------------------------------------------------------------------
// Open Map image file
//--------------------------------------------------------------------------------------
void MainWindow::on_openMapButton_clicked()
{
    // Open settings to read path to files
    QSettings settings(settingsFileName, QSettings::IniFormat);
    QString path = settings.value(settingsKeyPath).toString();
    if (path.isEmpty()) path = QDir::currentPath();

    // Open map file
    const auto fileName = QFileDialog::getOpenFileName(this, tr("Open map image"), path, QString("Pictures (*.png *.jpg); All files (*.*)"));
    if (fileName.isEmpty()) {
        ui.textBrowser->append("Operation cancelled");
        return;
    }

    // Create Mat frame
    mapImage = cv::imread(fileName.toStdString().c_str(), cv::IMREAD_COLOR);

    // Add map on the scene
    QGraphicsPixmapItem* newItem = new QGraphicsPixmapItem(cvMatToQPixmap(mapImage));
    newItem->setPos(0,0);
    newItem->setZValue(0);
    newItem->setAcceptedMouseButtons(0);
    newItem->setTransformationMode(Qt::SmoothTransformation);
    mapScene.addItem(newItem);
    mapScene.setSceneRect(newItem->boundingRect());
    mapWidget.show();

    // Save path into settings
    settings.setValue(settingsKeyPath, QFileInfo(fileName).absoluteDir().absolutePath());

    ui.setScaleButton->setEnabled(true);
    ui.scaleDistLabel->setEnabled(true);
    ui.scaleSpinBox->setEnabled(true);

}

//--------------------------------------------------------------------------------------
// Set map scale
//--------------------------------------------------------------------------------------
void MainWindow::on_setScaleButton_clicked()
{
    // find two and only two squares
    QPointF begin, end; // in scene coordinates
    bool moreThanTwoPoints = false; // if there are more than two square marks on the map

    foreach(QGraphicsItem *item, mapScene.items()) {
        if( item->type() == QGraphicsRectItem::Type) {
            if (begin.isNull()) {
                begin = item->mapToScene(item->boundingRect().center());
            }
            else if (end.isNull()) {
                end = item->mapToScene(item->boundingRect().center());
            }
            else {
                moreThanTwoPoints = true;
            }
        }
    }

    if (moreThanTwoPoints || begin.isNull() || end.isNull()) {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("Only two marks should be on the map to measure scale"),
                              QMessageBox::Ok);
        return;
    }

    // Save result
    scaleKmInPoint = ui.scaleSpinBox->value() / QLineF(begin, end).length();
    ui.textBrowser->append("Scene " + QString::number(QLineF(begin, end).length()) +
                           " points is " + ui.scaleSpinBox->text() + " kilometers");

    ui.textBrowser->append("Map height " + QString::number(mapScene.height()) +
                           " points, width " + QString::number(mapScene.width()) + " points");

    // Update scene scale
    double newHeight = mapScene.height() * scaleKmInPoint;
    double newWidth = mapScene.width() * scaleKmInPoint;
    ui.textBrowser->append("Map height " + QString::number(newHeight) +
                           " km, width " + QString::number(newWidth) + " km");

    //mapScene.setSceneRect(0, 0, newWidth, newHeight);

    ui.nodesGroupBox->setEnabled(true);
    ui.distanceGroupBox->setEnabled(true);
}


//--------------------------------------------------------------------------------------
// Scale changed
//--------------------------------------------------------------------------------------
void MainWindow::on_scaleSpinBox_editingFinished()
{
    on_setScaleButton_clicked();
}



//--------------------------------------------------------------------------------------
// Save Nodes file
//--------------------------------------------------------------------------------------
void MainWindow::on_saveNodesButton_clicked()
{
    // Open settings to read path to files
    QSettings settings(settingsFileName, QSettings::IniFormat);
    QString path = settings.value(settingsKeyPath).toString();
    if (path.isEmpty())
        path = QDir::currentPath();

    QString	fileName = QFileDialog::getSaveFileName(nullptr, "Save nodes", path);
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        ui.textBrowser->append("Operation failed");
        return;
    }

    QDataStream stream(&file);
    int rowCount = ui.nodesTable->rowCount();
    stream << scaleKmInPoint;
    stream << rowCount;
    for (int i = 0; i < rowCount; ++i) {
        QCheckBox* checkBox = static_cast<QCheckBox*>(ui.nodesTable->cellWidget(i, 0));
        stream << checkBox->isChecked();
        stream << ui.nodesTable->itemAt(i, 1)->text().toInt();
        stream << ui.nodesTable->itemAt(i, 2)->text().toDouble();
        stream << ui.nodesTable->itemAt(i, 3)->text().toDouble();
        stream << ui.nodesTable->itemAt(i, 4)->text().toDouble();
        stream << ui.nodesTable->itemAt(i, 5)->text().toDouble();
    }

    settings.setValue(settingsKeyPath, QFileInfo(fileName).absoluteDir().absolutePath());
}

//--------------------------------------------------------------------------------------
// Open Nodes file
//--------------------------------------------------------------------------------------
void MainWindow::on_openNodesButton_clicked()
{
    // Open settings to read path to files
    QSettings settings(settingsFileName, QSettings::IniFormat);
    QString path = settings.value(settingsKeyPath).toString();
    if (path.isEmpty())
        path = QDir::currentPath();

    // Open map file
    const auto fileName = QFileDialog::getOpenFileName(this, tr("Open nodes file"), path);
    if (fileName.isEmpty()) {
        ui.textBrowser->append("Operation cancelled");
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        ui.textBrowser->append("Operation failed");
        return;
    }

    QDataStream stream(&file);
    int rowCount;
    stream >> scaleKmInPoint;
    stream >> rowCount;
    ui.nodesTable->setRowCount(rowCount);
    for (int i = 0; i < rowCount; ++i) {
        bool isChecked;
        stream >> isChecked;
        QCheckBox * newCheckBox = new QCheckBox();
        newCheckBox->setChecked(isChecked);
        ui.nodesTable->setCellWidget(i, 0, newCheckBox);

        stream << ui.nodesTable->itemAt(i, 1)->text().toInt();
        stream << ui.nodesTable->itemAt(i, 2)->text().toDouble();
        stream << ui.nodesTable->itemAt(i, 3)->text().toDouble();
        stream << ui.nodesTable->itemAt(i, 4)->text().toDouble();
        stream << ui.nodesTable->itemAt(i, 5)->text().toDouble();

        QTableWidgetItem * newItem;
        int intNumber;
        double doubleNumber;
        stream >> intNumber;
        newItem =  new QTableWidgetItem(QString::number(intNumber));
        ui.nodesTable->setItem(i, 1, newItem);
        stream >> doubleNumber;
        newItem =  new QTableWidgetItem(QString::number(doubleNumber, 'g', 3));
        ui.nodesTable->setItem(i, 2, newItem);
        stream >> doubleNumber;
        newItem =  new QTableWidgetItem(QString::number(doubleNumber, 'g', 3));
        ui.nodesTable->setItem(i, 3, newItem);

        stream >> doubleNumber;
        newItem =  new QTableWidgetItem(QString::number(doubleNumber));
        ui.nodesTable->setItem(i, 4, newItem);
        stream >> doubleNumber;
        newItem =  new QTableWidgetItem(QString::number(doubleNumber));
        ui.nodesTable->setItem(i, 5, newItem);
    }

    // Save path into settings
    settings.setValue(settingsKeyPath, QFileInfo(fileName).absoluteDir().absolutePath());
}

//--------------------------------------------------------------------------------------
// Solve
//--------------------------------------------------------------------------------------
void MainWindow::on_solveButton_clicked()
{
    on_clearEdges_clicked();

    int scoreSln = 0;
    int nodesSln = 0;
    double distSln = 0;

    QVector<int>  distMatrix; // distamces between nodes in decades of meters
    QString       tspFileName = QDir::currentPath() + "/tspMatrix.tsp";

    // fill vector with nodes checked for solve
    nodesForSolution.clear();
    for (int i = 0; i < ui.nodesTable->rowCount(); i++) {
        if (!((QCheckBox*)ui.nodesTable->cellWidget(i, 0))->isChecked()) {
            continue;
        }
        nodeForSolution node;
        node.idxInTable = i;
        node.score = ui.nodesTable->item(i, 1)->text().toInt();
        node.xKm = ui.nodesTable->item(i, 2)->text().toDouble();
        node.yKm = ui.nodesTable->item(i, 3)->text().toDouble();
        node.xMx10 = node.xKm * 100;
        node.yMx10 = node.yKm * 100;
        node.x = ui.nodesTable->item(i, 4)->text().toDouble();
        node.y = ui.nodesTable->item(i, 5)->text().toDouble();
        scoreSln += node.score / 10;
        nodesForSolution.append(node);

        qDebug() << node.xMx10 << node.yMx10;
    }
    nodesSln = nodesForSolution.count();

    if (nodesSln < 2) {
        ui.textBrowser->append("Add at least two marks on the map");
        return;
    }


    // Prepare matrix of distances
    for (int i = 0; i < nodesSln; i++) {
        for (int j = 0; j < nodesSln; j++) {
            if (i != j) {
                distMatrix.append(pow(((nodesForSolution[i].xMx10 - nodesForSolution[j].xMx10)*(nodesForSolution[i].xMx10 - nodesForSolution[j].xMx10) +
                                       (nodesForSolution[i].yMx10 - nodesForSolution[j].yMx10)*(nodesForSolution[i].yMx10 - nodesForSolution[j].yMx10)), 0.5));
            }
            else {
                distMatrix.append(0);
            }
        }
    }
    ui.textBrowser->append("Matrix ready for " + QString::number(nodesSln) + " nodes. Matrix size " + QString::number(distMatrix.count()));
    qDebug() << distMatrix;


    // Check matrix (square, symmetrical)
    if (distMatrix.count() != nodesSln * nodesSln) {
        ui.textBrowser->append("Check matrix failed - wrong nodes count");
        return;
    }
    for (int i = 0; i < nodesSln; i++) {
        if (distMatrix[i * nodesSln + i] != 0) {
            ui.textBrowser->append("Check matrix failed - no zeros on diagonal");
            return;
        }
        for (int j = 0; j < nodesSln; j++) {
            if (distMatrix[i * nodesSln + j] != distMatrix[j * nodesSln + i]) {
                ui.textBrowser->append("Check matrix failed - not symmetrical");
                return;
            }
        }
    } 

    // Generate TSP file
    ui.textBrowser->append("TSP file name: " + tspFileName);
    try {
        tspWriteTspFile(distMatrix, tspFileName);
    }
    catch (std::runtime_error& e) {
        ui.textBrowser->append("TSP file genaration failed: " + QString::fromLocal8Bit(e.what()));
        return;
    }
    ui.textBrowser->append("TSP file ready");


    // Solve
    try {
        solution = tspRoute(tspFileName);
    }
    catch (std::runtime_error& e) {
        ui.textBrowser->append("TSP solving failed: " + QString::fromLocal8Bit(e.what()));
        return;
    }
    if (solution.count() == 0) {
        ui.textBrowser->append("Solution is empty");
        return;
    }
    ui.textBrowser->append("TSP solution ready for " + QString::number(solution.count()) + " nodes");


    // Show test solution
    QString textSolution;
    for (int i = 0; i < solution.count(); i++) {
        textSolution += QString::number(nodesForSolution[solution[i]].score) + " -> ";
    }
    ui.textBrowser->append(textSolution);


    // Draw lines
    QGraphicsLineItem * newLine;
    for (int i = 0; i < solution.count() - 1; i++) {
        newLine = new QGraphicsLineItem(nodesForSolution[solution[i]].x, nodesForSolution[solution[i]].y, nodesForSolution[solution[i+1]].x, nodesForSolution[solution[i+1]].y);
        //newItem->setFlag(QGraphicsItem::ItemIsMovable, true);
        newLine->setFlag(QGraphicsItem::ItemIsSelectable, true);
        newLine->setFlag(QGraphicsItem::ItemIsFocusable, true);
        QPen newPen(Qt::blue);
        newPen.setWidth(4);
        newLine->setPen(newPen);
        mapScene.addItem(newLine);
        distSln += newLine->line().length() * scaleKmInPoint;
    }

    // Calculate distance
    ui.nodesLabel->setText(QString::number(nodesForSolution.count()));
    ui.scoreLabel->setText(QString::number(scoreSln));
    ui.distanceLabel->setText(QString::number(distSln));
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
void MainWindow::on_nodesTable_cellChanged(int row, int column)
{
    // if score changed
    if (column == 1 && ui.nodesTable->rowCount()) {
        // summ of scores
        int sumScore = 0;
        for (int i = 0; i < ui.nodesTable->rowCount(); i++) {
            sumScore += ui.nodesTable->item(i, 1)->text().toInt() / 10;
        }
        ui.scoreTotalLabel->setText(QString::number(sumScore));
        ui.nodesTotalLabel->setText(QString::number(ui.nodesTable->rowCount()));
    }
}
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
void MainWindow::on_nodesTable_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    mapScene.clearSelection();

    if (ui.nodesTable->item(currentRow, 4) && ui.nodesTable->item(currentRow, 5)) {
        QGraphicsItem * item = mapScene.itemAt(ui.nodesTable->item(currentRow, 4)->text().toDouble(), ui.nodesTable->item(currentRow, 5)->text().toDouble(), QTransform());
        if (item && item->type() == QGraphicsEllipseItem::Type) {
            item->setSelected(true);
        }
    }
}







//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
void MainWindow::itemSelected(QGraphicsItem* item)
{
    ui.segmentDistKm->setText("0");
    ui.totalDistKm->setText("0");

    if (!item)
        return;

    if (item->type() == QGraphicsEllipseItem::Type) {
        QGraphicsEllipseItem *markItem = (QGraphicsEllipseItem*)item;
        for (int i = 0; i < ui.nodesTable->rowCount(); i++) {
            if ((ui.nodesTable->item(i, 4)->text() == QString::number(markItem->rect().center().x())) &&
                    (ui.nodesTable->item(i, 5)->text() == QString::number(markItem->rect().center().y())))   {

                ui.nodesTable->setCurrentCell(i, 1);
                ui.textBrowser->append("Mark " + ui.nodesTable->item(i, 1)->text() + " selected");

                // if no number - prepare to edit
                if (ui.nodesTable->item(i, 1)->text() == "0") {
                    ui.nodesTable->editItem(ui.nodesTable->item(i, 1));
                }
            }
        }
        ui.nodesTable->sortItems(1);
    }

    if (item->type() == CustomRect::Type) {
        ui.totalDistKm->setText(QString::number(((CustomRect*)item)->getTotalLen() * scaleKmInPoint, 'g', 3));
    }

    if (item->type() == CustomLine::Type) {
        ui.segmentDistKm->setText(QString::number(((QGraphicsLineItem*)item)->line().length() * scaleKmInPoint, 'g', 3));
        ui.totalDistKm->setText(QString::number(((CustomRect*)item)->getTotalLen() * scaleKmInPoint, 'g', 3));
    }

    if (item->type() == QGraphicsLineItem::Type) {
        ui.segmentDistKm->setText(QString::number(((QGraphicsLineItem*)item)->line().length() * scaleKmInPoint, 'g', 3));
    }
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
void MainWindow::itemInserted(QGraphicsItem*item)
{
    if (item->type() == QGraphicsEllipseItem::Type) {
        QGraphicsEllipseItem *markItem = (QGraphicsEllipseItem*)item;
        ui.textBrowser->append("New mark item at " +
                               QString::number(markItem->rect().center().x()) + ", " +
                               QString::number(markItem->rect().center().y()) + " (" +
                               QString::number(markItem->rect().center().x() * scaleKmInPoint) + ", " +
                               QString::number(markItem->rect().center().y() * scaleKmInPoint) + "km)");

        // add free row
        ui.nodesTable->setRowCount(ui.nodesTable->rowCount() + 1);
        int freeRow = ui.nodesTable->rowCount() - 1;

        QCheckBox * newCheckBox = new QCheckBox();
        newCheckBox->setChecked(true);
        ui.nodesTable->setCellWidget(freeRow, 0, newCheckBox);

        QTableWidgetItem * newItem;
        newItem =  new QTableWidgetItem("0");
        ui.nodesTable->setItem(freeRow, 1, newItem);
        newItem =  new QTableWidgetItem(QString::number(markItem->rect().center().x() * scaleKmInPoint, 'g', 3));
        ui.nodesTable->setItem(freeRow, 2, newItem);
        newItem =  new QTableWidgetItem(QString::number(markItem->rect().center().y() * scaleKmInPoint, 'g', 3));
        ui.nodesTable->setItem(freeRow, 3, newItem);

        newItem =  new QTableWidgetItem(QString::number(markItem->rect().center().x()));
        ui.nodesTable->setItem(freeRow, 4, newItem);
        newItem =  new QTableWidgetItem(QString::number(markItem->rect().center().y()));
        ui.nodesTable->setItem(freeRow, 5, newItem);

        ui.nodesTable->sortItems(1);

    }
}



//--------------------------------------------------------------------------------------
// Del button
//--------------------------------------------------------------------------------------
void MainWindow::keyPressEvent(QKeyEvent *ev)
{
    if (ev->key() == Qt::Key_Delete) {
        mapScene.deleteSelectedItems();
    }
}


//--------------------------------------------------------------------------------------
// Clear nodes - circlrs
//--------------------------------------------------------------------------------------
void MainWindow::on_clearNodesButton_clicked()
{
    ui.nodesTable->clear();
    ui.nodesTable->setRowCount(0);
    ui.nodesTotalLabel->setText("0");
    ui.scoreTotalLabel->setText("0");

    on_clearEdges_clicked();

    foreach(QGraphicsItem *item, mapScene.items()) {
        if( item->type() == QGraphicsEllipseItem::Type) {
            mapScene.removeItem(item);
            delete item;
        }
    }
}

//--------------------------------------------------------------------------------------
// Clear solution - blue lines
//--------------------------------------------------------------------------------------
void MainWindow::on_clearEdges_clicked()
{
    foreach(QGraphicsItem *item, mapScene.items()) {
        if( item->type() == QGraphicsLineItem::Type && ((QGraphicsLineItem*)item)->pen().color() == Qt::blue) {
            mapScene.removeItem(item);
            delete item;
        }
    }
    ui.nodesLabel->setText("0");
    ui.scoreLabel->setText("0");
    ui.distanceLabel->setText("0");
}

//--------------------------------------------------------------------------------------
// Clear milestones - rects and black lines
//--------------------------------------------------------------------------------------
void MainWindow::on_clearButton_clicked()
{
    foreach(QGraphicsItem *item, mapScene.items()) {
        if( item->type() == QGraphicsRectItem::Type || (item->type() == QGraphicsLineItem::Type && ((QGraphicsLineItem*)item)->pen().color() == Qt::black)) {
            mapScene.removeItem(item);
            delete item;
        }
    }
}


//--------------------------------------------------------------------------------------
// Set nodes
//--------------------------------------------------------------------------------------
void MainWindow::on_addNodeButton_toggled(bool checked)
{
    if (checked) {
        ui.measureButton->setChecked(false);
        mapScene.setClickMode(CLICKMODE_ADD_CIRCLE);
    }
    else {
        mapScene.setClickMode(CLICKMODE_SELECT);
    }
}

//--------------------------------------------------------------------------------------
// Set milestones
//--------------------------------------------------------------------------------------
void MainWindow::on_measureButton_toggled(bool checked)
{
    if (checked) {
        ui.addNodeButton->setChecked(false);
        mapScene.setClickMode(CLICKMODE_ADD_RECT);
    }
    else {
        mapScene.setClickMode(CLICKMODE_SELECT);
    }
}

















//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
void MainWindow::on_selectNodeButton_clicked()
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
void MainWindow::on_findNodeButton_clicked()
{
    cv::Mat result;
    cv::Mat resultNormalized;
    //cv::Mat channels[3];
    //cv::split(mapImage, channels);
    //mapImage = channels[1]; // B[G]R
    //cv::matchTemplate(mapImage, markImage, result, cv::TM_SQDIFF_NORMED);
    //cv::normalize(result, resultNormalized, 0, 255, cv::NORM_MINMAX);
    //ui.mapLabel->setPixmap(cvMatToQPixmap(result));
    //cv::Mat mat2;
    //resultNormalized.convertTo(mat2, CV_8UC1);

    cv::Mat channels[3];
    cv::split(markImage, channels);

    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(channels[1], circles, cv::HOUGH_GRADIENT, 1, channels[1].rows/2, channels[1].rows/4, channels[1].rows/2);
    assert(!circles.empty());
    cv::Mat markImage2;
    cv::cvtColor(channels[1], markImage2, cv::COLOR_GRAY2BGR);

    // Draw the circles detected
    assert(circles.size() == 1);
    for (size_t i = 0; i < circles.size(); ++i) {
        cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);
        cv::circle(markImage2, center, radius, cv::Scalar(0,0,255), 2, 8, 0);// circle outline
    }
    const int radius = cvRound(circles[0][2]);
    //cv::imshow("", markImage2);
    const auto avgColor = averageCircleColor(markImage, circles[0]);

    QString text2 = QString("(") + QString::number(avgColor[0]) + QString(",") +
            QString::number(avgColor[1]) + QString(",") +
            QString::number(avgColor[2]) + QString(")");

    // find circles on map with Hough
    cv::split(mapImage, channels);
    circles.clear();
    cv::HoughCircles(channels[1], circles, cv::HOUGH_GRADIENT,
            1,          // Inverse ratio of the accumulator resolution to the image resolution
            radius*2,   // Minimum distance between the centers of the detected circles
            radius*0.8, // the higher threshold of the two passed to the Canny edge detector (the lower one is twice smaller)
            20,         // the accumulator threshold for the circle centers at the detection stage. The smaller it is,
            // the more false circles may be detected. Circles,
            // corresponding to the larger accumulator values, will be returned first
            radius*0.8, radius*1.2); // min and max radius
    cv::Mat mapImage2;
    cv::cvtColor(channels[1], mapImage2, cv::COLOR_GRAY2BGR);
    for (size_t i = 0; i < circles.size(); ++i) {
        const cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        const int radius = cvRound(circles[i][2]);
        //const auto avgMapColor = averageCircleColor(mapImage, circles[i]);
        cv::Vec3b avgMapColor;
        int d0 = static_cast<int>(avgColor[0]) - static_cast<int>(avgMapColor[0]);
        int d1 = static_cast<int>(avgColor[1]) - static_cast<int>(avgMapColor[1]);
        int d2 = static_cast<int>(avgColor[2]) - static_cast<int>(avgMapColor[2]);
        double result = std::max({d0, d1, d2});
        cv::Scalar scalar;
        if (result < 50)
            scalar = cv::Scalar(0,0,255);
        else
            scalar = cv::Scalar(255,0,0);
        cv::circle(mapImage2, center, radius, scalar, 1, 8, 0);// circle outline
    }
    QString text = QString("found ") + QString::number(circles.size()) + QString("circles");
    cv::putText(mapImage2, text.toStdString().c_str(),  cv::Point(10, 40), cv::FONT_HERSHEY_DUPLEX, 1.5, cv::Scalar(0));
    cv::putText(mapImage2, text2.toStdString().c_str(), cv::Point(10, 80), cv::FONT_HERSHEY_DUPLEX, 1.5, cv::Scalar(0));
    cv::imshow("", mapImage2);

    cv::Mat frameResult;
    cv::Canny(channels[1], frameResult, 5, 100);
    cv::imshow("2", frameResult);
}




//        int ret = QMessageBox::warning(this,
//                                       tr("Bindings debug"),
//                                       tr("Do you want to delete all binding records? "),
//                                       QMessageBox::Yes|QMessageBox::No);
//        if (ret != QMessageBox::Yes) {
//            ui->textBrowser->append("Operation is cancelled");
//            return;
//        }








