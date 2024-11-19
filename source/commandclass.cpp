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

// Функция для вычисления нормали к плоскости методом наименьших квадрато
