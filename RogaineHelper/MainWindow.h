#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_MainWindow.h"
#include "NodeEditorScene.h"
#include "NodeEditorWidget.h"
#include <opencv2/opencv.hpp>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

private slots:
    void on_openMapButton_clicked();
    void on_selectNodeButton_clicked();
    void on_findNodeButton_clicked();
    void on_openNodesButton_clicked();
    void on_setScaleButton_clicked();
    void on_solveButton_clicked();
    void on_scaleSpinBox_editingFinished();
    void on_clearButton_clicked();
    void on_measureButton_toggled(bool checked);
    void on_addNodeButton_toggled(bool checked);
    void on_clearNodesButton_clicked();
    void on_clearEdges_clicked();
    void on_nodesTable_cellChanged(int row, int column);

    void itemSelected(QGraphicsItem*item);
    void itemInserted(QGraphicsItem*item);

    void on_nodesTable_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

protected:
    void keyPressEvent(QKeyEvent *ev);

private:
    Ui::MainWindow ui;

    NodeEditorScene mapScene;
    NodeEditorWidget mapWidget;

    double scaleKmInPoint;


    struct nodeForSolution {
        int idxInTable;
        int score;
        double xKm;         // coordinates in kilometers
        double yKm;         // coordinates in kilometers
        int xMx10;         // coordinates in decades of meters
        int yMx10;          // coordinates in decades of meters
        double x;           // coordinates on scene
        double y;           // coordinates on scene
    };

    QVector<nodeForSolution> nodesForSolution; // node numbers (scores)
    QVector<int> solution; // indexes of items in nodesForSolution vector


    // OpenCv data for automatic node finder
    cv::Mat mapImage;
    cv::Mat markImage;


};

#endif // MAINWINDOW_H
