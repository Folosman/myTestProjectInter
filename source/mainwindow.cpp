#include "./lib/mainwindow.h"
#include "ui_mainwindow.h"
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QListWidget>
#include <QRandomGenerator>
#include <QtMath>
#include <QDir>
#include <QFile>

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
    QObject::connect(&m_circle,     &CircleByPoints::sendPoint,     this, &MainWindow::circleParams);
    QObject::connect(&m_pointOnPlace,&PointOnPlace::pointOnPlace,   this, &MainWindow::pointAndPlace);
    QObject::connect(&m_placeForm,  &PlaceForm::sendPoint,          this, &MainWindow::placePoints);

    QObject::connect(this, &MainWindow::pointOnPlace, &m_pointOnPlace, &PointOnPlace::pointAndPlace);
    QObject::connect(this, &MainWindow::sendPointInPlace, &m_placeForm, &PlaceForm::pointData);
    QObject::connect(this, &MainWindow::sendPoint, &m_move, &MoveClass::pointData);
    QObject::connect(this, &MainWindow::sendPoint, &m_circle, &CircleByPoints::circleByPoint);

    connect(ui->m_commentBtn,       &QPushButton::clicked,  this, &MainWindow::commitBtn);
    connect(ui->m_moveBtn,          &QPushButton::clicked,  this, &MainWindow::moveBtn);
    connect(ui->m_pointBtn,         &QPushButton::clicked,  this, &MainWindow::createPointBtn);
    connect(ui->m_circleBtn,        &QPushButton::clicked,  this, &MainWindow::makeCircleBtn);
    connect(ui->m_bestFitBtn,       &QPushButton::clicked,  this, &MainWindow::makePlace);
    connect(ui->m_pointCircleBtn,   &QPushButton::clicked,  this, &MainWindow::pointFromCircleBtn);
    connect(ui->m_pointPlaceBtn,    &QPushButton::clicked,  this, &MainWindow::pointOnPlaceBtn);
    connect(ui->m_errorPointBtn,    &QPushButton::clicked,  this, &MainWindow::errorPointBtn);
    connect(ui->m_errorCircleBtn,   &QPushButton::clicked,  this, &MainWindow::errorCircleBtn);
    connect(ui->actionSave_File,    &QAction::triggered,    this, &MainWindow::save);

    connect(ui->m_commandList, &QListWidget::itemChanged, this, &MainWindow::updateList);
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
    emit sendPoint(m_commandPointName);
    m_move.show();
}

void MainWindow::makeCircleBtn()
{
    emit sendPoint(m_commandPointName);
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

    QAction *deleteAction = new QAction("Удалить", this);
    QAction *editAction = new QAction("Изменить", this);

    contextMenu.addAction(deleteAction);
    contextMenu.addAction(editAction);

    // Получаем элемент, на котором вызвано контекстное меню
    QListWidgetItem *item = ui->m_commandList->itemAt(pos);
    if (!item) return;  // Если нет элемента под курсором, не показываем меню


    connect(editAction, &QAction::triggered,
            [this, item]() {editItem(item);});

    connect(deleteAction, &QAction::triggered,
            [this, item](){deleteItem(item);});

    contextMenu.exec(ui->m_commandList->mapToGlobal(pos));
}

void MainWindow::editItem(QListWidgetItem *item)
{
    m_editItem = item;
    m_editIndex = item->data(Qt::UserRole).toInt();
    if(m_pointChecker[m_editIndex])
    {
        m_pointEdit = true;
        m_point.show();
    }
    else if(m_circleChecker[m_editIndex])
    {
        m_circleEdit = true;
        m_circle.show();
    }
    else if(m_placeChecker[m_editIndex])
    {
        m_placeEdit = true;
        m_placeForm.show();
    }
    else if(m_pointByPlace[m_editIndex])
    {
        m_placeEdit = true;
        emit pointOnPlace(m_commandPointName, m_commandPlaceName);
        m_pointOnPlace.show();
    }
    else
    {
        bool ok;
        QString newText = QInputDialog::getText(this, "Редактировать", "Новое значение:",
                                                QLineEdit::Normal, item->text(), &ok);

        if (ok && !newText.isEmpty())
            item->setText(newText);
    }
}

void MainWindow::deleteItem(QListWidgetItem *item)
{
    int currentId = item->data(Qt::UserRole).toInt();

    for (auto it = m_dependence.constBegin(); it != m_dependence.constEnd(); ++it)
    {
        const QVector<int>& vector = it.value();

        for (int element : vector)
        {
            if(element == currentId)
            {
                QMessageBox::critical(
                    nullptr,
                    "Ошибка",
                    "Нельзя удалить зависимый элемент.",
                    QMessageBox::Ok
                    );
                return;
            }
        }
    }
    m_dependence[currentId].clear();
    delete item;

}

