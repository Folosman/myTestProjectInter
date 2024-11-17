#ifndef PLACEFORM_H
#define PLACEFORM_H

#include <QWidget>

namespace Ui {
class PlaceForm;
}

class PlaceForm : public QWidget
{
    Q_OBJECT

public:
    explicit PlaceForm(QWidget *parent = nullptr);
    ~PlaceForm();
    int firstPoint;
    int secondPoint;
    int threedPoint;

    void currentFirstPoint(int index);
    void currentSecondePoint(int index);
    void currentThreedPoint(int index);

    void okBtn();

private:
    Ui::PlaceForm *ui;
    QVector<int> m_indexPoint;
public slots:
    void pointData(const QMap<int, QString> point);
signals:
    void sendPoint(QVector<int> pointIndex);
};

#endif // PLACEFORM_H
