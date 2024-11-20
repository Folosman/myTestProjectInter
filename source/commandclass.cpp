#include "./lib/commandclass.h"

#include <QCoreApplication>
#include <QVector3D>
#include <QVector>
#include <QDebug>

CommandClass::CommandClass(QObject *parent)
    : QObject{parent}
{}

QVector3D CommandClass::calculateCenter(const QVector<QVector3D>& points)
{
    QVector3D center(0, 0, 0);
    for (const auto& point : points) {
        center += point;
    }
    center /= points.size();
    return center;
}

QVector3D CommandClass::crossProduct(const QVector3D &v1, const QVector3D &v2) {
    return QVector3D::crossProduct(v1, v2);
}

// Скалярное произведение двух векторов
double CommandClass::dotProduct(const QVector3D &v1, const QVector3D &v2) {
    return QVector3D::dotProduct(v1, v2);
}

// Нахождение равноудалённой точки
QVector3D CommandClass::findCircumcenter(const QVector3D &A, const QVector3D &B, const QVector3D &C) {
    // Векторы AB и AC
    QVector3D AB = B - A;
    QVector3D AC = C - A;

    // Нормаль к плоскости треугольника
    QVector3D normal = crossProduct(AB, AC);

    // Скалярное произведение нормали на себя
    double normalDot = dotProduct(normal, normal);


    // Временные переменные для расчёта центра окружности
    QVector3D temp1 = crossProduct(
        (dotProduct(AB, AB) * AC - dotProduct(AC, AC) * AB),
        normal
        );

    double scale = 0.5 / normalDot;

    // Координаты равноудалённой точки
    return A + temp1 * scale;
}

std::tuple<QVector3D, float> CommandClass::calculateCircle3D(const QVector3D& p1, const QVector3D& p2, const QVector3D& p3) {
    // Векторы между точками
    QVector3D v1 = p2 - p1;
    QVector3D v2 = p3 - p1;

    // Векторное произведение для получения нормали плоскости
    QVector3D normal = QVector3D::crossProduct(v1, v2);
    if (normal.lengthSquared() < 1e-6) {
        // Точки коллинеарны или слишком близки
        return {QVector3D(0,0,0), 0.00};
    }

    // Середины сторон
    QVector3D mid1 = (p1 + p2) / 2.0;
    QVector3D mid2 = (p1 + p3) / 2.0;

    // Направления векторов
    QVector3D dir1 = QVector3D::crossProduct(normal, v1).normalized();
    QVector3D dir2 = QVector3D::crossProduct(normal, v2).normalized();

    // Решаем систему линейных уравнений для нахождения центра окружности
    QVector3D diff = mid2 - mid1;
    double t1 = QVector3D::dotProduct(QVector3D::crossProduct(diff, dir2), normal) /
                QVector3D::dotProduct(QVector3D::crossProduct(dir1, dir2), normal);

    // Центр окружности
    QVector3D center = mid1 + dir1 * t1;

    // Радиус окружности
    double radius = (center - p1).length();
    qDebug() << center << radius;

    return {center, radius};
}