QVector3D generateVector(double min, double max)
{
    QVector3D temp;
    float x = min + (QRandomGenerator::global()->generateDouble() * (max - min));
    float y = min + (QRandomGenerator::global()->generateDouble() * (max - min));
    float z = min + (QRandomGenerator::global()->generateDouble() * (max - min));

    temp = {x, y, z};
    return temp;
}


        /*      SLOTS       */
void MainWindow::commentSlot(const QString &text)
{
    QString command = QString("COMMENT: %1").arg(text);
    ui->m_commandList->addItem(command);
}

        /*
         *      Создание точки
         *
         *      Да я знаю, выглядит отвратительно, да я знаю как сделать лучше
         *      Почему не сделал?
         *      Потому что cделать костыль было быстрее, а еще дольше затягивать это тестовое я не могу
         *
         */
void MainWindow::createPointSlot(const QString& name, const QVector3D& point, const QVector3D& normale)
{
    QString tempName = name;
    if(m_pointEdit)
    {
        tempName = m_commandPointName[m_editIndex];
        m_commandPoints[m_editIndex] = point;
        m_comandActualPoints[m_editIndex] = point + generateVector(-0.2, 0.2);
        m_commandNormales[m_editIndex] = normale;
        m_commandActualNormales[m_editIndex] = normale + generateVector(-0.2, 0.2);
        m_commandPointName[m_editIndex] = tempName;
        m_pointChecker[m_editIndex] = true;

        QString nominal = QString("(%1, %2, %3) [%4, %5, %6]")
                              .arg(point.x())
                              .arg(point.y())
                              .arg(point.z())
                              .arg(normale.x())
                              .arg(normale.y())
                              .arg(normale.z());

        QString actual = QString("(%1, %2, %3) [%1, %2, %3]")
                             .arg(m_comandActualPoints[m_editIndex].x())
                             .arg(m_comandActualPoints[m_editIndex].y())
                             .arg(m_comandActualPoints[m_editIndex].z())
                             .arg(m_commandActualNormales[m_editIndex].x())
                             .arg(m_commandActualNormales[m_editIndex].y())
                             .arg(m_commandActualNormales[m_editIndex].z());

        QString command = QString("%1:\nNOMINAL: %2\nACTUAL: %3")
                              .arg(tempName)
                              .arg(nominal)
                              .arg(actual);


        m_editItem->setText(command);

        for (auto it = m_dependence.constBegin(); it != m_dependence.constEnd(); ++it)
        {
            int key = it.key();
            const QVector<int>& vector = it.value();

            for (int element : vector)
            {
                // qDebug() << element;
                if((element == m_editIndex) && m_moveChecker[key])
                {
                    // qDebug() << "SDSDS";
                    QVector3D newCoord = /*m_comandActualPoints[m_editIndex] + */m_commandMoveVector[key];

                    QString actual = QString("(%1, %2, %3)")
                                         .arg(newCoord.x())
                                         .arg(newCoord.y())
                                         .arg(newCoord.z());

                    QString commandText = QString("MOVE %1:\nACTUAL: %2")
                                              .arg(m_commandPointName[m_editIndex])
                                              .arg(actual);
                    m_listItem[key]->setText(commandText);
                }

                if ((element == m_editIndex) && m_placeChecker[key])
                {
                    QVector3D middle{0,0,0};
                    QVector3D normale;
                    QVector3D middleActual{0,0,0};
                    QVector3D normaleActual;

                    for(int i = 0; i < vector.size(); i++)
                    {
                        middle += m_commandPoints[m_dependence[key].at(i)] + generateVector(-0.2, 0.2);
                        normale += m_commandNormales[m_dependence[key].at(i)] + generateVector(-0.2, 0.2);
                        middleActual += m_comandActualPoints[m_dependence[key].at(i)] + generateVector(-0.2, 0.2);
                        normaleActual += m_commandActualNormales[m_dependence[key].at(i)] + generateVector(-0.2, 0.2);
                    }

                    normale.normalize();
                    middle /= (float)vector.size();

                    m_commandPoints[key]  = middle;
                    m_commandPlace[key]   = middle;
                    // m_commandPlaceName[key] = name;
                    // m_dependence[m_editIndex]     = index;
                    // m_placeChecker[m_editIndex]   = true;
                    QString actual = QString("(%1, %2, %3) [%4, %5, %6]")
                                              .arg(middleActual[0])
                                              .arg(middleActual[1])
                                              .arg(middleActual[2])
                                              .arg(normaleActual[0])
                                              .arg(normaleActual[1])
                                              .arg(normaleActual[2]);

                    QString coordString = QString("(%1, %2, %3) [%4, %5, %6]")
                                              .arg(middle[0])
                                              .arg(middle[1])
                                              .arg(middle[2])
                                              .arg(normale[0])
                                              .arg(normale[1])
                                              .arg(normale[2]);

                    QString command = QString("%1:\nPOSITION: %2\n ACTUAL: %3")
                                          .arg(m_commandPlaceName[key])
                                          .arg(coordString)
                                          .arg(actual);
                    m_listItem[key]->setText(command);
                }
                if ((element == m_editIndex) && m_pointErrorChecker[key])
                {
                    QString name = m_errorName[key];

                    m_errorName[key] = name;
                    float error = sqrtf(pow(m_commandPoints[element].x() - m_comandActualPoints[element].x(), 2)
                                        + pow(m_commandPoints[element].y() - m_comandActualPoints[element].y(), 2)
                                        + pow(m_commandPoints[element].z() - m_comandActualPoints[element].z(), 2));

                    m_dependence[key] = QVector<int>(element);

                    m_pointErrors[key] = error;

                    QString errorString = QString("%1(%2)")
                                              .arg(error)
                                              .arg(m_error);
                    QString command = QString("%1:\n ERROR: %2")
                                          .arg(name)
                                          .arg(errorString);
                    m_listItem[key]->setText(command);
                }

                // if((element == m_editIndex) && m_pointByPlace[key])
                // {
                //     QString name = m_commandPointName[m_editIndex];
                //     QVector3D vecToPoint = m_comandActualPoints[element] - m_commandPlace[vector[1]];
                //     double distance = QVector3D::dotProduct(vecToPoint, m_commandNormales[vector[1]]);

                //     QVector3D projection = m_comandActualPoints[element]
                //                            - distance *  m_commandNormales[vector[1]];


                //     // Сохраняем точку и нормаль в QMap
                //     m_commandPoints[key] = projection;
                //     m_comandActualPoints[key] = projection;
                //     m_commandNormales[key] = m_commandNormales[vector[1]];
                //     m_dependence[key] = QVector<int>(element, vector[1]);


                //     QString actual = QString("(%1, %2, %3) [%4, %5, %6]")
                //                          .arg(projection.x())
                //                          .arg(projection.y())
                //                          .arg(projection.z())
                //                          .arg(m_commandNormales[vector[1]].x())
                //                          .arg(m_commandNormales[vector[1]].y())
                //                          .arg(m_commandNormales[vector[1]].z());


                //     QString command = QString("%1:\nACTUAL: %2")
                //                           .arg(name)
                //                           .arg(actual);
                //     m_listItem[key]->setText(command);
                // }
            }
        }

        m_pointEdit = false;
        m_editIndex = -1;
    }
    else
    {
        if(name.isEmpty())
        {
            tempName = QString("POINT%1").arg(m_pointCounter);
            m_pointCounter++;
        }

        // Создаем уникальный ID для команды
        int commandId = ++m_commandCounter;


        // Сохраняем точку и нормаль в QMap
        m_commandPoints[commandId] = point;
        m_comandActualPoints[commandId] = point + generateVector(-0.2, 0.2);
        m_commandNormales[commandId] = normale;
        m_commandActualNormales[commandId] = normale + generateVector(-0.2, 0.2);
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

        QString actual = QString("(%1, %2, %3) [%4, %5, %6]")
                             .arg(m_comandActualPoints[commandId].x())
                             .arg(m_comandActualPoints[commandId].y())
                             .arg(m_comandActualPoints[commandId].z())
                             .arg(m_commandActualNormales[commandId].x())
                             .arg(m_commandActualNormales[commandId].y())
                             .arg(m_commandActualNormales[commandId].z());

        QString command = QString("%1:\nNOMINAL: %2\nACTUAL: %3")
                              .arg(tempName)
                              .arg(nominal)
                              .arg(actual);

        // Добавляем команду в QListWidget
        QListWidgetItem* item = new QListWidgetItem(command);
        item->setData(Qt::UserRole, commandId); //  ID команды в элементе списка
        ui->m_commandList->addItem(item);
        m_listItem[commandId] = item;
    }
}
    /*
     *  Создание точки на плоскости
     *
     */
