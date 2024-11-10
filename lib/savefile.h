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
signals:
};

#endif // SAVEFILE_H
