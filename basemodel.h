//
// Created by Administrator on 2017-3-14.
//

#ifndef BASEMODEL_H
#define BASEMODEL_H

#include "db.h"

class BaseModel
{
    //Q_OBJECT
public:

    BaseModel();

    // 使用了单例模式，只能直接用DB::instance()获取唯一实例直接使用，不能赋值给类的属性..
    //static DB& db;
    //static DB& db = DB::instance();

    int uid = 1; // 会员ID，保存设置时作为主键用，实际上并没有登录、注册

};


#endif //BASEMODEL_H
