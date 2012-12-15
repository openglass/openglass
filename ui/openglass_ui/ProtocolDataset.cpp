/*
File name   :   ProtocolDataset.h
Desc        :   ProtocolDataset Class declaration
Author      :   Renjini
Version     :   1.0
*/

#include "ProtocolDataset.h"



ProtocolDataset :: ProtocolDataset()
{
    protocolId_     = "";
    protocolName_   = "";
    protocolVersion_= "";
    protocolDesc_   = "";
    protocolDetDesc_= "";
    protocolType_   = "";
    columnCount_    = COLUMN;
}

ProtocolDataset::ProtocolDataset(QString id, QString name, QString version, QString type, QString detDesc, QString desc, int count)
{
    protocolId_      = id;
    protocolName_    = name;
    protocolVersion_ = version;
    protocolDesc_    = desc;
    protocolDetDesc_ = detDesc;
    protocolType_    = type;
    columnCount_     = count;
}

ProtocolDataset::~ProtocolDataset()
{

}

QString ProtocolDataset::getProtocolName()
{
    return protocolName_;
}

QString ProtocolDataset::getProtocolID()
{
    return protocolId_;
}

QString ProtocolDataset::getProtocolVersion()
{
    return protocolVersion_;
}

QString ProtocolDataset::getProtocolDesc()
{
    return protocolDesc_;
}

QString ProtocolDataset::getProtocolDetDesc()
{
    return protocolDetDesc_;
}

QString ProtocolDataset::getProtocolType()
{
    return protocolType_;
}

int ProtocolDataset::getColumnCount()
{
    return columnCount_;
}

void ProtocolDataset::setProtocolID(QString id)
{
    protocolId_ = id;
}

void ProtocolDataset::setProtocolName(QString name)
{
    protocolName_ = name;
}

void ProtocolDataset::setProtocolVersion(QString version)
{
    protocolVersion_ = version;
}

void ProtocolDataset::setProtocolDesc(QString desc)
{
    protocolDesc_ = desc;
}

void ProtocolDataset::setProtocolDetDesc(QString detDesc)
{
    protocolDetDesc_ = detDesc;
}

void ProtocolDataset::setProtocolType(QString type)
{
    protocolType_ = type;
}

void ProtocolDataset::setColumnCount(int count)
{
    columnCount_ = count;
}

void ProtocolDataset::clearData()
{
    protocolId_         = "";
    protocolName_       = "";
    protocolVersion_    = "";
    protocolDesc_       = "";
    protocolType_       = "";
    protocolDetDesc_    = "";
    columnCount_        = COLUMN;
}