void MainWindow::pointAndPlace(int pointIndex, int placeIndex)
{
    if(m_placeEdit)
    {
        QString name = QString("%1").arg(m_commandPointName[m_editIndex]);
        QVector3D vecToPoint = m_commandPoints[pointIndex] - m_commandPlace[placeIndex];
        QVector3D vecToPointActual = m_comandActualPoints[pointIndex] - m_commandPlace[placeIndex];
        double distance = QVector3D::dotProduct(vecToPoint, m_commandNormales[placeIndex]);

        QVector3D projection = m_commandPoints[pointIndex] - distance *  m_commandNormales[placeIndex];
        QVector3D projectionActual = m_comandActualPoints[pointIndex] - distance *  m_commandActualNormales[placeIndex];


        // Сохраняем точку и нормаль в QMap
        m_commandPoints[m_editIndex] = projection;
        m_comandActualPoints[m_editIndex] = vecToPointActual;
        m_commandNormales[m_editIndex] = m_commandNormales[placeIndex];
        m_commandActualNormales[m_editIndex] = m_commandActualNormales[placeIndex];
        m_dependence[m_editIndex] = QVector<int>(pointIndex, placeIndex);


        QString nominal = QString ("(%1, %2, %3 [%4, %5, %6]")
                              .arg(projection.x())
                              .arg(projection.y())
                              .arg(projection.z())
                              .arg(m_commandNormales[placeIndex].x())
                              .arg(m_commandNormales[placeIndex].y())
                              .arg(m_commandNormales[placeIndex].z());
        QString actual = QString("(%1, %2, %3) [%4, %5, %6]")
                             .arg(projectionActual.x())
                             .arg(projectionActual.y())
                             .arg(projectionActual.z())
                             .arg(m_commandActualNormales[placeIndex].x())
                             .arg(m_commandActualNormales[placeIndex].y())
                             .arg(m_commandActualNormales[placeIndex].z());


        QString command = QString("%1:\nACTUAL: %2")
                              .arg(name)
                              .arg(actual);
        m_editItem->setText(command);
        m_placeEdit = false;

    }
    else
    {
        QString name = QString("POINT%1").arg(m_pointCounter);

        QVector3D vecToPoint = m_commandPoints[pointIndex] - m_commandPlace[placeIndex];
        QVector3D vecToPointActual = m_comandActualPoints[pointIndex] - m_commandPlace[placeIndex];

        double distance = QVector3D::dotProduct(vecToPoint, m_commandNormales[placeIndex]);

        QVector3D projection = m_comandActualPoints[pointIndex] - distance *  m_commandNormales[placeIndex];
        QVector3D projectionActual = m_comandActualPoints[pointIndex] - distance *  m_commandActualNormales[placeIndex];

        // Создаем уникальный ID для команды
        int commandId = ++m_commandCounter;

        // Сохраняем точку и нормаль в QMap
        m_commandPoints[commandId] = projection;
        m_comandActualPoints[commandId] = vecToPointActual;
        m_commandNormales[commandId] = m_commandNormales[placeIndex];
        m_commandActualNormales[commandId] = m_commandActualNormales[placeIndex];
        m_commandPointName[commandId] = name;
        m_pointByPlace[commandId] = true;
        m_dependence[commandId] = QVector<int>(pointIndex, placeIndex);

        QString nominal = QString("(%1, %2, %3) [%4, %5, %6]")
                              .arg(projection.x())
                              .arg(projection.y())
                              .arg(projection.z())
                              .arg(m_commandNormales[placeIndex].x())
                              .arg(m_commandNormales[placeIndex].y())
                              .arg(m_commandNormales[placeIndex].z());

        QString actual = QString("(%1, %2, %3) [%4, %5, %6]")
                             .arg(projectionActual.x())
                             .arg(projectionActual.y())
                             .arg(projectionActual.z())
                             .arg(m_commandActualNormales[placeIndex].x())
                             .arg(m_commandActualNormales[placeIndex].y())
                             .arg(m_commandActualNormales[placeIndex].z());


        QString command = QString("%1:\nACTUAL: %2")
                              .arg(name)
                              .arg(actual);

        QListWidgetItem* item = new QListWidgetItem(command);
        item->setData(Qt::UserRole, commandId); // ID команды в элементе списка
        ui->m_commandList->addItem(item);
    }
}


            /*
             * Перемещение точки MOVE
             */
