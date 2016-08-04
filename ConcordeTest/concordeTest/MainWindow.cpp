#include "MainWindow.h"
#include <QDebug>
#include <QFile>
#include <QDir>
#include "tsp.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    ui.setupUi(this);
}

void MainWindow::on_pushButton_clicked()
{
    QVector<int> route = tspRoute("myroute_concorde.tsp");

    QString text;
    std::for_each(route.cbegin(), route.cend(), [&](int v) {
        text += QString::number(v) + "->";
    });
    ui.label->setText(text);
}

void MainWindow::on_pushButton_2_clicked()
{
    QVector<QPoint> xy; // вектор точек с координатами (x,y)

    QFile file("c:/projects/RogaineHelper/ConcordeTest/debug/debug/points2.txt");
    bool openResult = file.open(QIODevice::ReadOnly);
    if (!openResult)
        qDebug() << "points file cannot be opened";

    QTextStream textStream(&file);
    while (true) {
        int x, y;
        textStream >> x;
        textStream >> y;
        xy.append(QPoint(x, y));
        if (textStream.atEnd())
            break;
    }


    // Creates distance matrix for the given coordinate vectors
    const int N = xy.size();
    QVector<int> matrix(N*N); // матрица расстояний
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            matrix[i*N + j] = sqrt((xy[i].x() - xy[j].x())*(xy[i].x() - xy[j].x()) + (xy[i].y() - xy[j].y())*(xy[i].y() - xy[j].y()));
        }
    }

    // Calculate route
    //  convert an asymterical tsp to symetrical не нужно, т.к. матрица уже симметричная
    //  масштабируем элементы, чтобы они были меньше 32768
    // keep it < 32768 to stay within 16bit int
    double scale = 32768.0 / *std::max_element(matrix.cbegin(), matrix.cend());
    scale = (std::min)(scale, 1.0); // std::min в скобках, чтобы не дать отработать макросу min
    std::for_each(matrix.begin(), matrix.end(), [&](int& element) {
        element *= scale;
    });

    tspWriteTspFile(matrix, "myroute_concorde.tsp");
 }
