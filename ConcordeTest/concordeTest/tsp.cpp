#include <QProcess>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDebug>
#include <cassert>
#include "tsp.h"

void tspWriteTspFile(const QVector<int>& matrix, const QString& tspFileName) {
    const int N = sqrt(matrix.size());

    assert(N*N == matrix.size() && "matrix must be square");

    // удаляем ненужные файлы
    QFileInfo tspFileInfo(tspFileName);
    QString path = tspFileInfo.path() + "/";
    QString baseName = tspFileInfo.completeBaseName();
    QString fileNameWithoutExt = path + baseName;

    QFile::remove(fileNameWithoutExt + ".tsp");
    QFile::remove(fileNameWithoutExt + ".mas");
    QFile::remove(fileNameWithoutExt + ".pul");
    QFile::remove(fileNameWithoutExt + ".sav");
    QFile::remove(fileNameWithoutExt + ".sol");
    QFile::remove(path + "O" + baseName + ".mas");
    QFile::remove(path + "O" + baseName + ".pul");
    QFile::remove(path + "O" + baseName + ".sav");

    // записываем новый tsp Файл
    QFile dest(tspFileName);
    if (!dest.open(QIODevice::WriteOnly)) {
        qDebug() << "cannot create tsp file";
        std::runtime_error("cannot create tsp file");
    }

    // форматирование матрицы для файла
    int maxElement = *std::max_element(matrix.cbegin(), matrix.cend());
    int maxDigits = QString::number(maxElement).size();
    QString matrixString;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j <= i; ++j) {
            matrixString += QString("  %1").arg(matrix[i*N+j], maxDigits);
        }
        matrixString += "\n";
    };

    QString templateString =
R"(NAME: %1
TYPE: TSP
COMMENT: %2
DIMENSION: %3
EDGE_WEIGHT_TYPE: EXPLICIT
EDGE_WEIGHT_FORMAT: LOWER_DIAG_ROW
EDGE_WEIGHT_SECTION
%4EOF)"; // новый вид сырых строковых литералов см. c++11 raw literals

    templateString = templateString.arg("My Route").arg("My Route").arg(N).arg(matrixString);
    if (dest.write(templateString.toLocal8Bit()) < 0) {
        qDebug() << "cannot write tsp file";
        std::runtime_error("cannot write tsp file");
    }
    dest.close();
}

QVector<int> tspRoute(const QString& tspFileName) {
    QString command = QString("concorde.exe ") + tspFileName;
    int exitCode = QProcess::execute(command);

    if (exitCode == -2) {
        qDebug() << "concorde cannot be started";
        throw std::runtime_error("concorde cannot be started");
    } else if(exitCode == -1) {
        qDebug() << "concorde crashed";
        throw std::runtime_error("concorde crashed");
    }

    QFileInfo tspFileInfo(tspFileName);
    QString solFileName = tspFileInfo.path() + "/" + tspFileInfo.completeBaseName() + ".sol";

    QFile file(solFileName);
    bool openResult = file.open(QIODevice::ReadOnly);
    if (!openResult) {
        QString errorString = QString("solution file ") + solFileName + QString(" cannot be opened");
        qDebug() << errorString;
        throw std::runtime_error(errorString.toStdString());
    }

    QTextStream textStream(&file);
    int numPoints;
    textStream >> numPoints;
    QVector<int> route(numPoints);

    for (int i = 0; i < numPoints; ++i) {
        textStream >> route[i];
    }
    return route;
}