void MainWindow::moveSlot(const QVector3D& moveVector, int poinIndex)
{

    // QListWidgetItem* selectedItem = ui->m_commandList->currentItem();
    // if (!selectedItem)
    // {
    //     qWarning() << "Не выбран элемент для обновления!";
    //     return;
    // }

    int commandId = poinIndex;
    int newCommandId = ++m_commandCounter;

    QVector3D newCoord = m_comandActualPoints[commandId] + moveVector;
    // qDebug() << m_comandActualPoints[commandId];
    // m_commandMoveVector[newCommandId] = moveVector;
    m_comandActualPoints[commandId] += m_commandMoveVector[newCommandId];
    m_moveChecker[newCommandId] = true;
    // Форматируем координаты в строку
    QString actual = QString("(%1, %2, %3)")
                         .arg(newCoord.x())
                         .arg(newCoord.y())
                         .arg(newCoord.z());


    // Формируем текст команды
    QString commandText = QString("MOVE:\nACTUAL: %2")
                              .arg(actual);

    QVector<int> temp;
    temp.append(commandId);
    m_dependence[newCommandId] = temp;
    // qDebug() << m_dependence[newCommandId];

    // Добавляем команду в QListWidget
    QListWidgetItem* item = new QListWidgetItem(commandText);
    item->setData(Qt::UserRole, newCommandId); // ID команды в элементе списка
    ui->m_commandList->addItem(item);
    m_listItem[newCommandId] = item;

}


        /*
         *          Создание и изменение круга
         */
