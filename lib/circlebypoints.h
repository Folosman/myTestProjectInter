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
    // void circleByPoint();
    void currentFirstPoint(int);
    void currentSecondPoint(int);
    void currentThreedPoint(int);

    void okBtn();
    void cancelBtn();

private:
    Ui::CircleByPoints *ui;
    int firstPoint;
    int secondePoint;
    int threedPoint;
    QVector<int> m_indexPoint;
signals:
    void sendPoint(QVector<int> index);
public slots:
    void circleByPoint(QMap<int, QString> point);
};

#endif // CIRCLEBYPOINTS_H
