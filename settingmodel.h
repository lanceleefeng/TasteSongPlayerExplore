#ifndef SETTINGMODEL_H
#define SETTINGMODEL_H

#include "basemodel.h"

class SettingModel : public BaseModel
{

public:
    SettingModel();

    QVariantMap getSetting();

    bool saveVolume(int volume);
    bool saveMode(int mode);

    bool settingExists(int uid);
    bool saveSetting(QVariantMap data);

    QString table = "settings";

};

#endif //SETTINGMODEL_H
