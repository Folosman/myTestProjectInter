#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "./lib/comment.h"
#include "./lib/pointcreator.h"
#include "./lib/moveclass.h"
#include "./lib/circlebypoints.h"
#include "./lib/pointonplace.h"
#include "./lib/placeform.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QContextMenuEvent>
#include <QMenu>
#include <QMainWindow>
#include <QAction>
#include <QMessageBox>
#include <QInputDialog>
#include <QVBoxLayout>
#include <QListWidget>
#include <QVector3D>
#include <QVector>

#include <QMouseEvent>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    Comment         m_comment;
    PointCreator    m_point;
    MoveClass       m_move;
    CircleByPoints  m_circle;
    PointOnPlace    m_pointOnPlace;
    PlaceForm       m_placeForm;

    QMenu           m_contextMenu;

    QVector<QVector3D> m_moveVector;
    QVector<QVector3D> m_normaleVector;
    QVector3D          m_actualCoord {0, 0, 0};



            /*      Вы спросите, почему не структуры Никита?
                    А я вам отвечу, потому что я отсталый       */

    QMap<int, QVector3D> m_commandCoords;
    QMap<int, QVector3D> m_commandPoints;
    QMap<int, QVector3D> m_comandActualPoints;
    QMap<int, QVector3D> m_commandNormales;
    QMap<int, QVector3D> m_commandCircleCenter;
    QMap<int, QVector3D> m_commandPlace;
    QMap<int, float>     m_commandCircleRadius;
    QMap<int, float>     m_commandCircleActualRadius;
    QMap<int, QString>   m_commandPointName;
    QMap<int, QString>   m_commandPlaceName;
    QMap<int, QString>   m_commandCircleName;
    QMap<int, QVector3D> m_commandMoveVector;
    QMap<int, QVector3D> m_commandActualNormales;

    QMap<int, float>     m_pointErrors;
    QMap<int, float>    m_radiusErrors;
    QMap<int, QString>   m_errorName;

    QMap<int, bool>     m_pointChecker;
    QMap<int, bool>     m_placeChecker;
    QMap<int, bool>     m_circleChecker;
    QMap<int, bool>     m_moveChecker;
    QMap<int, bool>     m_pointByCircleChecker;
    QMap<int, bool>     m_pointErrorChecker;
    QMap<int, bool>     m_circleErrorCheker;
    QMap<int, bool>     m_pointByPlace;

    QMap<int, QVector<int>> m_dependence;
    QMap<int, QListWidgetItem*> m_listItem;

    int m_commandCounter    = 1;
    int m_pointCounter      = 1;
    int m_placeCounter      = 1;
    int m_circleCounter     = 1;
    int m_locationCounter   = 1;

    QListWidgetItem *m_editItem;
    int m_editIndex = -1;
    bool m_pointEdit = false;
    bool m_placeEdit = false;
    bool m_circleEdit = false;

    float m_error = 0.15f;
public:
    void commitBtn();
    void moveBtn();
    void createPointBtn();
    void makePlace();
    void makeCircleBtn();
    void pointFromCircleBtn();
    void pointOnPlaceBtn();
    void errorPointBtn();
    void errorCircleBtn();

    void startBtn();
    void clearBtn();

    void showContextMenu(const QPoint &pos);
    void editItem(QListWidgetItem *item);
    void deleteItem(QListWidgetItem *item);
    void updateList();

    void save();
    void load();
    void start();

public slots:
    void commentSlot(const QString &text);
    void createPointSlot(const QString& name, const QVector3D& point, const QVector3D& normale);
    void moveSlot(const QVector3D& moveVec);
    void circleParams(QVector<int> index);
    void pointAndPlace(int pointIndex, int placeIndex);
    void placePoints(QVector<int> index);

signals:
    void pointOnPlace(QMap<int, QString> point, QMap<int, QString> place);
    void sendPointInPlace(QMap<int, QString> point);
    void sendPoint(QMap<int, QString> point);

};
#endif // MAINWINDOW_H
