#include "./lib/mainwindow.h"
#include "ui_mainwindow.h"
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QListWidget>
#include <QRandomGenerator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->m_commandList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->m_commandList, &QListWidget::customContextMenuRequested,
            this, &MainWindow::showContextMenu);

    QObject::connect(&m_comment,    &Comment::commentText,          this, &MainWindow::commentSlot);
    QObject::connect(&m_point,      &PointCreator::coordinatSender, this, &MainWindow::createPointSlot);
    QObject::connect(&m_move,       &MoveClass::moveSignal,         this, &MainWindow::moveSlot);
    QObject::connect(&m_circle,     &CircleByPoints::circleParams,  this, &MainWindow::circleParams);
    QObject::connect(&m_pointOnPlace,&PointOnPlace::pointOnPlace,   this, &MainWindow::pointAndPlace);
    QObject::connect(&m_placeForm,  &PlaceForm::sendPoint,          this, &MainWindow::placePoints);

    QObject::connect(this, &MainWindow::pointOnPlace, &m_pointOnPlace, &PointOnPlace::pointAndPlace);
    QObject::connect(this, &MainWindow::sendPointInPlace, &m_placeForm, &PlaceForm::sendPoint);

    connect(ui->m_commentBtn,   &QPushButton::clicked, this, &MainWindow::commitBtn);
    connect(ui->m_moveBtn,      &QPushButton::clicked, this, &MainWindow::moveBtn);
    connect(ui->m_pointBtn,     &QPushButton::clicked, this, &MainWindow::createPointBtn);
    connect(ui->m_circleBtn,    &QPushButton::clicked, this, &MainWindow::makeCircleBtn);
    connect(ui->m_bestFitBtn,   &QPushButton::clicked, this, &MainWindow::makePlace);
    connect(ui->m_pointCircleBtn,&QPushButton::clicked, this, &MainWindow::pointFromCircleBtn);
    connect(ui->m_pointPlaceBtn, &QPushButton::clicked, this, &MainWindow::pointOnPlaceBtn);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::commitBtn()
{
    m_comment.show();
}

void MainWindow::createPointBtn()
{
    m_point.show();
}

void MainWindow::moveBtn()
{
    m_move.show();
}

void MainWindow::makeCircleBtn()
{
    m_circle.show();
}

void MainWindow::makePlace()
{
    emit sendPointInPlace(m_commandPointName);
    m_placeForm.show();
}

//*     Контекстное меню        *//
/// \brief MainWindow::showContextMenu
/// \param pos
/// При нажатии пкм по объекту QListView срабатывает сигнал и вызывается окно
///
/// <TODO> НЕОБХОДИМО СДЕЛАТЬ ВЫЗОВ РАЗНЫХ ФУНКЦИЙ ДЛЯ РАЗНЫХ ЭЛЕМЕНТОВ

void MainWindow::showContextMenu(const QPoint &pos) {
    QMenu contextMenu(this);

    // Создаем действия
    QAction *deleteAction = new QAction("Удалить", this);
    QAction *editAction = new QAction("Изменить", this);

    // Добавляем действия в меню
    contextMenu.addAction(deleteAction);
    contextMenu.addAction(editAction);

    // Получаем элемент, на котором вызвано контекстное меню
    QListWidgetItem *item = ui->m_commandList->itemAt(pos);
    if (!item) return;  // Если нет элемента под курсором, не показываем меню

    // Подключаем действие "Изменить" к слоту editItem
    connect(editAction, &QAction::triggered,
            [this, item]() {editItem(item);});

    connect(deleteAction, &QAction::triggered,
            [this, item](){deleteItem(item);});
    // Показываем контекстное меню
    contextMenu.exec(ui->m_commandList->mapToGlobal(pos));
}

void MainWindow::editItem(QListWidgetItem *item) {
    bool ok;
    QString newText = QInputDialog::getText(this, "Редактировать", "Новое значение:",
                                            QLineEdit::Normal, item->text(), &ok);

    if (ok && !newText.isEmpty())
        item->setText(newText);
}

