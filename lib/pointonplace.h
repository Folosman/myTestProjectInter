#ifndef POINTONPLACE_H
#define POINTONPLACE_H

#include <QWidget>

namespace Ui {
class PointOnPlace;
}

class PointOnPlace : public QWidget
{
    Q_OBJECT

public:
    explicit PointOnPlace(QWidget *parent = nullptr);
    ~PointOnPlace();
    void okBtn();
    void cancelBtn();
private:
    Ui::PointOnPlace *ui;

    QMap<int, QString> m_pointName;
    QMap<int, QString> m_placeName;

    int m_pointIndex;
    int m_placeIndex;
signals:
    void pointOnPlace(int pointIndex, int placeIndex);
public slots:
    void pointAndPlace(const QMap<int, QString> pointName,
                       const QMap<int, QString> placeName);

    void currentPointSlot(int index);
    void currentPlaceSlot(int index);
};

#endif // POINTONPLACE_H
