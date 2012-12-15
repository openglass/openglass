#ifndef TAGDATASET_H
#define TAGDATASET_H

/*
File name   :   TagDataset.h
Desc        :   TagDataset Class declaration
Author      :   Renjini
Version     :   1.0
*/

#include <QtGui>

/*
This class represent the tag db dataset to be displayed in the UI tag table
*/

class TagDataset
{
public:
    TagDataset();
    ~TagDataset();
    TagDataset(QString,QString,QString,QString,QString,QString,QString,QString,QString,int);
    QString getTagID();
    QString getProtocol();
    QString getProtocolId();
    QString getMessageId();
    QString getMessageName();
    QString getProtocolMsgTagId();
    QString getTagName();
    QString getTagDesc();
    QString getDestDatatype();//
    QString getTagType();
    QString getTagLength();
    QString getJustifyWith();//
    QString getTagEditable();//
    QString getTagReq();//
    QString getTagOffset();//
    QString getTagSeq();//
    QString getTagDefValue();
    int getColumnCount();
    void setTagID(QString);
    void setProtocol(QString);
    void setProtocolId(QString);
    void setMessageId(QString);
    void setMessageName(QString);
    void setProtocolMsgTagId(QString);
    void setTagName(QString);
    void setTagDesc(QString);
    void setTagLength(QString);
    void setTagType(QString);
    void setTagDefValue(QString);
    void setColumnCount(int);
    void setDestDatatype(QString);
    void setJustifyWith(QString);
    void setTagEditable(QString);
    void setTagReq(QString);
    void setTagSeq(QString);
    void setTagOffset(QString);
    void clearData();
private:
    QString protocol_;
    QString protocolId_;
    QString messageId_;
    QString messageName_;
    QString protocolMsgTagId_;
    QString tagId_;
    QString tagName_;
    QString tagDesc_;
    QString destDatatype_;
    QString tagType_;
    QString tagLen_;
    QString justifyWith_;
    bool    tagEditable_;
    bool    tagRequired_;
    QString tagOffset_;
    QString tagSeq_;
    QString tagDefValue_;
    //TODO: Possible values mechanism to be included
    int columnCount_;
    enum constants
    {
        COLUMN = 9
    };
};



#endif // TAGDATASET_H