void MainWindow::deleteItem(QListWidgetItem *item)
{
    delete item;
}




        /*      SLOTS       */
void MainWindow::commentSlot(const QString &text)
{
    QString command = QString("COMMENT: %1").arg(text);
    ui->m_commandList->addItem(command);
}

void MainWindow::pointAndPlace(int pointIndex, int placeIndex)
{
    QString name = QString("POINT%1").arg(m_pointCounter);

    QVector3D vecToPoint = m_comandActualPoints[pointIndex] - m_commandPlace[placeIndex];

    double distance = QVector3D::dotProduct(vecToPoint, m_commandNormales[placeIndex]);

    QVector3D projection = m_comandActualPoints[pointIndex] - distance *  m_commandNormales[placeIndex];


    // Создаем уникальный ID для команды
    int commandId = ++m_commandCounter;

    // Сохраняем точку и нормаль в QMap
    m_commandPoints[commandId] = projection;
    m_comandActualPoints[commandId] = projection;
    m_commandNormales[commandId] = m_commandNormales[placeIndex];
    m_commandPointName[commandId] = name;


    QString actual = QString("(%1, %2, %3) [%4, %5, %6]")
                          .arg(projection.x())
                          .arg(projection.y())
                          .arg(projection.z())
                          .arg(m_commandNormales[placeIndex].x())
                          .arg(m_commandNormales[placeIndex].y())
                          .arg(m_commandNormales[placeIndex].z());


    QString command = QString("%1:\nACTUAL: %2")
                          .arg(name)
                          .arg(actual);

    QListWidgetItem* item = new QListWidgetItem(command);
    item->setData(Qt::UserRole, commandId); // Сохраняем ID команды в элементе списка
    ui->m_commandList->addItem(item);
}
        /*      END     */

        /*      Создание точки      */
void MainWindow::createPointSlot(const QString& name, const QVector3D& point, const QVector3D& normale)
{
    QString tempName = name;
    if(name.isEmpty())
    {
        tempName = QString("POINT%1").arg(m_pointCounter);
        m_pointCounter++;
    }
    // Обновляем текущие координаты

    // Создаем уникальный ID для команды
    int commandId = ++m_commandCounter;

    // Сохраняем точку и нормаль в QMap
    m_commandPoints[commandId] = point;
    m_comandActualPoints[commandId] = point;
    m_commandNormales[commandId] = normale;
    m_commandPointName[commandId] = tempName;
    m_pointChecker[commandId] = true;

    // Форматируем координаты и нормаль для отображения
    QString nominal = QString("(%1, %2, %3) [%4, %5, %6]")
                          .arg(point.x())
                          .arg(point.y())
                          .arg(point.z())
                          .arg(normale.x())
                          .arg(normale.y())
                          .arg(normale.z());

    QString actual = QString("(%1, %2, %3)")
                         .arg(m_comandActualPoints[commandId].x())
                         .arg(m_comandActualPoints[commandId].y())
                         .arg(m_comandActualPoints[commandId].z());

    QString command = QString("%1:\nNOMINAL: %2\nACTUAL: %3")
                          .arg(tempName)
                          .arg(nominal)
                          .arg(actual);

    // Добавляем команду в QListWidget
    QListWidgetItem* item = new QListWidgetItem(command);
    item->setData(Qt::UserRole, commandId); // Сохраняем ID команды в элементе списка
    ui->m_commandList->addItem(item);
}

            /*      Перемещение точки      */