void MainWindow::circleParams(QVector<int> index,
                              float radius)
{

    if(m_circleEdit)
    {
        QVector3D middle;
        QVector3D normale;
        QVector3D middleActual;
        QVector3D normaleActual;

        for(int i = 0; i < index.size(); i++)
        {
            middle += m_commandPoints[index.at(i)];
            normale += m_commandNormales[index.at(i)];
            middleActual += m_comandActualPoints[index.at(i)];
            normaleActual += m_commandActualNormales[index.at(i)];
        }

        normale.normalize();
        middle /= (float)index.size();

        int commandId = ++m_commandCounter;
        m_commandPoints[m_editIndex]      = middle;
        m_commandCircleCenter[m_editIndex] = middle;
        m_comandActualPoints[m_editIndex]  = middleActual;
        m_commandNormales[m_editIndex]     = normale;
        m_commandActualNormales[m_editIndex] = normaleActual;
        m_commandCircleRadius[m_editIndex] = radius;
        m_circleChecker[m_editIndex]       = true;

        QString actual = QString("(%1, %2, %3) [%4, %5, %6]")
                             .arg(middleActual[0])
                             .arg(middleActual[1])
                             .arg(middleActual[2])
                             .arg(normaleActual[0])
                             .arg(normaleActual[1])
                             .arg(normaleActual[2]);

        QString coordString = QString("(%1, %2, %3) [%4, %5, %6]")
                                  .arg(middle[0])
                                  .arg(middle[1])
                                  .arg(middle[2])
                                  .arg(normale[0])
                                  .arg(normale[1])
                                  .arg(normale[2]);

        QString command = QString("%1:\nPOSITION: %2\nRADIUS: %3")
                              .arg(m_commandCircleName[commandId])
                              .arg(coordString)
                              .arg(radius);

        m_editItem->setText(command);

        for (auto it = m_dependence.constBegin(); it != m_dependence.constEnd(); ++it)
        {
            int key = it.key();
            const QVector<int>& vector = it.value();

            for (int element : vector)
            {
                if ((element == m_editIndex) && m_pointByCircleChecker[key])
                {

                    QVector3D coord = m_commandPoints[element];
                    QVector3D normale = m_commandNormales[element];

                    m_commandPoints[key] = coord;
                    m_comandActualPoints[key] = coord;
                    m_commandNormales[key] = normale;

                    QString nominal = QString("(%1, %2, %3) [%4, %5, %6]")
                                          .arg(coord[0])
                                          .arg(coord[1])
                                          .arg(coord[2])
                                          .arg(normale[0])
                                          .arg(normale[1])
                                          .arg(normale[2]);

                    QString actual = QString("(%1, %2, %3)")
                                         .arg(m_comandActualPoints[element].x())
                                         .arg(m_comandActualPoints[element].y())
                                         .arg(m_comandActualPoints[element].z());

                    QString command = QString("%1:\nNOMINAL: %2\nACTUAL: %3")
                                          .arg(m_commandCircleName[key])
                                          .arg(nominal)
                                          .arg(actual);
                    m_listItem[key]->setText(command);
                }
            }
        }
        m_circleEdit = false;
    }
    else
    {


        QString tempName = QString("CIRCLE%1: %2 %3 %4")
                        .arg(m_circleCounter)
                        .arg(m_commandPointName[index[0]])
                        .arg(m_commandPointName[index[1]])
                        .arg(m_commandPointName[index[2]]);
        m_circleCounter++;


        QVector3D middle;
        QVector3D normale;
        QVector3D middleActual;
        QVector3D normaleActual;

        for(int i = 0; i < index.size(); i++)
        {
            middle += m_commandPoints[index.at(i)];
            normale += m_commandNormales[index.at(i)];
            middleActual += m_comandActualPoints[index.at(i)];
            normaleActual += m_commandActualNormales[index.at(i)];
        }

        normale.normalize();
        middle /= (float)index.size();

        int commandId = ++m_commandCounter;
        m_commandPoints[commandId]      = middle;
        m_commandCircleCenter[commandId] = middle;
        m_comandActualPoints[commandId]  = middleActual;
        m_commandNormales[commandId]     = normale;
        m_commandActualNormales[commandId] = normaleActual;
        m_commandCircleRadius[commandId] = radius;
        m_commandCircleName[commandId]   = tempName;
        m_circleChecker[commandId]       = true;

        QString actual = QString("(%1, %2, %3) [%4, %5, %6]")
                             .arg(middleActual[0])
                             .arg(middleActual[1])
                             .arg(middleActual[2])
                             .arg(normaleActual[0])
                             .arg(normaleActual[1])
                             .arg(normaleActual[2]);

        QString coordString = QString("(%1, %2, %3) [%4, %5, %6]")
                                  .arg(middle[0])
                                  .arg(middle[1])
                                  .arg(middle[2])
                                  .arg(normale[0])
                                  .arg(normale[1])
                                  .arg(normale[2]);

        QString command = QString("%1:\nPOSITION: %2\nRADIUS: %3")
                              .arg(tempName)
                              .arg(coordString)
                              .arg(radius);

        QListWidgetItem* item = new QListWidgetItem(command);
        item->setData(Qt::UserRole, commandId); // ID команды в элементе списка
        ui->m_commandList->addItem(item);
        m_listItem[commandId] = item;
    }
}

            /*
             *
             *
             *          Добавь QMap<int, QVector<QWidgetItem>> Чтобы хранить в нем родителей элементов
             *          потом при редактировании родителя, пройдись по массиву в поиске итема и поменяй
             *          все завимимые итемы, сегодня конечно на это времени нет,
             *          но завтра после работы в самый раз.
             *
             *
             *
             *
             *
             */



            /*
             *          Создание и изменение плоскости
             */
