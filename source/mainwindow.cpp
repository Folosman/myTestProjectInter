#include "./lib/mainwindow.h"
#include "ui_mainwindow.h"
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QListWidget>
#include <QRandomGenerator>
#include <QtMath>
#include <QDir>
#include <QFile>
#include <QDialog>

#include <QDebug>
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
    QObject::connect(&m_settingWidget, &SettingWidget::errorValue,  this, &MainWindow::errorSetter);

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
    connect(ui->actionLoad_File,    &QAction::triggered,    this, &MainWindow::load);
    connect(ui->setting,            &QAction::triggered,    this, &MainWindow::setting);
    connect(ui->m_startBtn,         &QPushButton::clicked,  this, &MainWindow::start);

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

void MainWindow::setting()
{
    m_settingWidget.show();
}

void MainWindow::errorSetter(int error)
{
    m_error = error;
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
void MainWindow::commentSlot(const QString &text, const bool &trash)
{
    QString command = QString("COMMENT: %1").arg(text);
    int commandId = ++m_commandCounter;
    m_commentdBool[commandId] = trash;
    m_commentChecker[commandId] = true;
    m_comentString[commandId] = command;
    m_commandPoints[commandId] = {0,0,0};
    ui->m_commandList->addItem(command);
}

        /*
         *      Создание точки
         */
void MainWindow::createPointSlot(const QString& name, const QVector3D& point, const QVector3D& normale)
{
    QString tempName = name;
    if(m_pointEdit)
    {
        tempName = m_commandPointName[m_editIndex];
        m_commandPoints[m_editIndex] = point;
        m_comandActualPoints[m_editIndex] = point + generateVector(-0.02, 0.02);
        m_commandNormales[m_editIndex] = normale;
        m_commandActualNormales[m_editIndex] = (normale + generateVector(-0.02, 0.02)).normalized();
        m_commandPointName[m_editIndex] = tempName;
        m_pointChecker[m_editIndex] = true;

        QString nominal = QString("(%1, %2, %3) [%4, %5, %6]")
                              .arg(point.x())
                              .arg(point.y())
                              .arg(point.z())
                              .arg(normale.x())
                              .arg(normale.y())
                              .arg(normale.z());

        QString actual = QString("(%1, %2, %3) [%4, %5, %6]")
                             .arg(m_commandPoints[m_editIndex].x())
                             .arg(m_commandPoints[m_editIndex].y())
                             .arg(m_commandPoints[m_editIndex].z())
                             .arg(m_commandNormales[m_editIndex].x())
                             .arg(m_commandNormales[m_editIndex].y())
                             .arg(m_commandNormales[m_editIndex].z());

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
                if ((element == m_editIndex) && m_placeChecker[key])
                {
                    QVector3D middle{0,0,0};
                    QVector3D middleActual{0,0,0};
                    QVector3D vectorOne = m_commandPoints[m_dependence[key].at(0)]
                                          - m_commandPoints[m_dependence[key].at(1)];

                    QVector3D vectorTwo = m_commandPoints[m_dependence[key].at(0)]
                                          - m_commandPoints[m_dependence[key].at(2)];

                    QVector3D normal = QVector3D(
                        vectorOne.y() * vectorTwo.z() - vectorOne.z() * vectorTwo.y(),
                        vectorOne.z() * vectorTwo.x() - vectorOne.x() * vectorTwo.z(),
                        vectorOne.x() * vectorTwo.y() - vectorOne.y() * vectorTwo.x());

                    QVector3D vectorOneActual = m_comandActualPoints[m_dependence[key].at(0)]
                                                - m_comandActualPoints[m_dependence[key].at(1)];

                    QVector3D vectorTwoActual = m_comandActualPoints[m_dependence[key].at(0)]
                                                - m_comandActualPoints[m_dependence[key].at(2)];

                    QVector3D normaleActual = QVector3D(
                        vectorOneActual.y() * vectorTwoActual.z() - vectorOneActual.z() * vectorTwoActual.y(),
                        vectorOneActual.z() * vectorTwoActual.x() - vectorOneActual.x() * vectorTwoActual.z(),
                        vectorOneActual.x() * vectorTwoActual.y() - vectorOneActual.y() * vectorTwoActual.x());


                    for(int i = 0; i < vector.size(); i++)
                    {
                        middle += m_commandPoints[m_dependence[key].at(i)];
                        middleActual += m_comandActualPoints[m_dependence[key].at(i)] + generateVector(-0.2, 0.2);
                    }

                    normal.normalize();
                    normaleActual.normalize();
                    middle /= (float)vector.size();
                    middleActual /= (float)vector.size();

                    m_commandPoints[key]  = middle;
                    m_comandActualPoints[key] = middleActual;
                    m_commandPlace[key]   = middle;
                    // m_commandPlaceName[key] = name;
                    // m_dependence[m_editIndex]     = index;
                    // m_placeChecker[m_editIndex]   = true;
                    QString actual = QString("(%1, %2, %3) [%4, %5, %6]")
                                              .arg(middle[0])
                                              .arg(middle[1])
                                              .arg(middle[2])
                                              .arg(normal[0])
                                              .arg(normal[1])
                                              .arg(normal[2]);

                    QString coordString = QString("(%1, %2, %3) [%4, %5, %6]")
                                              .arg(middle[0])
                                              .arg(middle[1])
                                              .arg(middle[2])
                                              .arg(normal[0])
                                              .arg(normal[1])
                                              .arg(normal[2]);

                    QString command = QString("%1:\nPOSITION: %2\n ACTUAL: %3")
                                          .arg(m_commandPlaceName[key])
                                          .arg(coordString)
                                          .arg(actual);
                    m_listItem[key]->setText(command);
                };
                if ((element == m_editIndex) && m_circleChecker[key])
                {
                    QVector3D oneVector = m_commandPoints[m_dependence[key].at(1)] - m_commandPoints[m_dependence[key].at(0)];
                    QVector3D twoVector = m_commandPoints[m_dependence[key].at(2)] - m_commandPoints[m_dependence[key].at(0)];
                    QVector3D normale = QVector3D::crossProduct(oneVector, twoVector).normalized();


                    QVector3D midAB = (m_commandPoints[m_dependence[key].at(0)] + m_commandPoints[m_dependence[key].at(1)]) / 2.0f;
                    QVector3D midAC = (m_commandPoints[m_dependence[key].at(0)] + m_commandPoints[m_dependence[key].at(2)]) / 2.0f;

                    QVector3D dirAB = QVector3D::crossProduct(normale, oneVector).normalized();
                    QVector3D dirAC = QVector3D::crossProduct(normale, twoVector).normalized();

                    float t = QVector3D::dotProduct(dirAC, midAB - midAC) / QVector3D::dotProduct(dirAC, dirAB);
                    QVector3D middle = (midAB + t * dirAB);

                    float radius = (m_commandPoints[m_dependence[key].at(0)] - middle).length() / 2.0f;


                    /*
                    *  Актуальные
                    */

                    QVector3D oneVectorActual = m_comandActualPoints[m_dependence[key].at(1)] - m_comandActualPoints[m_dependence[key].at(0)];
                    QVector3D twoVectorActual = m_comandActualPoints[m_dependence[key].at(2)] - m_comandActualPoints[m_dependence[key].at(0)];
                    QVector3D normaleActual = QVector3D::crossProduct(oneVectorActual, twoVectorActual).normalized();


                    QVector3D midActualAB = (m_comandActualPoints[m_dependence[key].at(1)] + m_comandActualPoints[m_dependence[key].at(0)]) / 2.0f;
                    QVector3D midActualAC = (m_comandActualPoints[m_dependence[key].at(2)] + m_comandActualPoints[m_dependence[key].at(0)]) / 2.0f;

                    QVector3D dirActualAB = QVector3D::crossProduct(normaleActual, oneVectorActual).normalized();
                    QVector3D dirActualAC = QVector3D::crossProduct(normaleActual, twoVectorActual).normalized();

                    float tActual = QVector3D::dotProduct(dirActualAC, midActualAB - midActualAC)
                                    / QVector3D::dotProduct(dirActualAC, dirActualAB);
                    QVector3D middleActual = (midActualAB + tActual * dirActualAB);

                    float radiusActual = (m_comandActualPoints[m_dependence[key].at(0)] - middleActual).length() /2.0f;

                    // normale.normalize();

                    m_commandPoints[key]        = middle;
                    m_commandCircleCenter[key]  = middle;
                    m_comandActualPoints[key]   = middleActual;
                    m_commandNormales[key]      = normale;
                    m_commandActualNormales[key] = normaleActual;
                    m_commandCircleRadius[key]  = radius;
                    m_commandCircleActualRadius[key] = radiusActual;

                    QString actual = QString("(%1, %2, %3) [%4, %5, %6]")
                                         .arg(m_commandCircleCenter[key].x() / 2)
                                         .arg(m_commandCircleCenter[key].y() / 2)
                                         .arg(m_commandCircleCenter[key].z() / 2)
                                         .arg(m_commandNormales[key].x())
                                         .arg(m_commandNormales[key].y())
                                         .arg(m_commandNormales[key].z());

                    QString coordString = QString("(%1, %2, %3) [%4, %5, %6]")
                                              .arg(m_commandCircleCenter[key].x() / 2)
                                              .arg(m_commandCircleCenter[key].y() / 2)
                                              .arg(m_commandCircleCenter[key].z() / 2)
                                              .arg(m_commandNormales[key].x())
                                              .arg(m_commandNormales[key].y())
                                              .arg(m_commandNormales[key].z());

                    QString command = QString("%1:\nPOSITION: %2\nACTUAL: %3\nRADIUS: %4 (%5)")
                                          .arg(m_commandCircleName[key])
                                          .arg(coordString)
                                          .arg(actual)
                                          .arg(QString::number(radius))
                                          .arg(QString::number(radius));
                    m_listItem[key]->setText(command);
                };
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
                };

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
        m_commandActualNormales[commandId] = (normale + generateVector(-0.2, 0.2)).normalized();
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
                             .arg(m_commandPoints[commandId].x())
                             .arg(m_commandPoints[commandId].y())
                             .arg(m_commandPoints[commandId].z())
                             .arg(m_commandNormales[commandId].x())
                             .arg(m_commandNormales[commandId].y())
                             .arg(m_commandNormales[commandId].z());

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
        double distanceActua = QVector3D::dotProduct(vecToPointActual, m_commandActualNormales[placeIndex]);

        QVector3D projection = m_commandPoints[pointIndex] - distance *  m_commandNormales[placeIndex];
        QVector3D projectionActual = m_comandActualPoints[pointIndex] - distanceActua *  m_commandActualNormales[placeIndex];


        // Сохраняем точку и нормаль в QMap
        m_commandPoints[m_editIndex] = projection;
        m_comandActualPoints[m_editIndex] = projectionActual;
        m_commandNormales[m_editIndex] = m_commandNormales[placeIndex];
        m_commandActualNormales[m_editIndex] = m_commandActualNormales[placeIndex];
        m_dependence[m_editIndex] = QVector<int>(pointIndex, placeIndex);
        m_pointByPlace[m_editIndex] = true;

        QString nominal = QString ("(%1, %2, %3 [%4, %5, %6]")
                              .arg(projection.x())
                              .arg(projection.y())
                              .arg(projection.z())
                              .arg(m_commandNormales[placeIndex].x())
                              .arg(m_commandNormales[placeIndex].y())
                              .arg(m_commandNormales[placeIndex].z());
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
        m_editItem->setText(command);
        m_placeEdit = false;

    }
    else
    {
        QString name = QString("POINT%1").arg(m_pointCounter);

        QVector3D vecToPoint = m_commandPoints[pointIndex] - m_commandPlace[placeIndex];
        QVector3D vecToPointActual = m_comandActualPoints[pointIndex] - m_commandPlace[placeIndex];

        double distance = QVector3D::dotProduct(vecToPoint, m_commandNormales[placeIndex]);
        double distanceActual = QVector3D::dotProduct(vecToPointActual, m_commandActualNormales[placeIndex]);

        QVector3D projection = m_comandActualPoints[pointIndex] - distance *  m_commandNormales[placeIndex];
        QVector3D projectionActual = m_comandActualPoints[pointIndex] - distanceActual *  m_commandActualNormales[placeIndex];

        // Создаем уникальный ID для команды
        int commandId = ++m_commandCounter;

        // Сохраняем точку и нормаль в QMap
        m_commandPoints[commandId] = projection;
        m_comandActualPoints[commandId] = projectionActual;
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
                             .arg(projection.x())
                             .arg(projection.y())
                             .arg(projection.z())
                             .arg(m_commandNormales[placeIndex].x())
                             .arg(m_commandNormales[placeIndex].y())
                             .arg(m_commandNormales[placeIndex].z());


        QString command = QString("%1:\nNOMINAL: %2\nACTUAL: %3")
                              .arg(name)
                              .arg(nominal)
                              .arg(actual);

        QListWidgetItem* item = new QListWidgetItem(command);
        item->setData(Qt::UserRole, commandId); // ID команды в элементе списка
        ui->m_commandList->addItem(item);
    }
}


            /*
             * Перемещение точки MOVE
             */
