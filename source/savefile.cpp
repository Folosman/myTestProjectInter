#include "./lib/savefile.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVector3D>

SaveFile::SaveFile(QObject *parent)
    : QObject{parent}
{}
void SaveFile::saveVector3DMap(QJsonObject& root, const QString& key, const QMap<int, QVector3D>& map) {
    QJsonObject jsonMap;
    for (auto it = map.begin(); it != map.end(); ++it) {
        QJsonArray vectorArray = { it.value().x(), it.value().y(), it.value().z() };
        jsonMap[QString::number(it.key())] = vectorArray;
    }
    root[key] = jsonMap;
}

// Универсальная функция для сохранения QMap<int, float>
void SaveFile::saveFloatMap(QJsonObject& root, const QString& key, const QMap<int, float>& map) {
    QJsonObject jsonMap;
    for (auto it = map.begin(); it != map.end(); ++it) {
        jsonMap[QString::number(it.key())] = it.value();
    }
    root[key] = jsonMap;
}

// Универсальная функция для сохранения QMap<int, QString>
void SaveFile::saveStringMap(QJsonObject& root, const QString& key, const QMap<int, QString>& map) {
    QJsonObject jsonMap;
    for (auto it = map.begin(); it != map.end(); ++it) {
        jsonMap[QString::number(it.key())] = it.value();
    }
    root[key] = jsonMap;
}

// Универсальная функция для сохранения QMap<int, bool>
void SaveFile::saveBoolMap(QJsonObject& root, const QString& key, const QMap<int, bool>& map) {
    QJsonObject jsonMap;
    for (auto it = map.begin(); it != map.end(); ++it) {
        jsonMap[QString::number(it.key())] = it.value();
    }
    root[key] = jsonMap;
}

// Универсальная функция для сохранения QMap<int, QVector<int>>
void SaveFile::saveVectorIntMap(QJsonObject& root, const QString& key, const QMap<int, QVector<int>>& map) {
    QJsonObject jsonMap;
    for (auto it = map.begin(); it != map.end(); ++it) {
        QJsonArray vectorArray;
        for (int value : it.value()) {
            vectorArray.append(value);
        }
        jsonMap[QString::number(it.key())] = vectorArray;
    }
    root[key] = jsonMap;
}

void SaveFile::saveData(const QString& filename,
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
              const QMap<int, QVector<int>>& dependence) {
    QJsonObject root;

    // Сохраняем данные
    saveVector3DMap(root, "CommandCoords", commandCoords);
    saveVector3DMap(root, "CommandPoints", commandPoints);
    saveVector3DMap(root, "ComandActualPoints", comandActualPoints);
    saveVector3DMap(root, "CommandNormales", commandNormales);
    saveVector3DMap(root, "CommandCircleCenter", commandCircleCenter);
    saveVector3DMap(root, "CommandPlace", commandPlace);
    saveVector3DMap(root, "CommandMoveVector", commandMoveVector);
    saveVector3DMap(root, "CommandActualNormales", commandActualNormales);

    saveFloatMap(root, "CommandCircleRadius", commandCircleRadius);
    saveFloatMap(root, "CommandCircleActualRadius", commandCircleActualRadius);
    saveFloatMap(root, "PointErrors", pointErrors);
    saveFloatMap(root, "RadiusErrors", radiusErrors);

    saveStringMap(root, "CommandPointName", commandPointName);
    saveStringMap(root, "CommandPlaceName", commandPlaceName);
    saveStringMap(root, "CommandCircleName", commandCircleName);
    saveStringMap(root, "ComentString", comentString);
    saveStringMap(root, "ErrorName", errorName);

    saveBoolMap(root, "PointChecker", pointChecker);
    saveBoolMap(root, "PlaceChecker", placeChecker);
    saveBoolMap(root, "CircleChecker", circleChecker);
    saveBoolMap(root, "MoveChecker", moveChecker);
    saveBoolMap(root, "PointByCircleChecker", pointByCircleChecker);
    saveBoolMap(root, "PointErrorChecker", pointErrorChecker);
    saveBoolMap(root, "CircleErrorCheker", circleErrorCheker);
    saveBoolMap(root, "PointByPlace", pointByPlace);
    saveBoolMap(root, "CommentdBool", commentdBool);
    saveBoolMap(root, "CommentChecker", commentChecker);

    saveVectorIntMap(root, "Dependence", dependence);

    // Записываем в файл
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open file for saving.");
        return;
    }

    QJsonDocument doc(root);
    file.write(doc.toJson());
    file.close();
}