//* Добавить текущий элемент и его перемещение      *//
void MainWindow::moveSlot(const QVector3D& moveVector) {

    QListWidgetItem* selectedItem = ui->m_commandList->currentItem();
    if (!selectedItem)
    {
        qWarning() << "Не выбран элемент для обновления!";
        return;
    }

     int commandId = selectedItem->data(Qt::UserRole).toInt();

    if (!m_commandPoints.contains(commandId))
     {
        qWarning() << "Элемент с ID" << commandId << "не найден!";
        return;
    }

    // m_comandActualPoints[commandId] += moveVector;
    QVector3D newCoord = m_comandActualPoints[commandId] + moveVector;

    // Форматируем координаты в строку
    QString actual = QString("(%1, %2, %3)")
                         .arg(newCoord.x())
                         .arg(newCoord.y())
                         .arg(newCoord.z());

    // Создаем ID для команды
    // int commandId = ++m_commandCounter;
    // m_commandCoords[commandId] = newCoord;

    // Формируем текст команды
    QString commandText = QString("MOVE %1:\nACTUAL: %2")
                              .arg(m_commandPointName[commandId])
                              .arg(actual);

    // Добавляем команду в QListWidget
    QListWidgetItem* item = new QListWidgetItem(commandText);
    item->setData(Qt::UserRole, commandId); // Сохраняем ID команды в элементе списка
    ui->m_commandList->addItem(item);


}


        /*      Создание Круга      */
void MainWindow::circleParams(QString name, QVector3D coord, QVector3D normale, float radius)
{

    QString tempName = name;

    if(name.isEmpty())
    {
        tempName = QString("CIRCLE%1").arg(m_circleCounter);
        m_circleCounter++;
    }

    int commandId = ++m_commandCounter;
    m_commandPoints[commandId]      = coord;
    m_commandCircleCenter[commandId] = coord;
    m_commandCircleRadius[commandId] = radius;
    m_commandCircleName[commandId]   = tempName;
    m_circleChecker[commandId]       = true;

    QString coordString = QString("(%1, %2, %3) [%4, %5, %6]")
                              .arg(coord[0])
                              .arg(coord[1])
                              .arg(coord[2])
                              .arg(normale[0])
                              .arg(normale[1])
                              .arg(normale[2]);

    QString command = QString("%1:\nPOSITION: %2\nRADIUS: %3")
                          .arg(tempName)
                          .arg(coordString)
                          .arg(radius);

    QListWidgetItem* item = new QListWidgetItem(command);
    item->setData(Qt::UserRole, commandId); // Сохраняем ID команды в элементе списка
    ui->m_commandList->addItem(item);
}


            /*      Создание плоскости      */
void MainWindow::placePoints(QVector<int> index)
{
    QString name = QString("PLACE%1").arg(m_placeCounter);
    m_placeCounter++;

    QVector3D middle{0,0,0};

    for(int i = 0; i < m_comandActualPoints.size(); i++)
    {
        middle += m_comandActualPoints[i];
    }

    QVector3D normale;
    for(int i = 0; i < m_commandNormales.size(); i++)
    {
        normale += m_commandNormales[i];
    }
    normale.normalize();
    middle /= m_commandPoints.size();

    int commandId = ++m_commandCounter;
    m_commandPoints[commandId]  = middle;
    m_commandPlace[commandId]   = middle;
    m_commandPlaceName[commandId] = name;
    m_placeChecker[commandId]   = true;


    QString coordString = QString("(%1, %2, %3) [%4, %5, %6]")
    .arg(middle[0])
        .arg(middle[1])
        .arg(middle[2])
        .arg(normale[0])
        .arg(normale[1])
        .arg(normale[2]);

    QString command = QString("%1:\nPOSITION: %2\n")
                          .arg(name)
                          .arg(coordString);

    QListWidgetItem* item = new QListWidgetItem(command);
    item->setData(Qt::UserRole, commandId); // Сохраняем ID команды в элементе списка
    ui->m_commandList->addItem(item);

    // QObject::connect(ui->m_commandList, &QListWidget::itemChanged, [&](QListWidgetItem *changedItem)
    // {
    //     int index = changedItem->data(Qt::UserRole).toInt();
    //     int currentIndex = item->data(Qt::UserRole).toInt();

    //     if (/*(index < currentIndex) && */(m_commandCircleCenter[index].isNull()) && (m_commandPlace[index].isNull())) {
    //         QVector3D middle{0,0,0};
    //         QVector3D normale;

    //         for(int i = 0; i < index; i++)
    //         {
    //             if((m_commandCircleCenter[index].isNull()) && (m_commandPlace[index].isNull()))
    //             {
    //                 middle += m_comandActualPoints[i];
    //                 normale += m_commandNormales[i];
    //             }
    //         }

    //         m_commandPoints[currentIndex]  = middle;
    //         m_commandPlace[currentIndex]   = middle;

    //         QString coordString = QString("(%1, %2, %3) [%4, %5, %6]")
    //                                   .arg(middle[0])
    //                                   .arg(middle[1])
    //                                   .arg(middle[2])
    //                                   .arg(normale[0])
    //                                   .arg(normale[1])
    //                                   .arg(normale[2]);

    //         QString command = QString("%1:\nPOSITION: %2\n")
    //                               .arg(m_commandPlaceName[currentIndex])
    //                               .arg(coordString);
    //         item->setText(command);
    //     }
    // });


}

