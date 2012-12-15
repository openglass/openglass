/*
File name   :   OgMessagePage.h
Desc        :   OgMessagePage Class declaration
Author      :   Renjini
Version     :   1.0
*/

#ifndef OGMESSAGEPAGE_H
#define OGMESSAGEPAGE_H


#include <QtGui/QMainWindow>
#include <QtGui>
#include "qobject.h"

#include "ui_OgAdminWindow.h"
#include "NewMessageDialog.h"
#include "ExistingMessageDialog.h"
#include "MessageDataset.h"
#include "MessageTableModel.h"
#include "InputFile.h"
#include "OgClient.h"

//middle tier header files
#include "common/DataSet.h"
#include "network/TCPConnection.h"
#include "network/TCPClient.h"
#include "common/TextSerializer.h"
#include "network/Packetizer.h"
#include "common/Logger.h"
#include "common/Command.h"

/*
Implements Message Page in the right pane of GUI
Initializes the table widget
Implements SLOTS ans SIGNAL
*/

class OgMessagePage : public QWidget
{
    Q_OBJECT

    public:
        OgMessagePage(Ui::OgAdminwindow *, OgClient*, QWidget *parent = 0);
        ~OgMessagePage();
        void initializeTable();             //Initial function called to set the table
        enum{
            ID          = 0,
            PROTOCOL    = 1,
            MESSAGE     = 2,
            DESCRIPTION = 3
        };
        QString getIDFilter();              //Gets ID filter
        QString getProtocolFilter();        //Gets Protocol filter
        QString getMessageFilter();         //Gets Message filter
        void    setIDFilter(QString);       //Sets ID filter
        void    setProtocolFilter(QString); //Sets Protocol filter
        void    setProtocolID(QString);     //Sets the Protocol ID for the page (when user selects messages for a particular protocol
        void    setMessageFilter(QString);  //Sets Message filter
        //void    setMessageDataset(QList <MessageDataset> *);    //Sets the messagedataset list for message table model
        bool    getAllMessageForProtocol(QString);
        int     getRowCount();              //Gets message table row count
        int     getColumnCount();           //Gets message table col count
        bool    populateProtocolComboBox(DataSet <TextSerializer> &, QComboBox* ); //Populates

        QString cellClickedImpl(QModelIndex, QString&, QString&);   //Handles cellClicked signal for message table

    private Q_SLOTS:
        void    newMessage();                       //Shows new message dialog box
        void    existingMessage();                  //Shows existing message dialog box
        void    enteredImpl(const QModelIndex &index); //Handles cell entered signal for message table (tableview)

    private:
        bool    setTableData();             //Sets the table widgets and data
        void    createActions();            //Creates actions for the ADD menu
        void    createMenu();               //Creates the ADD menu
        void    updateMessage(const QModelIndex);    //Brings up update message dialog, when EDIT icon is clicked.
        void    deleteMessage(const QModelIndex);    //Deletes the message from the table, when DELETE icon is clicked.

        //Methods to create database requests
        void    getMessageRequest();                //Creates get ALL messages request
        void    addMessageRequest(MessageDataset messageData);  //Creates add message request
        void    editMessageRequest(MessageDataset messageData); //Creates edit message request
        void    delMessageRequest(QString);                    //Creates delete message request
        void    getAllMessageForProtocolRequest(QString);       //Creates get ALL messages for a particular protocol


        //Methods to handle database response
        bool    getMessageResponse(DataSet<TextSerializer> *);  //Handles the getMessageRequest response
        bool    addMessageResponse(DataSet<TextSerializer> *, QString&);  //Handles the addMessageRequest response
        bool    editMessageResponse(DataSet<TextSerializer> *); //Handles the editMessageRequest response
        bool    delMessageResponse(DataSet<TextSerializer> *); //Handles the delMessageRequest response
        bool    getAllMessageForProtocolResponse(DataSet<TextSerializer> *);


        Ui::OgAdminwindow*  pUi_;               //Ui Class reference
        QString             protocolID_;         //Protocol ID when messages are displayed for a particular protocol
        QLineEdit*          pLineID_;           //Filter line edit for ID column
        QLineEdit*          pLineProtocol_;     //Filter line edit for protocol column
        QLineEdit*          pLineMessage_;      //Filter line edit for message column
        QPushButton*        pRowButton_;        //Row button
        QMenu*              pAddMenu_;          //Menu linked to row button
        QAction*            pAddNewAction_;     //Add new menu action
        QAction*            pAddExistingAction_;//Add existing menu action
        static int          tableChanged;       //set when edit is performed in the table

        OgClient*            client_;
        DataSet <TextSerializer>* dSet_;
        QList <ProtocolDataset>** protocolData;

        MessageDataset     dataSet_;               //Dataset class that hold the dta from protocol db
        QList <MessageDataset> messageData_;
        MessageTableModel*  messageTableModel_;    //Model class for the UI protocol table
};


#endif // OGMESSAGEPAGE_H
