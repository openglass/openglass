#ifndef OGTAGPAGE_H
#define OGTAGPAGE_H

#include <QtGui/QMainWindow>
#include <QtGui>
#include "qobject.h"

#include "ui_OgAdminWindow.h"
#include "NewTagDialog.h"
#include "ExistingTagDialog.h"
#include "TagDataset.h"
#include "TagTableModel.h"
#include "InputFile.h"

//middle tier header files
#include "common/DataSet.h"
#include "network/TCPConnection.h"
#include "network/TCPClient.h"
#include "common/TextSerializer.h"
#include "network/Packetizer.h"
#include "common/Logger.h"
#include "common/Command.h"
#include "OgClient.h"

/*
Implements Message Page in the right pane of GUI
Initializes the table widget
Implements SLOTS ans SIGNAL
*/

class OgTagPage : public QWidget
{
    Q_OBJECT

    public:
        OgTagPage(Ui::OgAdminwindow *, OgClient *, QWidget *parent = 0);
        ~OgTagPage();
        void initializeTable();             //Initial function called to set the table
        enum{
            TAGID       = 0,
            PROTOCOL    = 1,
            TAGNAME     = 2,
            DESCRIPTION = 3,
            LENGTH      = 4,
            TYPE        = 5,
            VALUE       = 6,
            GENERIC     = 7,
            NONGENERIC  = 8,
            BOTH        = 9
        };
        QString getIDFilter();              //Gets ID filter
        QString getProtocolFilter();        //Gets Protocol filter
        QString getTagFilter();             //Gets tag filter
        void    setIDFilter(QString);       //Sets ID filter
        void    setProtocolFilter(QString); //Sets Protocol filter
        void    setTagFilter(QString);      //Sets tag filter
        void    setIDs(QString, QString);   //Sets the ProtocolId and messageID for the page (when user selects tags for a particular protocol/message)
        int     getRowCount();              //Gets message table row count
        int     getColumnCount();           //Gets message table col count
        bool    getAllTagForProtocol(QString);
        bool    getAllTagForMessage(QString, QString);
        void    disableAddNewTagAction();
        void    enableAddNewTagAction();

        //Command pattern registered functions
        bool    getAllTagNotInProtocolForNewTagDialog(DataSet <TextSerializer> &);   //registered method for newTagDialog class
        bool    getAllTagInAllProtocolsForNewTagDialog(DataSet <TextSerializer> &);   //registered method for newTagDialog class

    private Q_SLOTS:
        void    newTag();                   //Shows new tag dialog box
        void    existingTag();              //Shows existing tag dialog box
        void    enteredImpl(const QModelIndex &index);      //Handles cell entered signal for tag table (tableview)
        void    cellClickedImpl(const QModelIndex);         //Handles cell clicked signal or tag table (tableview)
    private:
        bool    setTableData();             //Sets the table widgets and data
        void    createActions();            //Creates actions for the ADD menu
        void    createMenu();               //Creates the ADD menu
        void    updateTag(const QModelIndex);    //Brings up update tag dialog, when EDIT icon is clicked.
        void    deleteTag(const QModelIndex);    //Deletes the tag from the table, when DELETE icon is clicked.


        //Methods to create request to send to the database
        void    getTagRequest();                   //Creates get ALL tags request
        void    addTagRequest(TagDataset tagData, int);   //Creates add tag request, can be called from the
        void    editTagRequest(TagDataset tagData, int);  //Creates update tag request
        void    delTagRequest(QString, QString);          //Creates delete tag request
        void    getAllTagForProtocolRequest(QString);     //Creates get ALL tags for a particular protocol
        void    getAllTagForMessageRequest(QString, QString); //Creates get ALL tags for a particular message


        bool    getTagResponse(DataSet<TextSerializer> *);  //Handles the getTagRequest response
        bool    getTagResponse(QString);       //Handles the getTagRequest response, but copies the resultset to tagDataForNewDialog_
                                        //sends back this to newTagDialog class (gets the all tags except in the particular protocol)
        bool    getTagResponse();       //Handles the getTagRequest response, but copies the resultset to tagDataForNewDialog_
                                        //sends back this to newTagDialog class (gets all the tags in all the protocols)

        bool    getAllTagForProtocolResponse(DataSet<TextSerializer> *);    //Handles getAllTagProtocol response
        bool    getAllTagForMessageResponse(DataSet<TextSerializer> *);    //Handles getAllTagProtocol response
        bool    addTagResponse(DataSet<TextSerializer> *, QString&);
        bool    delTagResponse(DataSet<TextSerializer> *);
        bool    editTagResponse(DataSet<TextSerializer> *);


        Ui::OgAdminwindow*  pUi_;               //Ui Class reference
        QString             protocolID_;        //Protocol ID, needed when tags are displayed for a particular protocol
        QString             messageID_;          //Message ID, needed when tags are displayed for a particular message
        QLineEdit*          pLineID_;           //Filter line edit for ID column
        QLineEdit*          pLineProtocol_;     //Filter line edit for protocol column
        QLineEdit*          pLineTag_;          //Filter line edit for tag column
        QPushButton*        pRowButton_;        //Row button
        QMenu*              pAddMenu_;          //Menu linked to row button
        QAction*            pAddNewAction_;     //Add new menu action
        //QAction*            pAddExistingAction_;//Add existing menu action
        static int          tableChanged;       //set when edit is performed in the table

        TagDataset     dataSet_;               //Dataset class that hold the dta from tag db, used to set the data for requests
        QList <TagDataset> tagData_;            //The dataset list which is the container of data for the model layer
        QList <TagDataset> tagDataForNewDialog_; //The dataset list which is returned to newTagDialog to display in the combo box
        TagTableModel*  tagTableModel_;    //Model class for the UI protocol table

        OgClient*            client_;       //Client class that gets the connections right
        DataSet <TextSerializer>* dSet_;
};



#endif // OGTAGPAGE_H
