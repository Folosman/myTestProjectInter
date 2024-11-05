#ifndef POINTCREATOR_H
#define POINTCREATOR_H
#include <QVector3D>
#include <QDialog>

namespace Ui {
class PointCreator;
}

class PointCreator : public QDialog
{
    Q_OBJECT

public:
    explicit PointCreator(QWidget *parent = nullptr);
    ~PointCreator();

private:
    Ui::PointCreator *ui;
    void saveCoordinate();
    void cancelBtn();
signals:
    void coordinatSender(QString pointName, QVector3D point, QVector3D);
};


#endif // POINTCREATOR_H
