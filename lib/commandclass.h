#ifndef COMMANDCLASS_H
#define COMMANDCLASS_H

#include <QObject>
#include <Eigen/Dense>

class CommandClass : public QObject
{
    Q_OBJECT
public:
    explicit CommandClass(QObject *parent = nullptr);

    void movePoint(QVector3D move);
    QVector3D calculateCenter(const QVector<QVector3D>& points);
    QVector3D calculateNormal(const QVector<QVector3D>& points, const QVector3D& center);

    QVector3D crossProduct(const QVector3D &v1, const QVector3D &v2);
    double dotProduct(const QVector3D &v1, const QVector3D &v2);
    QVector3D findCircumcenter(const QVector3D &A, const QVector3D &B, const QVector3D &C);
    std::tuple<QVector3D, float> calculateCircle3D(const QVector3D& p1,
                                                   const QVector3D& p2,
                                                   const QVector3D& p3);
signals:

private:
};

#endif // COMMANDCLASS_H