void MainWindow::moveSlot(const QVector3D& moveVector)
{

    // Форматируем координаты в строку
    int commandId = ++m_commandCounter;

    m_commandMoveVector[commandId]  = moveVector;
    m_comandActualPoints[commandId] = moveVector ;

    QString nominal = QString("(%1, %2, %3)")
                         .arg(moveVector.x())
                         .arg(moveVector.y())
                         .arg(moveVector.z());
    QString actual = QString("(%1, %2, %3)")
                         .arg(m_comandActualPoints[commandId].x())
                         .arg(m_comandActualPoints[commandId].y())
                         .arg(m_comandActualPoints[commandId].z());
    // Формируем текст команды
    QString commandText = QString("MOVE:\nNOMINAL: %1\nACTUAL: %2")
                              .arg(nominal)
                              .arg(actual);

    // qDebug() << m_dependence[newCommandId];

    // Добавляем команду в QListWidget
    QListWidgetItem* item = new QListWidgetItem(commandText);
    item->setData(Qt::UserRole, commandId); // ID команды в элементе списка
    ui->m_commandList->addItem(item);
    m_listItem[commandId] = item;

}


        /*
         *          Создание и изменение круга
         */
void MainWindow::circleParams(QVector<int> index)
{

    if(m_circleEdit)
    {

        QVector3D oneVector = m_commandPoints[index.at(1)] - m_commandPoints[index.at(0)];
        QVector3D twoVector = m_commandPoints[index.at(2)] - m_commandPoints[index.at(0)];
        QVector3D normale = QVector3D::crossProduct(oneVector, twoVector).normalized();


        QVector3D midAB = (m_commandPoints[index.at(0)] + m_commandPoints[index.at(1)]) / 2.0f;
        QVector3D midAC = (m_commandPoints[index.at(0)] + m_commandPoints[index.at(2)]) / 2.0f;

        QVector3D dirAB = QVector3D::crossProduct(normale, oneVector).normalized();
        QVector3D dirAC = QVector3D::crossProduct(normale, twoVector).normalized();

        float t = QVector3D::dotProduct(dirAC, midAB - midAC) / QVector3D::dotProduct(dirAC, dirAB);
        QVector3D middle = (midAB + t * dirAB);

        float radius = (m_commandPoints[index.at(0)] - middle).length() / 2.0f;


        /*
         *  Актуальные
         */

        QVector3D oneVectorActual = m_comandActualPoints[index.at(1)] - m_comandActualPoints[index.at(0)];
        QVector3D twoVectorActual = m_comandActualPoints[index.at(2)] - m_comandActualPoints[index.at(0)];
        QVector3D normaleActual = QVector3D::crossProduct(oneVectorActual, twoVectorActual).normalized();


        QVector3D midActualAB = (m_comandActualPoints[index.at(1)] + m_comandActualPoints[index.at(0)]) / 2.0f;
        QVector3D midActualAC = (m_comandActualPoints[index.at(2)] + m_comandActualPoints[index.at(0)]) / 2.0f;

        QVector3D dirActualAB = QVector3D::crossProduct(normaleActual, oneVectorActual).normalized();
        QVector3D dirActualAC = QVector3D::crossProduct(normaleActual, twoVectorActual).normalized();

        float tActual = QVector3D::dotProduct(dirActualAC, midActualAB - midActualAC)
                        / QVector3D::dotProduct(dirActualAC, dirActualAB);
        QVector3D middleActual = (midActualAB + tActual * dirActualAB);

        float radiusActual = (m_comandActualPoints[index.at(0)] - middleActual).length() /2.0f;

        // normale.normalize();

        m_commandPoints[m_editIndex]      = middle;
        m_commandCircleCenter[m_editIndex] = middle;
        m_comandActualPoints[m_editIndex]  = middleActual;
        m_commandNormales[m_editIndex]     = normale;
        m_commandActualNormales[m_editIndex] = normaleActual;
        m_commandCircleRadius[m_editIndex] = radius;
        m_commandCircleActualRadius[m_editIndex] = radiusActual;

        m_dependence[m_editIndex] = index;
        QString actual = QString("(%1, %2, %3) [%4, %5, %6]")
                             .arg(middle[0] / 2)
                             .arg(middle[1] / 2)
                             .arg(middle[2] / 2)
                             .arg(normale[0])
                             .arg(normale[1])
                             .arg(normale[2]);

        QString coordString = QString("(%1, %2, %3) [%4, %5, %6]")
                                  .arg(middle[0] / 2)
                                  .arg(middle[1] / 2)
                                  .arg(middle[2] / 2)
                                  .arg(normale[0])
                                  .arg(normale[1])
                                  .arg(normale[2]);

        QString command = QString("%1:\nPOSITION: %2\nACTUAL: %5\nRADIUS: %3 (%4)")
                              .arg(m_commandCircleName[m_editIndex])
                              .arg(coordString)
                              .arg(radius)
                              .arg(radius)
                              .arg(actual);

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

        /*
         *   Рассчетные данные
         */
        QVector3D oneVector = m_commandPoints[index.at(1)] - m_commandPoints[index.at(0)];
        QVector3D twoVector = m_commandPoints[index.at(2)] - m_commandPoints[index.at(0)];
        QVector3D normale   = QVector3D::crossProduct(oneVector, twoVector).normalized();


        QVector3D midAB = (m_commandPoints[index.at(0)] + m_commandPoints[index.at(1)]) / 2.0f;
        QVector3D midAC = (m_commandPoints[index.at(0)] + m_commandPoints[index.at(2)]) / 2.0f;

        QVector3D dirAB = QVector3D::crossProduct(normale, oneVector).normalized();
        QVector3D dirAC = QVector3D::crossProduct(normale, twoVector).normalized();

        float t = QVector3D::dotProduct(dirAC, midAB - midAC) / QVector3D::dotProduct(dirAC, dirAB);
        QVector3D middle = (midAB + t * dirAB);

        float radius = (m_commandPoints[index.at(0)] - middle).length() / 2.0f;


        /*
         *  Актуальные
         */

        QVector3D oneVectorActual = m_comandActualPoints[index.at(1)] - m_comandActualPoints[index.at(0)];
        QVector3D twoVectorActual = m_comandActualPoints[index.at(2)] - m_comandActualPoints[index.at(0)];
        QVector3D normaleActual = QVector3D::crossProduct(oneVectorActual, twoVectorActual).normalized();


        QVector3D midActualAB = (m_comandActualPoints[index.at(1)] + m_comandActualPoints[index.at(0)]) / 2.0f;
        QVector3D midActualAC = (m_comandActualPoints[index.at(2)] + m_comandActualPoints[index.at(0)]) / 2.0f;

        QVector3D dirActualAB = QVector3D::crossProduct(normaleActual, oneVectorActual).normalized();
        QVector3D dirActualAC = QVector3D::crossProduct(normaleActual, twoVectorActual).normalized();

        float tActual = QVector3D::dotProduct(dirActualAC, midActualAB - midActualAC)
                        / QVector3D::dotProduct(dirActualAC, dirActualAB);
        QVector3D middleActual = (midActualAB + tActual * dirActualAB);

        float radiusActual = (m_comandActualPoints[index.at(0)] - middleActual).length() /2.0f;

        // normale.normalize();

        int commandId   = ++m_commandCounter;
        m_commandPoints[commandId]          = middle;
        m_commandCircleCenter[commandId]    = middle;
        m_comandActualPoints[commandId]     = middleActual;
        m_commandNormales[commandId]        = normale;
        m_commandActualNormales[commandId]  = normaleActual;
        m_commandCircleRadius[commandId]    = radius;
        m_commandCircleActualRadius[commandId] = radiusActual;
        m_commandCircleName[commandId]   = tempName;
        m_circleChecker[commandId]       = true;

        m_dependence[commandId] = index;

        QString actual = QString("(%1, %2, %3) [%4, %5, %6]")
                             .arg(middle[0] / 2)
                             .arg(middle[1] / 2)
                             .arg(middle[2] / 2)
                             .arg(normale[0])
                             .arg(normale[1])
                             .arg(normale[2]);

        QString coordString = QString("(%1, %2, %3) [%4, %5, %6]")
                                  .arg(middle[0] / 2)
                                  .arg(middle[1] / 2)
                                  .arg(middle[2] / 2)
                                  .arg(normale[0])
                                  .arg(normale[1])
                                  .arg(normale[2]);

        QString command = QString("%1:\nPOSITION: %2\nACTUAL: %5\nRADIUS: %3 (%4)")
                              .arg(tempName)
                              .arg(coordString)
                              .arg(radius)
                              .arg(radius)
                              .arg(actual);

        QListWidgetItem* item = new QListWidgetItem(command);
        item->setData(Qt::UserRole, commandId); // ID команды в элементе списка
        ui->m_commandList->addItem(item);
        m_listItem[commandId] = item;
    }
}
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
        QVector3D middleActual{0,0,0};
        QVector3D vectorOne = m_commandPoints[index.at(0)] - m_commandPoints[index.at(1)];
        QVector3D vectorTwo = m_commandPoints[index.at(0)] - m_commandPoints[index.at(2)];

        QVector3D normal = QVector3D(
            vectorOne.y() * vectorTwo.z() - vectorOne.z() * vectorTwo.y(),
            vectorOne.z() * vectorTwo.x() - vectorOne.x() * vectorTwo.z(),
            vectorOne.x() * vectorTwo.y() - vectorOne.y() * vectorTwo.x());

        QVector3D vectorOneActual = m_comandActualPoints[index.at(0)] - m_comandActualPoints[index.at(1)];
        QVector3D vectorTwoActual = m_comandActualPoints[index.at(0)] - m_comandActualPoints[index.at(2)];

        QVector3D normaleActual = QVector3D(
            vectorOneActual.y() * vectorTwoActual.z() - vectorOneActual.z() * vectorTwoActual.y(),
            vectorOneActual.z() * vectorTwoActual.x() - vectorOneActual.x() * vectorTwoActual.z(),
            vectorOneActual.x() * vectorTwoActual.y() - vectorOneActual.y() * vectorTwoActual.x());

        for(int i = 0; i < m_dependence[m_editIndex].size(); i++)
        {
            middle += m_commandPoints[m_dependence[m_editIndex].at(i)];
            middleActual += m_comandActualPoints[m_dependence[m_editIndex].at(i)];
        }

        normal.normalize();
        normaleActual.normalize();
        middle /= (float)index.size();
        middleActual /= (float)index.size();

        m_commandPoints[m_editIndex]  = middle;
        m_commandPlace[m_editIndex]   = middle;
        m_commandPlaceName[m_editIndex] = name;
        m_comandActualPoints[m_editIndex] = middleActual;
        m_commandNormales[m_editIndex] = normal;
        m_commandActualNormales[m_editIndex] = normaleActual;
        // m_dependence[m_editIndex]     = index;
        // m_placeChecker[m_editIndex]   = true;

        QString actual = QString("(%1, %2, %3) [%4, %5, %6]")
                             .arg(middle[0])
                             .arg(middle[1])
                             .arg(middle[2])
                             .arg(normal[0])
                             .arg(normal[1])
                             .arg(normal[2]);

        QString coordString = QString("(%1, %2, %3) [%4, %5, %6]")
                                  .arg(middle[0])
                                  .arg(middle[1])
                                  .arg(middle[2])
                                  .arg(normal[0])
                                  .arg(normal[1])
                                  .arg(normal[2]);

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
        QVector3D middleActual{0,0,0};
        QVector3D normaleActual;


        QVector3D vectorOne = m_commandPoints[index.at(0)] - m_commandPoints[index.at(1)];
        QVector3D vectorTwo = m_commandPoints[index.at(0)] - m_commandPoints[index.at(2)];
        QVector3D normal = QVector3D(
            vectorOne.y() * vectorTwo.z() - vectorOne.z() * vectorTwo.y(),
            vectorOne.z() * vectorTwo.x() - vectorOne.x() * vectorTwo.z(),
            vectorOne.x() * vectorTwo.y() - vectorOne.y() * vectorTwo.x());

        QVector3D vectorOneActual = m_comandActualPoints[index.at(0)] - m_comandActualPoints[index.at(1)];
        QVector3D vectorTwoActual = m_comandActualPoints[index.at(0)] - m_comandActualPoints[index.at(2)];

        normaleActual = QVector3D(
            vectorOneActual.y() * vectorTwoActual.z() - vectorOneActual.z() * vectorTwoActual.y(),
            vectorOneActual.z() * vectorTwoActual.x() - vectorOneActual.x() * vectorTwoActual.z(),
            vectorOneActual.x() * vectorTwoActual.y() - vectorOneActual.y() * vectorTwoActual.x());

        for(int i = 0; i < index.size(); i++)
        {
            middle += m_commandPoints[index.at(i)];
            middleActual += m_comandActualPoints[index.at(i)];
        }

        normal.normalize();
        normaleActual.normalize();
        middle /= (float)index.size();
        middleActual /= (float)index.size();

        int commandId = ++m_commandCounter;
        m_commandPoints[commandId]  = middle;
        m_commandPlace[commandId]   = middle;
        m_commandPlaceName[commandId] = name;
        m_commandNormales[commandId] = normal;
        m_comandActualPoints[commandId] = middleActual;
        m_commandActualNormales[commandId] = normaleActual;
        m_placeChecker[commandId]   = true;
        m_dependence[commandId]     = index;

        // qDebug() << m_dependence[commandId];

        QString actual = QString("(%1, %2, %3) [%4, %5, %6]")
                             .arg(middle[0])
                             .arg(middle[1])
                             .arg(middle[2])
                             .arg(normal[0])
                             .arg(normal[1])
                             .arg(normal[2]);

        QString coordString = QString("(%1, %2, %3) [%4, %5, %6]")
                                  .arg(middle[0])
                                  .arg(middle[1])
                                  .arg(middle[2])
                                  .arg(normal[0])
                                  .arg(normal[1])
                                  .arg(normal[2]);

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
    m_comandActualPoints[newCommandId] = m_comandActualPoints[commandId] ;
    m_commandNormales[newCommandId] = normale;
    m_commandActualNormales[newCommandId] = m_commandActualNormales[commandId] ;
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
                         .arg(m_commandPoints[commandId].x())
                         .arg(m_commandPoints[commandId].y())
                         .arg(m_commandPoints[commandId].z())
                         .arg(m_commandNormales[newCommandId].x())
                         .arg(m_commandNormales[newCommandId].y())
                         .arg(m_commandNormales[newCommandId].z());

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

    QString name = QString("LOCATION%1 %2")
                       .arg(m_locationCounter)
                       .arg(m_commandPointName[commandId]);

    m_errorName[newCommandId] = name;
    float error = sqrtf(pow(m_commandPoints[commandId].x() - m_comandActualPoints[commandId].x(), 2)
                        + pow(m_commandPoints[commandId].y() - m_comandActualPoints[commandId].y(), 2)
                        + pow(m_commandPoints[commandId].z() - m_comandActualPoints[commandId].z(), 2));

    m_dependence[newCommandId].append(commandId);

    m_pointErrors[newCommandId] = error;

    QString errorString = QString("%1 (%2)")
                              .arg(error)
                              .arg(m_error);
    QString command = QString("%1:\n POSITION: %2")
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
    QString errorRadius = QString("%1 / %2 (%3/%4)")
                              .arg(m_commandCircleRadius[commandId])
                              .arg(m_commandCircleActualRadius[commandId])
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
        QDataStream out(&file);
        out.setVersion(QDataStream::Qt_5_15);

        // Сохраняем QMap
        out << m_commandCoords
            << m_commandPoints
            << m_comandActualPoints
            << m_commandNormales
            << m_commandCircleCenter
            << m_commandPlace
            << m_commandCircleRadius
            << m_commandCircleActualRadius
            << m_commandPointName
            << m_commandPlaceName
            << m_commandCircleName
            << m_commandMoveVector
            << m_commandActualNormales
            << m_pointErrors
            << m_radiusErrors
            << m_errorName
            << m_pointChecker
            << m_placeChecker
            << m_circleChecker
            << m_moveChecker
            << m_pointByCircleChecker
            << m_pointErrorChecker
            << m_circleErrorCheker
            << m_pointByPlace
            << m_dependence;

        file.close();
    }
}

