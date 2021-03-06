#include "OpencvQtTools.h"
#include <QDebug>


//--------------------------------------------------------------------------------------
// Convert OpenCV Mat frame to QT image format
// http://asmaloney.com/2013/11/code/converting-between-cvmat-and-qimage-or-qpixmap/
//--------------------------------------------------------------------------------------
QImage cvMatToQImage(const cv::Mat& mat) {
    const int width = mat.cols;
    const int height = mat.rows;
    const int bytesPerLine = mat.step;
    switch (mat.type()) {
    // 8-bit, 4 channel
    case CV_8UC4:
    {
        QImage image(mat.data, width, height, bytesPerLine, QImage::Format_RGB32);
        return image;
    }

    // 8-bit, 3 channel
    case CV_8UC3:
    {
        QImage image(mat.data, width, height, bytesPerLine, QImage::Format_RGB888);
        return image.rgbSwapped();
    }

    // 8-bit, 1 channel
    case CV_8UC1:
    {
        static QVector<QRgb>  sColorTable;

        // only create our color table once
        if (sColorTable.isEmpty()) {
            for (int i = 0; i < 256; ++i)
                sColorTable.push_back(qRgb(i, i, i));
        }
        QImage image(mat.data, width, height, bytesPerLine, QImage::Format_Indexed8);
        image.setColorTable(sColorTable);
        return image;
    }

    default:
        qWarning() << "ASM::cvMatToQImage() - cv::Mat image type not handled in switch:" << mat.type();
        break;
    }

    return QImage();
}

//--------------------------------------------------------------------------------------
// Convert OpenCV Mat frame to QT pixmap format
// http://asmaloney.com/2013/11/code/converting-between-cvmat-and-qimage-or-qpixmap/
//--------------------------------------------------------------------------------------
QPixmap cvMatToQPixmap(const cv::Mat& mat) {
   return QPixmap::fromImage(cvMatToQImage(mat));
}


