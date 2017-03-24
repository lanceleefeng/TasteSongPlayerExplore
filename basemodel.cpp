
#include "basemodel.h"

//虽然代码中没有使用，但头文件引入了，会影响
//头文件中也去掉，造成编译报错，两个地方都加上却能通过，一颗赛艇.
//2017-3-22 22:04:32
#include "db.h"

BaseModel::BaseModel()
{
    //static DB& db = DB::instance("music");
    
    //不能这样用 
    //db = DB::instance("music");

}
