#ifndef POINTBYCIRCLE_H
#define POINTBYCIRCLE_H

#include <QWidget>

namespace Ui {
class PointByCircle;
}

class PointByCircle : public QWidget
{
    Q_OBJECT

public:
    explicit PointByCircle(QWidget *parent = nullptr);
    ~PointByCircle();

private:
    Ui::PointByCircle *ui;
};

#endif // POINTBYCIRCLE_H