void MainWindow::placePoints(QVector<int> index)
{
    if(m_placeEdit)
    {

        m_dependence[m_editIndex] = index;
        m_editIndex = m_editItem->data(Qt::UserRole).toInt();
        QString name = QString("PLACE%1 %2 %3 %4")
                           .arg(m_placeCounter)
                           .arg(m_commandPointName[m_dependence[m_editIndex][0]])
                           .arg(m_commandPointName[m_dependence[m_editIndex][1]])
                           .arg(m_commandPointName[m_dependence[m_editIndex][2]]);
        m_placeCounter++;

        QVector3D middle{0,0,0};
        QVector3D normale;
        QVector3D middleActual{0,0,0};
        QVector3D normaleActual;


        for(int i = 0; i < m_dependence[m_editIndex].size(); i++)
        {
            middle += m_commandPoints[m_dependence[m_editIndex].at(i)];
            normale += m_commandNormales[m_dependence[m_editIndex].at(i)];
            middleActual += m_comandActualPoints[m_dependence[m_editIndex].at(i)];
            normaleActual += m_commandActualNormales[m_dependence[m_editIndex].at(i)];
        }

        normale.normalize();
        middle /= (float)index.size();

        m_commandPoints[m_editIndex]  = middle;
        m_commandPlace[m_editIndex]   = middle;
        m_commandPlaceName[m_editIndex] = name;
        // m_dependence[m_editIndex]     = index;
        // m_placeChecker[m_editIndex]   = true;

        QString actual = QString("(%1, %2, %3) [%4, %5, %6]")
                             .arg(middleActual[0])
                             .arg(middleActual[1])
                             .arg(middleActual[2])
                             .arg(normaleActual[0])
                             .arg(normaleActual[1])
                             .arg(normaleActual[2]);

        QString coordString = QString("(%1, %2, %3) [%4, %5, %6]")
                                  .arg(middle[0])
                                  .arg(middle[1])
                                  .arg(middle[2])
                                  .arg(normale[0])
                                  .arg(normale[1])
                                  .arg(normale[2]);

        QString command = QString("%1:\nPOSITION: %2\n ACTUAL: %3")
                              .arg(name)
                              .arg(coordString)
                              .arg(actual);

        m_editItem->setText(command);
        m_placeEdit = false;
    }

    else
    {

        QString name = QString("PLACE%1 %2 %3 %4")
        .arg(m_placeCounter)
            .arg(m_commandPointName[index[0]])
            .arg(m_commandPointName[index[1]])
            .arg(m_commandPointName[index[2]]);
        m_placeCounter++;

        QVector3D middle{0,0,0};
        QVector3D normale;
        QVector3D middleActual{0,0,0};
        QVector3D normaleActual;

        for(int i = 0; i < index.size(); i++)
        {
            middle += m_comandActualPoints[index.at(i)];
            normale += m_commandNormales[index.at(i)];
            middleActual += m_comandActualPoints[index.at(i)];
            normaleActual += m_commandActualNormales[index.at(i)];
        }

        normale.normalize();
        middle /= (float)index.size();

        int commandId = ++m_commandCounter;
        m_commandPoints[commandId]  = middle;
        m_commandPlace[commandId]   = middle;
        m_commandPlaceName[commandId] = name;
        m_placeChecker[commandId]   = true;
        m_dependence[commandId]     = index;

        // qDebug() << m_dependence[commandId];

        QString actual = QString("(%1, %2, %3) [%4, %5, %6]")
                             .arg(middleActual[0])
                             .arg(middleActual[1])
                             .arg(middleActual[2])
                             .arg(normaleActual[0])
                             .arg(normaleActual[1])
                             .arg(normaleActual[2]);

        QString coordString = QString("(%1, %2, %3) [%4, %5, %6]")
                                  .arg(middle[0])
                                  .arg(middle[1])
                                  .arg(middle[2])
                                  .arg(normale[0])
                                  .arg(normale[1])
                                  .arg(normale[2]);

        QString command = QString("%1:\nPOSITION: %2\n ACTUAL: %3")
                              .arg(name)
                              .arg(coordString)
                              .arg(actual);

        QListWidgetItem* item = new QListWidgetItem(command);
        item->setData(Qt::UserRole, commandId); // Сохраняем ID команды в элементе списка
        ui->m_commandList->addItem(item);
        m_listItem[commandId] = item;
    }
}

            /*
             *      Создание точки по кругу
             */
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
    m_comandActualPoints[newCommandId] = coord + generateVector(-0.5, 0.5);
    m_commandNormales[newCommandId] = normale;
    m_commandActualNormales[newCommandId] = normale + generateVector(-0.5, 0.5);
    m_pointChecker[newCommandId] = true;
    m_pointByCircleChecker[newCommandId] = true;
    m_dependence[commandId].append(newCommandId);

    QString nominal = QString("(%1, %2, %3) [%4, %5, %6]")
                              .arg(coord[0])
                              .arg(coord[1])
                              .arg(coord[2])
                              .arg(normale[0])
                              .arg(normale[1])
                              .arg(normale[2]);

    QString actual = QString("(%1, %2, %3) [%4, %5, %6]")
                         .arg(m_comandActualPoints[commandId].x())
                         .arg(m_comandActualPoints[commandId].y())
                         .arg(m_comandActualPoints[commandId].z())
                         .arg(m_commandActualNormales[newCommandId].x())
                         .arg(m_commandActualNormales[newCommandId].y())
                         .arg(m_commandActualNormales[newCommandId].z());

    QString command = QString("%1:\nNOMINAL: %2\nACTUAL: %3")
                          .arg(name)
                          .arg(nominal)
                          .arg(actual);

    QListWidgetItem* item = new QListWidgetItem(command);
    item->setData(Qt::UserRole, newCommandId); // Сохраняем ID команды в элементе списка
    ui->m_commandList->addItem(item);
    m_listItem[commandId] = item;
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
    int newCommandId = ++m_commandCounter;

    if (!m_commandPoints.contains(commandId))
    {
        qWarning() << "Элемент с ID" << commandId << "не найден!";
        return;
    }

    QString name = QString("LOCATION%1").arg(m_locationCounter);

    m_errorName[newCommandId] = name;
    float error = sqrtf(pow(m_commandPoints[commandId].x() - m_comandActualPoints[commandId].x(), 2)
                        + pow(m_commandPoints[commandId].y() - m_comandActualPoints[commandId].y(), 2)
                        + pow(m_commandPoints[commandId].z() - m_comandActualPoints[commandId].z(), 2));

    m_dependence[newCommandId].append(commandId);

    m_pointErrors[newCommandId] = error;

    QString errorString = QString("%1(%2)")
                              .arg(error)
                              .arg(m_error);
    QString command = QString("%1:\n ERROR: %2")
                         .arg(name)
                         .arg(errorString);

    QListWidgetItem* item = new QListWidgetItem(command);
    item->setData(Qt::UserRole, newCommandId); // ID команды в элементе списка
    ui->m_commandList->addItem(item);
    m_listItem[newCommandId] = item;
}



        /*
         *          Ошибка для круга
         */

