/*
File name   :   MessageDataset.cpp
Desc        :   MessageDataset Class declaration
Author      :   Renjini
Version     :   1.0
*/

#include "MessageDataset.h"



MessageDataset :: MessageDataset()
{
    messageId_      = "";
    messageName_    = "";
    messageDesc_    = "";
    protocol_       = "";
    protocolId_     = "";
    columnCount_    = COLUMN;
}

MessageDataset::MessageDataset(QString id, QString pId, QString pname, QString mname, QString desc, int count)
{
    messageId_      = id;
    messageName_    = mname;
    messageDesc_    = desc;
    protocol_       = pname;
    protocolId_     = pId;
    columnCount_    = count;
}

MessageDataset::~MessageDataset()
{

}

QString MessageDataset::getMessageName()
{
    return messageName_;
}

QString MessageDataset::getMessageID()
{
    return messageId_;
}

QString MessageDataset::getProtocol()
{
    return protocol_;
}

QString MessageDataset::getProtocolId()
{
    return protocolId_;
}

int MessageDataset::getColumnCount()
{
    return columnCount_;
}

QString MessageDataset::getMessageDesc()
{
    return messageDesc_;
}

void MessageDataset::setMessageID(QString id)
{
    messageId_ = id;
}

void MessageDataset::setMessageDesc(QString desc)
{
    messageDesc_ = desc;
}

void MessageDataset::setMessageName(QString name)
{
    messageName_ = name;
}

void MessageDataset::setProtocol(QString protocol)
{
    protocol_ = protocol;
}

void MessageDataset::setProtocolId(QString pId)
{
    protocolId_ = pId;
}

void MessageDataset::setColumnCount(int count)
{
    columnCount_ = count;
}

void MessageDataset::clearData()
{
    messageId_      = "";
    messageName_    = "";
    messageDesc_    = "";
    protocol_       = "";
    columnCount_    = COLUMN;
}


