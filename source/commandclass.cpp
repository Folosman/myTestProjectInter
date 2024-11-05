#include "./lib/commandclass.h"

#include <QCoreApplication>
#include <QVector3D>
#include <QVector>
#include <QDebug>
#include <Eigen/Dense>

CommandClass::CommandClass(QObject *parent)
    : QObject{parent}
{}

void CommandClass::movePoint(QVector3D move)
{

}

QVector3D CommandClass::calculateCenter(const QVector<QVector3D>& points)
{
    QVector3D center(0, 0, 0);
    for (const auto& point : points) {
        center += point;
    }
    center /= points.size();
    return center;
}

// Функция для вычисления нормали к плоскости методом наименьших квадратов
QVector3D CommandClass::calculateNormal(const QVector<QVector3D>& points, const QVector3D& center)
{
    // Создаем матрицу ковариации
    Eigen::Matrix3f covariance = Eigen::Matrix3f::Zero();
    for (const auto& point : points) {
        Eigen::Vector3f p(point.x() - center.x(), point.y() - center.y(), point.z() - center.z());
        covariance += p * p.transpose();
    }

    // Находим собственные значения и собственные вектора
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3f> solver(covariance);
    Eigen::Vector3f normal = solver.eigenvectors().col(0); // Минимальное собственное значение соответствует нормали

    return QVector3D(normal.x(), normal.y(), normal.z());
}
