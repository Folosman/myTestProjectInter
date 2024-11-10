#ifndef MOVECLASS_H
#define MOVECLASS_H
#include <QVector3D>
#include <QWidget>

namespace Ui {
class MoveClass;
}

class MoveClass : public QWidget
{
    Q_OBJECT

public:
    explicit MoveClass(QWidget *parent = nullptr);
    ~MoveClass();
    void currentPoint(int index);
    int point;
signals:
    void moveSignal(const QVector3D& moveVector, int pointIndex);
private:
    Ui::MoveClass *ui;
    void okBtn();
    void cancelBtn();

public slots:
    void pointData(const QMap<int, QString> point);
};

#endif // MOVECLASS_H
