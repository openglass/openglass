#ifndef MESSAGEDATASET_H
#define MESSAGEDATASET_H

/*
File name   :   MessageDataset.h
Desc        :   MessageDataset Class declaration
Author      :   Renjini
Version     :   1.0
*/

#include <QtGui>

/*
This class represent the message db dataset to be displayed in the UI message table
*/

class MessageDataset
{
public:
    MessageDataset();
    ~MessageDataset();
    MessageDataset(QString,QString,QString,QString,QString,int);
    QString getMessageName();
    QString getMessageID();
    QString getMessageDesc();
    QString getProtocol();
    QString getProtocolId();
    int getColumnCount();
    void setMessageID(QString);
    void setMessageName(QString);
    void setMessageDesc(QString);
    void setProtocol(QString);
    void setProtocolId(QString);
    void setColumnCount(int);
    void clearData();
private:
    QString protocolId_;
    QString messageId_;
    QString messageName_;
    QString messageDesc_;
    QString protocol_;
    int columnCount_;
    enum constants
    {
        COLUMN  =   7
    };
};


#endif // MESSAGEDATASET_H
