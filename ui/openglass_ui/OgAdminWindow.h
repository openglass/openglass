#ifndef OGADMINWINDOW_H
#define OGADMINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui>


#include "ui_OgAdminWindow.h"
#include "OgProtocolPage.h"
#include "OgMessagePage.h"
#include "MessageDataset.h"
#include "OgTagPage.h"
#include "OgClient.h"

//middle tier header files
#include "common/DataSet.h"
#include "network/TCPConnection.h"
#include "network/TCPClient.h"
#include "common/TextSerializer.h"
#include "network/Packetizer.h"
#include "common/Logger.h"


//Qt Form Class
class ogAdminWindow : public QMainWindow
{
    Q_OBJECT

public Q_SLOTS:
    void protocolButtonPressed();   //Slot for clicked signal from protocolButton
    void messageButtonPressed();    //Slot for clicked signal from messageButton
    void tagButtonPressed();        //Slot for clicked signal from tagButton

    void initializeClient();
public:
    ogAdminWindow(QWidget *parent = 0);
    ~ogAdminWindow();
    enum{
        PROTOCOL    = 0,
        MESSAGE     = 1,
        TAG         = 2
    };

private Q_SLOTS:
    void protocolTableClicked(QModelIndex); //Handles clicked signal for protocolTable (tableview)
    void messageTableClicked(QModelIndex);  //Handles clicked signal for messageTable (tableview)

private:
    Ui::OgAdminwindow* pUi_;        //Reference of main window UI
    OgProtocolPage* protocolPage;   //Reference of protocol page class
    OgMessagePage*  messagePage;    //REference of message page class
    OgTagPage*      tagPage;        //Reference of tag page class
    OgClient*       client_;

};

#endif // OGADMINWINDOW_H
