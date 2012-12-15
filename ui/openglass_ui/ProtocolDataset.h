#ifndef PROTOCOLDATASET_H
#define PROTOCOLDATASET_H

/*
File name   :   ProtocolDataset.h
Desc        :   ProtocolDataset Class declaration
Author      :   Renjini
Version     :   1.0
*/

#include <QtGui>

/*
This class represent the protocol table dataset to be displayed in the table
*/

class ProtocolDataset
{
public:
    ProtocolDataset();
    ~ProtocolDataset();
    ProtocolDataset(QString,QString,QString,QString,QString,QString,int);
    QString getProtocolName();
    QString getProtocolID();
    QString getProtocolVersion();
    QString getProtocolType();
    QString getProtocolDesc();
    QString getProtocolDetDesc();
    int getColumnCount();
    void setProtocolID(QString);
    void setProtocolName(QString);
    void setProtocolVersion(QString);
    void setProtocolType(QString);
    void setColumnCount(int);
    void setProtocolDesc(QString);
    void setProtocolDetDesc(QString);
    void clearData();
private:
    QString protocolId_;
    QString protocolName_;
    QString protocolVersion_;
    QString protocolType_;
    QString protocolDesc_;
    QString protocolDetDesc_;

    int columnCount_;
    enum constants
    {
        COLUMN  =   7
    };
};


#endif  //PROTOCOLDATASET_H
