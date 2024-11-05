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
signals:
    void moveSignal(const QVector3D& moveVector);
private:
    Ui::MoveClass *ui;
    void okBtn();
};

#endif // MOVECLASS_H