void MainWindow::load()
{
    QFile file("./save/save.txt");
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_15);

    // Загружаем QMap
    in >> m_commandCoords
        >> m_commandPoints
        >> m_comandActualPoints
        >> m_commandNormales
        >> m_commandCircleCenter
        >> m_commandPlace
        >> m_commandCircleRadius
        >> m_commandCircleActualRadius
        >> m_commandPointName
        >> m_commandPlaceName
        >> m_commandCircleName
        >> m_commandMoveVector
        >> m_commandActualNormales
        >> m_pointErrors
        >> m_radiusErrors
        >> m_errorName
        >> m_pointChecker
        >> m_placeChecker
        >> m_circleChecker
        >> m_moveChecker
        >> m_pointByCircleChecker
        >> m_pointErrorChecker
        >> m_circleErrorCheker
        >> m_pointByPlace
        >> m_dependence;


    for(auto it = m_commandPoints.constBegin(); it != m_commandPoints.constEnd(); ++it)
    {
        int key = it.key();
        if(m_pointChecker[key])
        {
            QString nominal = QString("(%1, %2, %3) [%4, %5, %6]")
                                  .arg(m_commandPoints[key].x())
                                  .arg(m_commandPoints[key].y())
                                  .arg(m_commandPoints[key].z())
                                  .arg(m_commandNormales[key].x())
                                  .arg(m_commandNormales[key].y())
                                  .arg(m_commandNormales[key].z());
            QString actual = QString("(%1, %2, %3) [%4, %5, %6]")
                                 .arg(m_commandPoints[key].x())
                                 .arg(m_commandPoints[key].y())
                                 .arg(m_commandPoints[key].z())
                                 .arg(m_commandNormales[key].x())
                                 .arg(m_commandNormales[key].y())
                                 .arg(m_commandNormales[key].z());
            QString command = QString("%1:\n NOMINAL: %2\n ACTUAL: %3\n")
                                  .arg(m_commandPointName[key])
                                  .arg(actual)
                                  .arg(nominal);
            QListWidgetItem* item = new QListWidgetItem(command);
            item->setData(Qt::UserRole, key); // ID команды в элементе списка
            ui->m_commandList->addItem(item);
        }
        if(m_placeChecker[key])
        {
            QString actual = QString("(%1, %2, %3) [%4, %5, %6]")
                                 .arg(m_commandPoints[key].x())
                                 .arg(m_commandPoints[key].y())
                                 .arg(m_commandPoints[key].z())
                                 .arg(m_commandNormales[key].x())
                                 .arg(m_commandNormales[key].y())
                                 .arg(m_commandNormales[key].z());

            QString coordString = QString("(%1, %2, %3) [%4, %5, %6]")
                                      .arg(m_commandPoints[key].x())
                                      .arg(m_commandPoints[key].y())
                                      .arg(m_commandPoints[key].z())
                                      .arg(m_commandNormales[key].x())
                                      .arg(m_commandNormales[key].y())
                                      .arg(m_commandNormales[key].z());

            QString command = QString("%1:\nPOSITION: %2\n ACTUAL: %3")
                                  .arg(m_commandPlaceName[key])
                                  .arg(coordString)
                                  .arg(actual);
            QListWidgetItem* item = new QListWidgetItem(command);
            item->setData(Qt::UserRole, key); // ID команды в элементе списка
            ui->m_commandList->addItem(item);
        }
        if(m_moveChecker[key])
        {
            QString nominal = QString("(%1, %2, %3)")
                                .arg(m_commandMoveVector[key].x())
                                .arg(m_commandMoveVector[key].y())
                                .arg(m_commandMoveVector[key].z());
            QString actual = QString("(%1, %2, %3)")
                                 .arg(m_comandActualPoints[key].x())
                                 .arg(m_comandActualPoints[key].y())
                                 .arg(m_comandActualPoints[key].z());
            // Формируем текст команды
            QString command = QString("MOVE:\nNOMINAL: %1\nACTUAL: %2")
                                      .arg(nominal)
                                      .arg(actual);
            QListWidgetItem* item = new QListWidgetItem(command);
            item->setData(Qt::UserRole, key); // ID команды в элементе списка
            ui->m_commandList->addItem(item);
        }
        if(m_circleChecker[key])
        {
            QString coordString = QString("(%1, %2, %3) [%4, %5, %6]")
                                    .arg(m_commandPoints[key].x() / 2)
                                    .arg(m_commandPoints[key].y() / 2)
                                    .arg(m_commandPoints[key].z() / 2)
                                    .arg(m_commandNormales[key].x())
                                    .arg(m_commandNormales[key].y())
                                    .arg(m_commandNormales[key].z());

            QString actual = QString("(%1, %2, %3) [%4, %5, %6]")
                                      .arg(m_commandPoints[key].x() / 2)
                                      .arg(m_commandPoints[key].y() / 2)
                                      .arg(m_commandPoints[key].z() / 2)
                                      .arg(m_commandNormales[key].x())
                                      .arg(m_commandNormales[key].y())
                                      .arg(m_commandNormales[key].z());

            QString command = QString("%1:\nPOSITION: %2\nACTUAL: %5\nRADIUS: %3 (%4)")
                                  .arg(m_commandCircleName[key])
                                  .arg(coordString)
                                  .arg(m_commandCircleRadius[key])
                                  .arg(m_commandCircleRadius[key])
                                  .arg(actual);
            QListWidgetItem* item = new QListWidgetItem(command);
            item->setData(Qt::UserRole, key); // ID команды в элементе списка
            ui->m_commandList->addItem(item);
        }
    }

}

