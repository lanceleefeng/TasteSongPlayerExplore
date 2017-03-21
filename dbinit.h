#ifndef DBINIT_H
#define DBINIT_H



class DBInit
{

public:
    DBInit();
    ~DBInit();

    //bool m_success;
    bool succeed();

    qint32 errorCode;
    QString errorMsg;

private:
    bool m_success = false;

};

#endif // DBINIT_H
