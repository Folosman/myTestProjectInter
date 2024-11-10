#include "./lib/savefile.h"

SaveFile::SaveFile(QObject *parent)
    : QObject{parent}
{}


void SaveFile::save()
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

    }

}
