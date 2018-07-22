#include <QStandardPaths>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
//#include <QMessageBox>
#include <QDebug>

#include "config.h"
#include "tools.h"

QString Config::appName = "TasteSong Player";

//没有设置应用名称和公司名称时是AppLocalData目录
//"C:/Users/Administrator/AppData/Local/pid"
//QString Config::dataPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
QString Config::dataPath = "";

int Config::width = 590;
int Config::height = 310;
QString Config::windowConfigPath = "/windowConfig.xml";

Config::Config()
{
    
}


QMap<QString, QString> Config::windowConfig()
{
    QString filePath = Config::dataPath + windowConfigPath;
    //qDebug() << "window config: " << filePath;

    QMap<QString, QString> config;
    config = {
        {"windowType", "normal"},
        {"x", "center"},
        {"y", "200"},
        {"width", "500"},
        {"height", "300"}
    };

    QFile file(filePath);
    bool openRes = file.open(QFile::ReadOnly | QFile::Text);

    if(!openRes){
        qDebug() << QString("Error, cannot read file %1").arg(filePath);
        return config;
    }

    QXmlStreamReader reader;
    reader.setDevice(&file);


    while(!reader.atEnd())
    {
        if(reader.isStartElement()){
            //QString tagName = reader.name();
            //QString* tagName = reader.name();
            QString tagName = reader.name().toString();
            //qDebug() << tagName;

            if(config.contains(tagName)){
                config[tagName] = reader.readElementText();
            }

            //switch(tagName){} // 要求变量是int..

            //if(tagName == "windowType"){
            //    //config["windowType"] = reader.text().toString();
            //    config["windowType"] = reader.readElementText();
            //}else if(tagName == "x"){
            //
            //}
        }
        reader.readNext();
    }

    file.close();
    //Tools::pf(config);

    return config;
}


bool Config::saveWindowConfig(QMap<QString, QString> data)
{

    QString filePath = Config::dataPath + windowConfigPath;

    qDebug() << "window config: " << filePath;

    QFile file(filePath);
    bool openRes = file.open(QFile::WriteOnly | QFile::Text);
    if(!openRes){
        qDebug() << "Error: open file error: "
                 << qPrintable(file.errorString());
        return false;
    }

    QXmlStreamWriter xmlWriter(&file);

    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("windowConfig");

    //xmlWriter.writeTextElement("windowType", "normal");
    xmlWriter.writeTextElement("windowType", data["windowType"]);

    xmlWriter.writeStartElement("windowGeometry");
    xmlWriter.writeTextElement("x", data["x"]);
    xmlWriter.writeTextElement("y", data["y"]);
    xmlWriter.writeTextElement("width", data["width"]);
    xmlWriter.writeTextElement("height", data["height"]);
    xmlWriter.writeEndElement();

    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();

    file.close();

    if (file.error()) {
        qDebug() << "Error: Cannot write file: "
                 << qPrintable(file.errorString());
        return false;
    }
    return true;
}