// Универсальная функция для загрузки QMap<int, QVector3D>
void SaveFile::loadVector3DMap(const QJsonObject& root, const QString& key, QMap<int, QVector3D>& map) {
    if (!root.contains(key)) return;

    QJsonObject jsonMap = root[key].toObject();
    for (auto it = jsonMap.begin(); it != jsonMap.end(); ++it) {
        QJsonArray vectorArray = it.value().toArray();
        if (vectorArray.size() == 3) {
            QVector3D vec(vectorArray[0].toDouble(),
                          vectorArray[1].toDouble(),
                          vectorArray[2].toDouble());
            map[it.key().toInt()] = vec;
        }
    }
}

// Универсальная функция для загрузки QMap<int, float>
void SaveFile::loadFloatMap(const QJsonObject& root, const QString& key, QMap<int, float>& map) {
    if (!root.contains(key)) return;

    QJsonObject jsonMap = root[key].toObject();
    for (auto it = jsonMap.begin(); it != jsonMap.end(); ++it) {
        map[it.key().toInt()] = it.value().toDouble();
    }
}

// Универсальная функция для загрузки QMap<int, QString>
void SaveFile::loadStringMap(const QJsonObject& root, const QString& key, QMap<int, QString>& map) {
    if (!root.contains(key)) return;

    QJsonObject jsonMap = root[key].toObject();
    for (auto it = jsonMap.begin(); it != jsonMap.end(); ++it) {
        map[it.key().toInt()] = it.value().toString();
    }
}

// Универсальная функция для загрузки QMap<int, bool>
void SaveFile::loadBoolMap(const QJsonObject& root, const QString& key, QMap<int, bool>& map) {
    if (!root.contains(key)) return;

    QJsonObject jsonMap = root[key].toObject();
    for (auto it = jsonMap.begin(); it != jsonMap.end(); ++it) {
        map[it.key().toInt()] = it.value().toBool();
    }
}

// Универсальная функция для загрузки QMap<int, QVector<int>>
void SaveFile::loadVectorIntMap(const QJsonObject& root, const QString& key, QMap<int, QVector<int>>& map) {
    if (!root.contains(key)) return;

    QJsonObject jsonMap = root[key].toObject();
    for (auto it = jsonMap.begin(); it != jsonMap.end(); ++it) {
        QJsonArray vectorArray = it.value().toArray();
        QVector<int> vec;
        for (const auto& value : vectorArray) {
            vec.append(value.toInt());
        }
        map[it.key().toInt()] = vec;
    }
}

// Главная функция для загрузки всех данных
void SaveFile::loadData(const QString& filename,
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
              QMap<int, QVector<int>>& m_dependence) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open file for loading.");
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject root = doc.object();

    // Загружаем данные
    loadVector3DMap(root, "CommandCoords", m_commandCoords);
    loadVector3DMap(root, "CommandPoints", m_commandPoints);
    loadVector3DMap(root, "ComandActualPoints", m_comandActualPoints);
    loadVector3DMap(root, "CommandNormales", m_commandNormales);
    loadVector3DMap(root, "CommandCircleCenter", m_commandCircleCenter);
    loadVector3DMap(root, "CommandPlace", m_commandPlace);
    loadVector3DMap(root, "CommandMoveVector", m_commandMoveVector);
    loadVector3DMap(root, "CommandActualNormales", m_commandActualNormales);

    loadFloatMap(root, "CommandCircleRadius", m_commandCircleRadius);
    loadFloatMap(root, "CommandCircleActualRadius", m_commandCircleActualRadius);
    loadFloatMap(root, "PointErrors", m_pointErrors);
    loadFloatMap(root, "RadiusErrors", m_radiusErrors);

    loadStringMap(root, "CommandPointName", m_commandPointName);
    loadStringMap(root, "CommandPlaceName", m_commandPlaceName);
    loadStringMap(root, "CommandCircleName", m_commandCircleName);
    loadStringMap(root, "ComentString", m_comentString);
    loadStringMap(root, "ErrorName", m_errorName);

    loadBoolMap(root, "PointChecker", m_pointChecker);
    loadBoolMap(root, "PlaceChecker", m_placeChecker);
    loadBoolMap(root, "CircleChecker", m_circleChecker);
    loadBoolMap(root, "MoveChecker", m_moveChecker);
    loadBoolMap(root, "PointByCircleChecker", m_pointByCircleChecker);
    loadBoolMap(root, "PointErrorChecker", m_pointErrorChecker);
    loadBoolMap(root, "CircleErrorCheker", m_circleErrorCheker);
    loadBoolMap(root, "PointByPlace", m_pointByPlace);
    loadBoolMap(root, "CommentdBool", m_commentdBool);
    loadBoolMap(root, "CommentChecker", m_commentChecker);

    loadVectorIntMap(root, "Dependence", m_dependence);
}


void SaveFile::save()
{
    QFile file("./save/save.txt");
    QString defaultDir = "./save";
    QDir dir(defaultDir);

    dir.mkdir(defaultDir);

    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);

    }

}
