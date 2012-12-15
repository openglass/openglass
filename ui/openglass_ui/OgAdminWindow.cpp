#include "OgAdminWindow.h"

ogAdminWindow::ogAdminWindow(QWidget *parent)
    : QMainWindow(parent)
{
    pUi_                = new Ui::OgAdminwindow();
    client_             = new OgClient();
    protocolPage        = new OgProtocolPage(pUi_, client_);
    messagePage         = new OgMessagePage(pUi_, client_);
    tagPage             = new OgTagPage(pUi_, client_);

    pUi_->setupUi(this);

    //Initialize the client to server connection
    initializeClient();

    sleep(2);

    //Initialize the protocol, message and tag pages, protocol is the default window that is being shown to the user

    tagPage->initializeTable();
    messagePage->initializeTable();
    protocolPage->initializeTable();

    //Connecting left pane buttons to right pane tables
    connect(pUi_->protocolButton, SIGNAL(clicked()), this, SLOT(protocolButtonPressed()));
    connect(pUi_->messageButton, SIGNAL(clicked()), this, SLOT(messageButtonPressed()));
    connect(pUi_->tagButton, SIGNAL(clicked()), this, SLOT(tagButtonPressed()));

    //Connecting protocol table clicked signal
    connect(pUi_->protocolTable, SIGNAL(clicked(QModelIndex)), this, SLOT(protocolTableClicked(QModelIndex)));
    //Connecting message table clicked signal
    connect(pUi_->messageTable, SIGNAL(clicked(QModelIndex)), this, SLOT(messageTableClicked(QModelIndex)));
}

ogAdminWindow::~ogAdminWindow()
{
    delete pUi_;
    delete protocolPage;
    delete messagePage;
    delete tagPage;
}

void ogAdminWindow::initializeClient()
{

    QMessageBox msg;
    QFile file("/appl/framework/network/unittest/test_config");
    if(! file.open(QIODevice::ReadOnly))
        msg.information(this,"file", "File not opened");

    char text[1024];
    file.read(text,1024);

    DataSet<TextSerializer> data;
    data.deserialize(text);

    client_->initialize(data);

    client_->run();

    file.close();

}


//Handles Protocol Button press
void ogAdminWindow::protocolButtonPressed()
{

    //pUi_->protocolButton->setStyleSheet(":pressed{color:blue;font:bold 20px;}");
    protocolPage->initializeTable();

}

//Handles Message Button press
void ogAdminWindow::messageButtonPressed()
{
    messagePage->initializeTable();
}

//Handles Tag Button press
void ogAdminWindow::tagButtonPressed()
{
    tagPage->initializeTable();
}

//Handles cellClicked signal for protocolTable.
//Calls protocolPage public function to return the protocol selected
//Sets messagePage protocol filter with the protocol string returned

void ogAdminWindow :: protocolTableClicked(QModelIndex index)
{
    QString protocolName;
    QString protocolID;
    QString messageID = "";
    //QList <MessageDataset>* mDataset = new QList<MessageDataset>;

    protocolName = protocolPage->cellClickedImpl(index, protocolID/*,mDataset*/);
    int stackedIndex = pUi_->rightPaneStackedWidget->currentIndex();

    /*
      When user clicks the "messages" or "tags" link the currentIndex of the stacked widget will change with abv cellClickedImpl function.
      Set the corresponding lineEdits only when the indexes are changed
    */
    if(stackedIndex == MESSAGE)
    {
        messagePage->setProtocolFilter(protocolName);
        messagePage->setProtocolID(protocolID);
        messagePage->getAllMessageForProtocol(protocolID);
    }
    else if(stackedIndex == TAG)
    {
        tagPage->setProtocolFilter(protocolName);
        tagPage->setIDs(protocolID, messageID);
        //tagPage->enableAddNewTagAction();
        tagPage->getAllTagForProtocol(protocolID);
    }

   // delete(mDataset);
}

void ogAdminWindow :: messageTableClicked(QModelIndex index)
{
    QString messageName;
    QString protocolId;
    QString messageId;

    messageName = messagePage->cellClickedImpl(index, protocolId, messageId);
    int stackedIndex = pUi_->rightPaneStackedWidget->currentIndex();

    /*
      When user clicks the "messages" or "tags" link the currentIndex of the stacked widget will change with abv cellClickedImpl function.
      Set the corresponding lineEdits only when the indexes are changed
    */
    if(stackedIndex == TAG)
    {
        tagPage->setProtocolFilter(messageName);
        tagPage->setIDs(protocolId, messageId);
        LOG_DEBUG("ProtocolIDDDDD = ");
        //LOG_DEBUG(protocolId);
        //tagPage->disableAddNewTagAction();
        tagPage->getAllTagForMessage(protocolId, messageId);
    }
}