void MainWindow::errorCircleBtn()
{
    QListWidgetItem* selectedItem = ui->m_commandList->currentItem();

    if (!selectedItem)
    {
        qWarning() << "Не выбран элемент для обновления!";
        return;
    }

    int commandId = selectedItem->data(Qt::UserRole).toInt();
    int newCommandId = ++m_commandCounter;

    if (!m_commandPoints.contains(commandId))
    {
        qWarning() << "Элемент с ID" << commandId << "не найден!";
        return;
    }

    QString name = QString("LOCATION%1").arg(m_locationCounter);

    m_errorName[newCommandId] = name;

    float error = sqrtf(pow(m_commandPoints[commandId].x() - m_comandActualPoints[commandId].x(), 2)
                        + pow(m_commandPoints[commandId].y() - m_comandActualPoints[commandId].y(), 2)
                        + pow(m_commandPoints[commandId].z() - m_comandActualPoints[commandId].z(), 2));

    m_commandCircleActualRadius[commandId] = m_commandCircleRadius[commandId];
    float radiusError = m_commandCircleActualRadius[commandId] - m_commandCircleRadius[commandId];

    m_dependence[newCommandId].append(commandId);

    m_pointErrors[newCommandId] = error;
    m_radiusErrors[newCommandId] = radiusError;

    QString errorString = QString("%1(%2)")
                              .arg(error)
                              .arg(m_error);
    QString errorRadius = QString("%1(%2)")
                              .arg(radiusError)
                              .arg(m_error);

    QString command = QString("%1: \nPOSITION: %2\n RADIUS: %3")
                          .arg(name)
                          .arg(errorString)
                          .arg(errorRadius);

    QListWidgetItem* item = new QListWidgetItem(command);
    item->setData(Qt::UserRole, newCommandId); // ID команды в элементе списка
    ui->m_commandList->addItem(item);
}

