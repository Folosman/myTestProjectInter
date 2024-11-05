#ifndef COMMANDCLASS_H
#define COMMANDCLASS_H

#include <QObject>

class CommandClass : public QObject
{
    Q_OBJECT
public:
    explicit CommandClass(QObject *parent = nullptr);

    void movePoint(QVector3D move);
    QVector3D calculateCenter(const QVector<QVector3D>& points);
    QVector3D calculateNormal(const QVector<QVector3D>& points, const QVector3D& center);

signals:

private:
};

#endif // COMMANDCLASS_H
