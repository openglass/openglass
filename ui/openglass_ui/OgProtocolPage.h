/*
File name   :   OgProtocolPage.h
Desc        :   OgProtocolPage Class declaration
Author      :   Renjini
Version     :   1.0
*/

#ifndef OGPROTOCOLPAGE_H
#define OGPROTOCOLPAGE_H

#include <QtGui/QMainWindow>
#include <QtGui>
#include <QThread>
#include "qobject.h"

#include "ui_OgAdminWindow.h"
#include "NewProtocolDialog.h"
#include "ProtocolDataset.h"
#include "MessageDataset.h"
#include "ProtocolTableModel.h"
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
Implements Protocol Page in the right pane of GUI
Initializes the table widget
Implements SLOTS ans SIGNAL
*/

class ProtocolFilterProxy : public QSortFilterProxyModel
{
protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
};

class OgProtocolPage : public QWidget
{
    Q_OBJECT

    public:
        OgProtocolPage(Ui::OgAdminwindow *, OgClient*, QWidget *parent = 0);
        ~OgProtocolPage();

        enum col{
            ID          = 0,
            PROTOCOL    = 1,
            VERSION     = 2,
            PROTOCOLTYPE= 7
        };

        void initializeTable();                             //Initial function called to set the table and connections

        QString getIDFilter();                              //Gets ID filter
        QString getProtocolFilter();                        //Gets Protocol filter
        void    setIDFilter(QString);                       //Sets ID filter
        void    setProtocolFilter(QString);                 //Sets Protocol filter
        int     getRowCount();                              //Gets protocol table row count
        int     getColumnCount();                           //Gets protocol table col count
        void    getAllProtocols(DataSet <TextSerializer> &dSet);    //Gives the list of protocols to the message page


        QString cellClickedImpl(QModelIndex, QString& /*,QList <MessageDataset>**/);    //Handles cellClicked signal for protocol table (tableview)



    private Q_SLOTS:
        void    newProtocol();                              //Shows new protocol dialog box
        void enteredImpl(const QModelIndex &index);         //Handles cell entered signal for protocol table (tableview)
        void pLineIDTextChanged(QString);                   //LineEdit textChanged signal handling slot
        void pLineProtocolTextChanged(QString);             //LineEdit textChanged signal handling slot

        //Methods for handling database response
        bool getProtocolResponse(DataSet<TextSerializer> *);
        bool addProtocolResponse(DataSet<TextSerializer> *, int&); ///////change this
        bool editProtocolResponse(DataSet<TextSerializer> *);
        bool delProtocolResponse(DataSet<TextSerializer> *);

    private:
        bool    setTableData();                 //Sets table data
        void    createActions();                //Creates actions for ADD menu
        void    createMenu();                   //Creates ADD menu
        void    updateProtocol(const QModelIndex);    //Brings up update protocol dialog, when EDIT icon is clicked.
        void    deleteProtocol(const QModelIndex);    //Deletes the protocol from the table, when DELETE icon is clicked.

        void    updateProtocolList(std::string);    //Updates the protocol dataset list after the database req-resp

        //Methods for database request creation

        bool    getProtocolRequest();                           //Creates get ALL protocols request
        void    addProtocolRequest(ProtocolDataset protocolData);   //Creates add protocol request
        void    editProtocolRequest(ProtocolDataset protocolData);  //Creates update protocol request
        void    delProtocolRequest(QString);                        //Creates delete protocol request

        Ui::OgAdminwindow*  pUi_;               //Ui Class reference
        QLineEdit*          pLineProtocol_;     //Filter line edit for protocol column
        QLineEdit*          pLineID_;           //Filter line edit for ID column
        QPushButton*        pRowButton_;        //Row button
        QMenu*              pAddMenu_;          //Menu linked to row button
        QAction*            pAddNewAction_;     //Add new menu action

        OgClient*            client_;
        DataSet <TextSerializer>* dSet_;

        ProtocolDataset     dataSet_;               //Dataset class that hold the dta from protocol db
        QList <ProtocolDataset> protocolData_;
        ProtocolTableModel*  protocolTableModel_;    //Model class for the UI protocol table
        QSortFilterProxyModel* protocolProxyModel_;     //Proxy modelfor filtering data in the tble
        ProtocolFilterProxy protocolProxy_;
        static int          tableChanged;       //set when edit is performed in the table
};

#endif // OGPROTOCOLPAGE_H
