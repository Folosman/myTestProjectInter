#include "./lib/circlebypoints.h"
#include "ui_circlebypoints.h"
#include "Eigen/Dense"
CircleByPoints::CircleByPoints(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CircleByPoints)
{
    ui->setupUi(this);
    connect(ui->m_okBtn, &QPushButton::clicked, this, &CircleByPoints::circleByPoint);
}

CircleByPoints::~CircleByPoints()
{
    delete ui;
}

struct CircleParams {
    QVector3D center;
    QVector3D normal;
    double radius;
};

// Функция для вычисления параметров окружности
CircleParams calculateCircleParams(const QVector<QVector3D>& points) {
    int n = points.size();
    if (n < 3) {
        throw std::runtime_error("Нужно как минимум 3 точки для определения окружности.");
    }

    // 1. Вычисление центра масс точек
    QVector3D centroid(0, 0, 0);
    for (const auto& point : points) {
        centroid += point;
    }
    centroid /= n;

    // 2. Составляем матрицу для вычисления нормали
    Eigen::MatrixXd A(n, 3);
    for (int i = 0; i < n; ++i) {
        QVector3D centeredPoint = points[i] - centroid;
        A(i, 0) = centeredPoint.x();
        A(i, 1) = centeredPoint.y();
        A(i, 2) = centeredPoint.z();
    }

    // 3. Вычисляем собственные векторы и собственные значения матрицы
    Eigen::JacobiSVD<Eigen::MatrixXd> svd(A, Eigen::ComputeThinU | Eigen::ComputeThinV);
    Eigen::Vector3d normalEigen = svd.matrixV().col(2);

    QVector3D normal(normalEigen(0), normalEigen(1), normalEigen(2));
    normal.normalize();

    // 4. Проецируем точки на плоскость окружности
    QVector<QVector3D> projectedPoints;
    for (const auto& point : points) {
        QVector3D vectorToPoint = point - centroid;
        QVector3D projection = vectorToPoint - QVector3D::dotProduct(vectorToPoint, normal) * normal;
        projectedPoints.append(projection + centroid);
    }

    // 5. Вычисляем центр окружности и радиус
    double xSum = 0, ySum = 0, xxSum = 0, yySum = 0, xySum = 0, zSum = 0;
    for (const auto& point : projectedPoints) {
        double x = point.x();
        double y = point.y();
        xSum += x;
        ySum += y;
        xxSum += x * x;
        yySum += y * y;
        xySum += x * y;
        zSum += x * x + y * y;
    }

    double C = n * xxSum - xSum * xSum;
    double D = n * xySum - xSum * ySum;
    double E = n * yySum - ySum * ySum;
    double G = n * zSum - (xSum * xSum + ySum * ySum);

    double a = (D * G - E * xSum) / (2 * (C * E - D * D));
    double b = (C * G - D * ySum) / (2 * (C * E - D * D));

    QVector3D circleCenter(centroid.x() + a, centroid.y() + b, centroid.z());
    double radius = std::sqrt(a * a + b * b + zSum / n);

    return {circleCenter, normal, radius};
}



void CircleByPoints::circleByPoint()
{
    QVector3D center;
    center[0] = ui->m_xCoordinate->text().toFloat();
    center[1] = ui->m_yCoordinate->text().toFloat();
    center[2] = ui->m_zCoordinate->text().toFloat();

    QVector3D normale;
    if (ui->m_xNormale->isChecked())
        normale = QVector3D(1, 0, 0);
    else if (ui->m_yNormale->isChecked())
        normale = QVector3D(0, 1, 0);
    else if (ui->m_zNormale->isChecked())
        normale = QVector3D(0, 0, 1);

    emit circleParams(ui->m_nameEdit->text(), center, normale, ui->m_circleRadius->text().toFloat());
}