void MainWindow::startBtn()
{
//     for (auto it = m_comandActualPoints.constBegin(); it != m_comandActualPoints.constEnd(); ++it)
//     {
//         it += QRandomGenerator::global()->generate(-0.05, 0.05);
//     }
}

void MainWindow::clearBtn()
{
    ui->m_commandList->clear();
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

void MainWindow::updateList()
{

}

void MainWindow::save()
{
    QFile file("./save/save.txt");
    QString defaultDir = "./save";
    QDir dir(defaultDir);

    if(!dir.exists())
    {
        dir.mkdir(defaultDir);
    }

    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
            out << &m_commandCoords << &m_commandPoints << &m_comandActualPoints;
            out << &m_commandNormales << &m_commandCircleCenter << &m_commandPlace;
            out << &m_commandCircleRadius << &m_commandCircleActualRadius;
            out << &m_commandPointName << &m_commandPlaceName << &m_commandCircleName;
            out << &m_commandMoveVector << &m_commandActualNormales;
            out << &m_pointErrors << &m_radiusErrors << &m_errorName;
            out << &m_pointChecker << &m_placeChecker << &m_circleChecker;
            out << &m_moveChecker << &m_pointByCircleChecker << &m_pointErrorChecker;
            out << &m_dependence;
    }
}

void MainWindow::load()
{
    QFile file("./save/save.txt");
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QDataStream in(&file);

    // Пример чтения переменных из файла
    in >> m_commandCoords >> m_commandPoints >> m_comandActualPoints;
    in >> m_commandNormales >> m_commandCircleCenter >> m_commandPlace;
    in >> m_commandCircleRadius >> m_commandCircleActualRadius;
    in >> m_commandPointName >> m_commandPlaceName >> m_commandCircleName;
    in >> m_commandMoveVector >> m_commandActualNormales;
    in >> m_pointErrors >> m_radiusErrors >> m_errorName;
    in >> m_pointChecker >> m_placeChecker >> m_circleChecker;
    in >> m_moveChecker >> m_pointByCircleChecker >> m_pointErrorChecker;
    in >> m_dependence;
}
/// 1к Строк кода которые необходимо удалить и переделать... ...
