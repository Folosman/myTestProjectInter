#ifndef SAVEFILE_H
#define SAVEFILE_H

#include <QObject>
#include <QFile>
#include <QDir>
#include <QTextStream>

class SaveFile : public QObject
{
    Q_OBJECT
public:
    explicit SaveFile(QObject *parent = nullptr);
    void save();
    void saveData(const QString& filename,
                  const QMap<int, QVector3D>& commandCoords,
                  const QMap<int, QVector3D>& commandPoints,
                  const QMap<int, QVector3D>& comandActualPoints,
                  const QMap<int, QVector3D>& commandNormales,
                  const QMap<int, QVector3D>& commandCircleCenter,
                  const QMap<int, QVector3D>& commandPlace,
                  const QMap<int, float>& commandCircleRadius,
                  const QMap<int, float>& commandCircleActualRadius,
                  const QMap<int, QString>& commandPointName,
                  const QMap<int, QString>& commandPlaceName,
                  const QMap<int, QString>& commandCircleName,
                  const QMap<int, QString>& comentString,
                  const QMap<int, QVector3D>& commandMoveVector,
                  const QMap<int, QVector3D>& commandActualNormales,
                  const QMap<int, float>& pointErrors,
                  const QMap<int, float>& radiusErrors,
                  const QMap<int, QString>& errorName,
                  const QMap<int, bool>& pointChecker,
                  const QMap<int, bool>& placeChecker,
                  const QMap<int, bool>& circleChecker,
                  const QMap<int, bool>& moveChecker,
                  const QMap<int, bool>& pointByCircleChecker,
                  const QMap<int, bool>& pointErrorChecker,
                  const QMap<int, bool>& circleErrorCheker,
                  const QMap<int, bool>& pointByPlace,
                  const QMap<int, bool>& commentdBool,
                  const QMap<int, bool>& commentChecker,
                  const QMap<int, QVector<int>>& dependence);

    void saveVector3DMap(QJsonObject& root, const QString& key, const QMap<int, QVector3D>& map);
    void saveFloatMap(QJsonObject& root, const QString& key, const QMap<int, float>& map);
    void saveStringMap(QJsonObject& root, const QString& key, const QMap<int, QString>& map);
    void saveBoolMap(QJsonObject& root, const QString& key, const QMap<int, bool>& map);
    void saveVectorIntMap(QJsonObject& root, const QString& key, const QMap<int, QVector<int>>& map);

    void loadVector3DMap(const QJsonObject& root, const QString& key, QMap<int, QVector3D>& map);
    void loadFloatMap(const QJsonObject& root, const QString& key, QMap<int, float>& map);
    void loadStringMap(const QJsonObject& root, const QString& key, QMap<int, QString>& map);
    void loadBoolMap(const QJsonObject& root, const QString& key, QMap<int, bool>& map);
    void loadVectorIntMap(const QJsonObject& root, const QString& key, QMap<int, QVector<int>>& map);

    void loadData(const QString& filename,
             QMap<int, QVector3D>& m_commandCoords,
             QMap<int, QVector3D>& m_commandPoints,
             QMap<int, QVector3D>& m_comandActualPoints,
             QMap<int, QVector3D>& m_commandNormales,
             QMap<int, QVector3D>& m_commandCircleCenter,
             QMap<int, QVector3D>& m_commandPlace,
             QMap<int, float>& m_commandCircleRadius,
             QMap<int, float>& m_commandCircleActualRadius,
             QMap<int, QString>& m_commandPointName,
             QMap<int, QString>& m_commandPlaceName,
             QMap<int, QString>& m_commandCircleName,
             QMap<int, QString>& m_comentString,
             QMap<int, QVector3D>& m_commandMoveVector,
             QMap<int, QVector3D>& m_commandActualNormales,
             QMap<int, float>& m_pointErrors,
             QMap<int, float>& m_radiusErrors,
             QMap<int, QString>& m_errorName,
             QMap<int, bool>& m_pointChecker,
             QMap<int, bool>& m_placeChecker,
             QMap<int, bool>& m_circleChecker,
             QMap<int, bool>& m_moveChecker,
             QMap<int, bool>& m_pointByCircleChecker,
             QMap<int, bool>& m_pointErrorChecker,
             QMap<int, bool>& m_circleErrorCheker,
             QMap<int, bool>& m_pointByPlace,
             QMap<int, bool>& m_commentdBool,
             QMap<int, bool>& m_commentChecker,
             QMap<int, QVector<int>>& m_dependence);
signals:
};

#endif // SAVEFILE_H