void MainWindow::pointFromCircleBtn()
{
    QListWidgetItem* selectedItem = ui->m_commandList->currentItem();

    if (!selectedItem)
    {
        qWarning() << "Не выбран элемент для обновления!";
        return;
    }

    int commandId = selectedItem->data(Qt::UserRole).toInt();

    if (!m_commandPoints.contains(commandId))
    {
        qWarning() << "Элемент с ID" << commandId << "не найден!";
        return;
    }

    QString name = QString("POINT%1").arg(m_pointCounter);
    m_pointCounter++;

    QVector3D coord = m_commandPoints[commandId];
    QVector3D normale = m_commandNormales[commandId];

    int newCommandId = ++m_commandCounter;
    m_commandPoints[newCommandId] = coord;
    m_comandActualPoints[newCommandId] = coord;
    m_commandNormales[newCommandId] = normale;
    m_pointChecker[newCommandId] = true;

    QString nominal = QString("(%1, %2, %3) [%4, %5, %6]")
                              .arg(coord[0])
                              .arg(coord[1])
                              .arg(coord[2])
                              .arg(normale[0])
                              .arg(normale[1])
                              .arg(normale[2]);

    QString actual = QString("(%1, %2, %3)")
                         .arg(m_comandActualPoints[commandId].x())
                         .arg(m_comandActualPoints[commandId].y())
                         .arg(m_comandActualPoints[commandId].z());

    QString command = QString("%1:\nNOMINAL: %2\nACTUAL: %3")
                          .arg(name)
                          .arg(nominal)
                          .arg(actual);

    QListWidgetItem* item = new QListWidgetItem(command);
    item->setData(Qt::UserRole, newCommandId); // Сохраняем ID команды в элементе списка
    ui->m_commandList->addItem(item);
}


void MainWindow::pointOnPlaceBtn()
{
    emit pointOnPlace(m_commandPointName, m_commandPlaceName);
    m_pointOnPlace.show();
}


void MainWindow::errorPointBtn()
{
    QListWidgetItem* selectedItem = ui->m_commandList->currentItem();

    if (!selectedItem)
    {
        qWarning() << "Не выбран элемент для обновления!";
        return;
    }

    int commandId = selectedItem->data(Qt::UserRole).toInt();

    if (!m_commandPoints.contains(commandId))
    {
        qWarning() << "Элемент с ID" << commandId << "не найден!";
        return;
    }


    /*      ДОБАВЬ УСЛОВИЯ ДЛЯ КРУГА И ХУЙНИ        */

    QString name = QString("LOCATION%1").arg(m_locationCounter);

    QVector3D error = m_commandPoints[commandId] - m_comandActualPoints[commandId];


}


void MainWindow::startBtn()
{

}

void MainWindow::clearBtn()
{
    m_commandCoords.clear();
    m_commandPoints.clear();
    m_comandActualPoints.clear();
    m_commandNormales.clear();
    m_commandCircleCenter.clear();
    m_commandPlace.clear();
    m_commandCircleRadius.clear();
    m_commandPointName.clear();
    m_commandPlaceName.clear();
    m_commandCircleName.clear();

    m_commandCounter    = 1;
    m_pointCounter      = 1;
    m_placeCounter      = 1;
    m_circleCounter     = 1;
}
