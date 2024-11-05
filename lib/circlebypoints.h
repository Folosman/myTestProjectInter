#ifndef CIRCLEBYPOINTS_H
#define CIRCLEBYPOINTS_H

#include <QWidget>
#include <QVector3D>

namespace Ui {
class CircleByPoints;
}

class CircleByPoints : public QWidget
{
    Q_OBJECT

public:
    explicit CircleByPoints(QWidget *parent = nullptr);
    ~CircleByPoints();
    void circleByPoint();
private:
    Ui::CircleByPoints *ui;
signals:
    void circleParams(QString name, QVector3D coord, QVector3D normale, float radius);
};

#endif // CIRCLEBYPOINTS_H
