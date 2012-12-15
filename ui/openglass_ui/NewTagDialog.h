#ifndef NEWTAGDIALOG_H
#define NEWTAGDIALOG_H

/*
File name   :   NewTagDialog.h
Desc        :   NewTagDialog Class declaration
Author      :   Renjini
Version     :   1.0
*/


#include <QtGui/QMainWindow>
#include <QtGui>

#include "ui_NewTagDialog.h"
#include "TagDataset.h"

//middle tier header files
#include "common/DataSet.h"
#include "network/TCPConnection.h"
#include "network/TCPClient.h"
#include "common/TextSerializer.h"
#include "network/Packetizer.h"
#include "common/Logger.h"
#include "common/Command.h"
#include "OgClient.h"

class NewTagDialog : public QDialog, public Ui::NewTagDialog
{
    Q_OBJECT

    public:
    NewTagDialog(QWidget *parent);
    ~NewTagDialog();

    //void setTagData(QList <TagDataset>&);   //Set tagData_
    void    setGenericity(int);
    int     getGenericity();
    void    setTagComboBoxIndex(int);
    int     getTagComboBoxIndex();
    void    setProtocolID(QString);
    void    setMessageID(QString);
    bool    populateProtocolComboBox();
    bool    populateTagComboBox(QString);

    QString getProtocol();      //gets the protocol id from the protocol combobox when the user comes from the left pane
    QString getProtocolName();  //gets the protocol name
    QString getTagName();       //gets the tag name
    QString getTagDesc();       //gets the tag descr
    QString getDestDatatype();  //gets the destination tag datatype
    QString getTagType();       //gets the tag type (body/header)
    QString getTagLength();     //gets the tag length
    QString getJustifyWith();   //gets Justify direction or the tag (left/right)
    QString getTagEditable();   //gets whether the tag is editable or not  **** TODO change to  radio button in UI *****
    QString getTagReq();        //gets whether is required for the message
    QString getTagOffset();     //gets the tag offset in the message
    QString getTagSeq();        //gets the tag sequence number
    QString getTagDefValue();   //gets the default value for the tag

    void setProtocolName(QString);  //sets the protocol name
    void setTagName(QString);       //sets the tag name
    void setTagDesc(QString);       //sets the tag descr
    void setDestDatatype(QString);  //sets the destination tag datatype
    void setTagType(QString);       //sets the tag type (body/header)
    void setTagLength(QString);     //sets the tag length
    void setJustifyWith(QString);   //sets Justify direction or the tag (left/right)
    void setTagEditable(QString);   //sets whether the tag is editable or not
    void setTagReq(QString);        //sets whether is required for the message **** TODO change to  radio button in UI *****
    void setTagOffset(QString);     //sets the tag offset in the message
    void setTagSeq(QString);        //sets the tag sequence number
    void setTagDefValue(QString);   //sets the default value for the tag

    private Q_SLOTS:
    void toggleCreateButtonImpl(QString);   //enables create button only if message name is not null
    void selectionChanged(int);             //Handles the selection changes in the comboBox and sets the index in the member variable
    void activateTagComboBox(int);

    private:
    //QList <TagDataset> &tagData_;   //Mirrors OgTagPage tagData_
    DataSet <TextSerializer> dataSet_;
    int                         genericity; //Determines whether generic or nongeneric gruop boxes to be populated
    int                         tagComboBoxIndex; //Index of tag comboBox which the user selected
    bool                        needBothSection;  //Specifies whether Both section (generic and nongeneric) needs to be sent
                                                  //when user tries to add tag coming from the message page
    QList <ProtocolDataset>**   protocolData_;  //Holds the list of protocols to show in the protocol combo box
    QList <TagDataset>**        tagData_;       //Holds the list of tags to show in the tag combo box

    QString                     protocolID_; //Holds the protocolId if the user has comes through protocol page / message page
    QString                     messageID_;  //Holds the messageId if the user has comes through message page

    enum{
        GENERIC     = 7,
        NONGENERIC  = 8,
        BOTH        = 9
    };

};

#endif // NEWTAGDIALOG_H