void MainWindow::start()
{
    ui->m_startList->clear();

    for(auto it = m_commandPoints.constBegin(); it != m_commandPoints.constEnd(); ++it)
    {
        int key = it.key();
        if(m_pointChecker[key])
        {
            // m_comandActualPoints[key] += generateVector(-0.02, 0.02);
            // m_commandActualNormales[key] += generateVector(-0.02, 0.02);

            QString nominal = QString("(%1, %2, %3) [%4, %5, %6]")
                                 .arg(m_commandPoints[key].x())
                                 .arg(m_commandPoints[key].y())
                                 .arg(m_commandPoints[key].z())
                                 .arg(m_commandNormales[key].x())
                                 .arg(m_commandNormales[key].y())
                                 .arg(m_commandNormales[key].z());
            QString actual = QString("(%1, %2, %3) [%4, %5, %6]")
                                 .arg(m_comandActualPoints[key].x())
                                 .arg(m_comandActualPoints[key].y())
                                 .arg(m_comandActualPoints[key].z())
                                 .arg(m_commandActualNormales[key].x())
                                 .arg(m_commandActualNormales[key].y())
                                 .arg(m_commandActualNormales[key].z());
            QString command = QString("%1:\n NOMINAL: %2\n ACTUAL: %3\n")
                                  .arg(m_commandPointName[key])
                                  .arg(nominal)
                                  .arg(actual);
            QListWidgetItem* item = new QListWidgetItem(command);
            ui->m_startList->addItem(item);
        }
        if(m_placeChecker[key])
        {
            QString actual = QString("(%1, %2, %3) [%4, %5, %6]")
            .arg(m_comandActualPoints[key].x())
                .arg(m_comandActualPoints[key].y())
                .arg(m_comandActualPoints[key].z())
                .arg(m_commandActualNormales[key].x())
                .arg(m_commandActualNormales[key].y())
                .arg(m_commandActualNormales[key].z());

            QString coordString = QString("(%1, %2, %3) [%4, %5, %6]")
                                      .arg(m_commandPoints[key].x())
                                      .arg(m_commandPoints[key].y())
                                      .arg(m_commandPoints[key].z())
                                      .arg(m_commandNormales[key].x())
                                      .arg(m_commandNormales[key].y())
                                      .arg(m_commandNormales[key].z());

            QString command = QString("%1:\nPOSITION: %2\n ACTUAL: %3")
                                  .arg(m_commandPlaceName[key])
                                  .arg(coordString)
                                  .arg(actual);
            QListWidgetItem* item = new QListWidgetItem(command);
            ui->m_startList->addItem(item);
        }
        if(m_moveChecker[key])
        {
            QString nominal = QString("(%1, %2, %3)")
            .arg(m_commandMoveVector[key].x())
                .arg(m_commandMoveVector[key].y())
                .arg(m_commandMoveVector[key].z());
            QString actual = QString("(%1, %2, %3)")
                                 .arg(m_comandActualPoints[key].x())
                                 .arg(m_comandActualPoints[key].y())
                                 .arg(m_comandActualPoints[key].z());
            // Формируем текст команды
            QString command = QString("MOVE:\nNOMINAL: %1\nACTUAL: %2")
                                  .arg(nominal)
                                  .arg(actual);
            QListWidgetItem* item = new QListWidgetItem(command);
            ui->m_startList->addItem(item);
        }
        if(m_circleChecker[key])
        {
            QString coordString = QString("(%1, %2, %3) [%4, %5, %6]")
            .arg(m_commandPoints[key].x() / 2)
                .arg(m_commandPoints[key].y() / 2)
                .arg(m_commandPoints[key].z() / 2)
                .arg(m_commandNormales[key].x())
                .arg(m_commandNormales[key].y())
                .arg(m_commandNormales[key].z());

            QString actual = QString("(%1, %2, %3) [%4, %5, %6]")
                                 .arg(m_comandActualPoints[key].x() / 2)
                                 .arg(m_comandActualPoints[key].y() / 2)
                                 .arg(m_comandActualPoints[key].z() / 2)
                                 .arg(m_commandActualNormales[key].x())
                                 .arg(m_commandActualNormales[key].y())
                                 .arg(m_commandActualNormales[key].z());

            QString command = QString("%1:\nPOSITION: %2\nACTUAL: %5\nRADIUS: %3 (%4)")
                                  .arg(m_commandCircleName[key])
                                  .arg(coordString)
                                  .arg(m_commandCircleRadius[key])
                                  .arg(m_commandCircleActualRadius[key])
                                  .arg(actual);

            QListWidgetItem* item = new QListWidgetItem(command);
            ui->m_startList->addItem(item);
        }
        if(m_pointErrorChecker[key])
        {
            QString errorString = QString("%1 (%2)")
                                   .arg(m_pointErrors[key])
                                   .arg(m_error);
            QString command = QString("%1:\n POSITION: %2")
                                  .arg(m_errorName[key])
                                  .arg(errorString);

            QListWidgetItem* item = new QListWidgetItem(command);
            if(m_error < m_pointErrors[key])
            {
                item->setBackground(QBrush(QColor(Qt::red)));
            }
            ui->m_startList->addItem(item);
        }
        if(m_circleErrorCheker[key])
        {
            QString errorString = QString("%1(%2)")
                                       .arg(m_pointErrorChecker[key])
                                       .arg(m_error);
            QString errorRadius = QString("%1 / %2 (%3/%4)")
                                      .arg(m_commandCircleRadius[key])
                                      .arg(m_commandCircleActualRadius[key])
                                      .arg(m_radiusErrors[key])
                                      .arg(m_error);

            QString command = QString("%1: \nPOSITION: %2\n RADIUS: %3")
                                  .arg(m_errorName[key])
                                  .arg(errorString)
                                  .arg(errorRadius);

            QListWidgetItem* item = new QListWidgetItem(command);
            if(m_error < m_radiusErrors[key])
            {
                item->setBackground(QBrush(QColor(Qt::red)));
            }
            ui->m_startList->addItem(item);
        }
        if(m_commentChecker[key])
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Комментарий");
            msgBox.setText(m_comentString[key]);
            msgBox.setStandardButtons(QMessageBox::Ok);

            // Показываем диалог и останавливаем выполнение программы до нажатия OK
            msgBox.exec();

            QListWidgetItem* item = new QListWidgetItem(m_comentString[key]);
            ui->m_startList->addItem(item);
        }
    }
}
/// 1к Строк кода которые необходимо удалить и переделать... ...
