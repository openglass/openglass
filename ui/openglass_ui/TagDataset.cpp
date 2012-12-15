/*
File name   :   TagDataset.cpp
Desc        :   TagDataset Class declaration
Author      :   Renjini
Version     :   1.0
*/

#include "TagDataset.h"



TagDataset :: TagDataset()
{
    tagId_          = "";
    tagName_        = "";
    tagDesc_        = "";
    protocol_       = "";
    protocolId_     = "";
    messageId_      = "";
    messageName_    = "";
    tagLen_         = "";
    tagType_        = "";
    destDatatype_   = "";
    justifyWith_    = "";
    tagEditable_    = "";
    tagRequired_    = "";
    tagOffset_      = "";
    tagSeq_         = "";
    tagDefValue_    = "";
    protocolMsgTagId_ = "";
    columnCount_= COLUMN;
}

TagDataset::TagDataset(QString id, QString pId, QString mId, QString pname, QString tname, QString desc,
                       QString len, QString type, QString val, int count)
{
    tagId_      = id;
    tagName_    = tname;
    tagDesc_    = desc;
    protocol_   = pname;
    protocolId_ = pId;
    messageId_  = mId;
    tagLen_     = len;
    tagType_    = type;
    tagDefValue_   = val;
    columnCount_= count;
}

TagDataset::~TagDataset()
{

}

QString TagDataset::getTagName()
{
    return tagName_;
}

QString TagDataset::getTagID()
{
    return tagId_;
}

QString TagDataset::getProtocol()
{
    return protocol_;
}

QString TagDataset::getProtocolId()
{
    return protocolId_;
}

QString TagDataset::getMessageId()
{
    return messageId_;
}

QString TagDataset :: getMessageName()
{
    return messageName_;
}

QString TagDataset::getProtocolMsgTagId()
{
    return protocolMsgTagId_;
}

int TagDataset::getColumnCount()
{
    return columnCount_;
}

QString TagDataset::getTagDesc()
{
    return tagDesc_;
}

QString TagDataset::getTagLength()
{
    return tagLen_;
}

QString TagDataset::getTagType()
{
    return tagType_;
}

QString TagDataset::getTagDefValue()
{
    return tagDefValue_;
}

QString TagDataset::getDestDatatype()
{
    return destDatatype_;
}

QString TagDataset::getJustifyWith()
{
    return justifyWith_;
}

QString TagDataset::getTagEditable()
{
    QString editable;

    if(tagEditable_)
        editable = "Y";
    else
        editable = "N";

    return editable;
}

QString TagDataset::getTagReq()
{
    QString req;

    if(tagRequired_)
        req = "Y";
    else
        req = "N";

    return req;
}

QString TagDataset::getTagSeq()
{
    return tagSeq_;
}

QString TagDataset::getTagOffset()
{
    return tagOffset_;
}

void TagDataset::setTagID(QString id)
{
    tagId_ = id;
}

void TagDataset::setTagName(QString name)
{
    tagName_ = name;
}

void TagDataset::setTagDesc(QString desc)
{
    tagDesc_ = desc;
}

void TagDataset::setTagLength(QString length)
{
    tagLen_ = length;
}

void TagDataset::setTagType(QString type)
{
    tagType_ = type;
}

void TagDataset::setTagDefValue(QString value)
{
    tagDefValue_ = value;
}

void TagDataset::setProtocol(QString protocol)
{
    protocol_ = protocol;
}

void TagDataset::setProtocolId(QString pId)
{
    protocolId_ = pId;
}

void TagDataset::setMessageId(QString mId)
{
    messageId_ = mId;
}

void TagDataset :: setMessageName(QString name)
{
    messageName_ = name;
}

void TagDataset::setDestDatatype(QString datatype)
{
    destDatatype_ = datatype;
}

void TagDataset::setJustifyWith(QString justify)
{
    justifyWith_ = justify;
}

void TagDataset::setTagEditable(QString editable)
{
    if( 0 == editable.compare("Y", Qt::CaseInsensitive))
        tagEditable_ = true;
    else if( 0 == editable.compare("N", Qt::CaseInsensitive))
        tagEditable_ = false;
}

void TagDataset::setTagReq(QString req)
{
    if(0 == req.compare("Y", Qt::CaseInsensitive) )
        tagRequired_ = true;
    else if( 0 == req.compare("N", Qt::CaseInsensitive))
        tagRequired_ = false;
    else
        tagRequired_ = false;
}

void TagDataset::setTagSeq(QString seq)
{
    tagSeq_ = seq;
}

void TagDataset::setTagOffset(QString offset)
{
    tagOffset_ = offset;
}

void TagDataset::setColumnCount(int count)
{
    columnCount_ = count;
}

void TagDataset::setProtocolMsgTagId(QString pMTId)
{
    protocolMsgTagId_ = pMTId;
}

void TagDataset::clearData()
{
    tagId_          = "";
    tagName_        = "";
    tagDesc_        = "";
    protocol_       = "";
    protocolId_     = "";
    messageId_      = "";
    messageName_    = "";
    tagLen_         = "";
    tagType_        = "";
    destDatatype_   = "";
    justifyWith_    = "";
    tagEditable_    = "";
    tagRequired_    = "";
    tagOffset_      = "";
    tagSeq_         = "";
    tagDefValue_    = "";
    protocolMsgTagId_ = "";
    columnCount_= COLUMN;
}



