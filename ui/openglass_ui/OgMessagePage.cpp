    /*
File name   :   OgMessagePage.cpp
Desc        :   OgMessagePage Class description
Author      :   Renjini
Version     :   1.0
*/


#include "OgMessagePage.h"
//#include "logger.h"


int OgMessagePage::tableChanged=1;

OgMessagePage :: OgMessagePage(Ui::OgAdminwindow* pUiArg, OgClient* client, QWidget * parent) : QWidget(parent), pUi_(pUiArg), client_(client)
{
    pLineID_            = new QLineEdit(this);
    pLineProtocol_      = new QLineEdit(this);
    pLineMessage_       = new QLineEdit(this);
    pRowButton_         = new QPushButton();
    messageTableModel_  = new MessageTableModel(messageData_, this);

    std::string request = "REQUEST";
    dSet_               = new DataSet <TextSerializer> (request);

    pRowButton_->setFixedSize(55,30);
    pRowButton_->setIcon(QIcon("://images//edit_add.ico"));
    pRowButton_->setPalette(QPalette(Qt::darkGray ));

    createActions();
    createMenu();
}

OgMessagePage :: ~OgMessagePage()
{
    if(pRowButton_)
        delete pRowButton_;
    if(pAddMenu_)
        delete pAddMenu_;
    if(pAddNewAction_)
        delete pAddNewAction_;
    if(pAddExistingAction_)
        delete pAddExistingAction_;
}

void OgMessagePage :: createActions()
{
    pAddNewAction_ = new QAction(QWidget::tr("&Add New"),this);
    //pAddNewAction_->setIcon();
    pAddNewAction_->setToolTip(QObject::tr("Add new message"));
    pAddNewAction_->setShortcut(QObject::tr("Ctrl+N"));
    QObject::connect(pAddNewAction_, SIGNAL(triggered(bool)), this, SLOT(newMessage()) );

    pAddExistingAction_ = new QAction(QWidget::tr("&Add Existing..."),this);
    //pAddExistingAction_->setIcon();
    pAddExistingAction_->setToolTip(QObject::tr("Add existing message"));
    pAddExistingAction_->setShortcut(QObject::tr("Ctrl+E"));
    QObject::connect(pAddExistingAction_, SIGNAL(triggered()), this, SLOT(existingMessage()) );

}

void OgMessagePage :: createMenu()
{
    pAddMenu_           = new QMenu();
    pAddMenu_->addAction(pAddNewAction_);
    pAddMenu_->addAction(pAddExistingAction_);
    pRowButton_->setMenu(pAddMenu_);
}

/*
  Reads the messages.txt file and populates the protocol table model.
  This model is then shown to user in the UI protocol table.
*/

bool OgMessagePage::setTableData()
{
    protocolID_.clear();        //User has not selected any protocol

    //Open the file
    InputFile messageFile("//appl//openglass//ui//openglass_ui//inputfiles//messages.txt");
    if(! messageFile.open() )
        return false;

    //Populate the header labels in the model
    QStringList headerLabel = messageFile.getHeaderLabel();
    messageTableModel_->setMessageTableHeader(headerLabel);

    dSet_->clear();

    //Fetch the request
    getMessageRequest();
    //Set the request to the client class
    client_->setRequest(dSet_);
    if ( ! client_->sendRequest())
    {
        QMessageBox msg;
        msg.information(this, "Connection Error", "Server not connected");
        return false;
    }

    //The response is populated in the variable of class OgClient, fetch it
    if (! getMessageResponse(client_->getRequest()))
    {
        QMessageBox msg;
        msg.information(this, "Error", "Error occured while fetching data");
        return false;
    }

    messageFile.close();
    return true;
}

void OgMessagePage :: initializeTable()
{

    bool result;
    QString rightPaneLabel;
    QString protocolName;

    protocolID_.clear();        //User has not selected any protocol

    if(pUi_->rightPaneStackedWidget->isHidden())
        pUi_->rightPaneStackedWidget->show();

    //Setting right pane label
    //Check the protocol filter before updating
    //protocolName = pLineProtocol_->text();
    /*
    if(! protocolName.isEmpty())
    {
        rightPaneLabel.append(QObject::tr(" Messages"));
        rightPaneLabel.prepend(protocolName);
    }
    else
    {
        rightPaneLabel.append(QObject::tr("All Messages"));
    }
    */

    rightPaneLabel.append(QObject::tr("All Messages"));
    pUi_->rightPaneLabel->setText(rightPaneLabel);
    pUi_->rightPaneStackedWidget->setCurrentIndex(1);

    //Populate data in the model
    setTableData();

    if(tableChanged == 1)
    {
        tableChanged = 0;

        QModelIndex index;
        //messageTableModel_->setMessageDataset(messageData_);
        pUi_->messageTable->setModel(messageTableModel_);

        //Set the LineEdits in the first row
        index = messageTableModel_->index(0, 0);
        pUi_->messageTable->setIndexWidget(index, pLineID_);
        index = messageTableModel_->index(0, 1);
        pUi_->messageTable->setIndexWidget(index, pLineProtocol_);
        index = messageTableModel_->index(0, 2);
        pUi_->messageTable->setIndexWidget(index, pLineMessage_);

        //Set the row button
        int row = messageTableModel_->rowCounter();
        index = messageTableModel_->index(row-1, 0);
        pUi_->messageTable->setIndexWidget(index, pRowButton_);

    }

    //Clear the lineEdits
    pLineID_->clear();
    pLineProtocol_->clear();
    pLineMessage_->clear();

    //Connect the signals to the slots

    //Handle enter SIGNAL for message table
    connect(pUi_->messageTable, SIGNAL(entered(QModelIndex)), this, SLOT(enteredImpl(QModelIndex)));

}

bool OgMessagePage :: populateProtocolComboBox(DataSet <TextSerializer> &dataSet, QComboBox *protocolCombo)
{
    //Call the registered service of OgProtocolPage class to get the list of protocols


    Fog::Common::CommandRouter::getInstance()->invokeMethod("AllProtocols", dataSet);

    protocolData = dataSet.get<QList <ProtocolDataset> *>("REQUEST", "PROTOCOLDATASET");

    //Populate the protocol combobox
    if(! protocolData)
        return false;

    int rowCount = (*protocolData)->size();
    ProtocolDataset data;

    //In protocolDataset list 0th and last element is blank (omit those)
    for(int row = 1; row < rowCount-1; row++)
    {
        data.clearData();
        data = (*protocolData)->value(row);
        //The combo box will not have those blank fields, the 0th element will not be blank, hence (row-1)
        protocolCombo->addItem(data.getProtocolName());
    }
    return true;
}

void OgMessagePage :: newMessage()
{
    NewMessageDialog newMessageDialog(this);    
    DataSet <TextSerializer> dataSet("REQUEST");
    QString protocolName;

    //Populate the protocol comboBox
    if(! populateProtocolComboBox(dataSet, newMessageDialog.protocolComboBox))
    {
        QMessageBox msg;
        msg.information(this, "Error", "Service not available");
        newMessageDialog.close();
    }

    //When user comes through the protocols page, we have the protocol ID
    if(! protocolID_.isEmpty())
    {
        //Get the protocol name for the corresponding protocol ID to display in the UI (after addition) and the combo box

        int rowCount = (*protocolData)->size();

        for(int row = 1; row < rowCount - 1; row++)
        {
            if( 0 == protocolID_.compare( (*protocolData)->value(row).getProtocolID() ) )
            {
                protocolName = (*protocolData)->value(row).getProtocolName();
                newMessageDialog.protocolComboBox->setCurrentIndex(row - 1);
            }
        }

        newMessageDialog.protocolComboBox->setDisabled(true);
    }
    else
        newMessageDialog.protocolComboBox->setEnabled(true);


    if(newMessageDialog.exec())
    {
        QModelIndex index;
        int rowCount = ( messageTableModel_->rowCounter() - 1 );

        //Get the message details from the dialog box
        //Assign it in the message dataset and send it across the network module

        dataSet_.clearData();
        //For new message, ID will be assigned in the database (auto-increment)

        if(! protocolID_.isEmpty() )
        {
            dataSet_.setProtocolId(protocolID_);        //User has selected messages for a particular protocol
            dataSet_.setProtocol(protocolName);
        }
        else
        {
            int comboRow = newMessageDialog.protocolComboBox->currentIndex();
            dataSet_.setProtocolId((*protocolData)->value(comboRow+1).getProtocolID());
            dataSet_.setProtocol(newMessageDialog.getProtocolName());
        }

        dataSet_.setMessageName(newMessageDialog.getMessageName());
        dataSet_.setMessageDesc(newMessageDialog.getMessageDesc());

        //Create the add message req and send it to the database
        //TODO: Handle the error condition here

        dSet_->clear();
        addMessageRequest(dataSet_);
        client_->setRequest(dSet_);
        client_->sendRequest();

        //The response is populated in the variable of class OgClient, fetch it
        QString messageId;
        if (! addMessageResponse(client_->getRequest(), messageId))
        {
            QMessageBox msg;
            msg.information(this, "Error", "Error occured while adding data");

            return;
        }

        messageTableModel_->insertRows(rowCount, 1, QModelIndex());

        index = messageTableModel_->index(rowCount, 0, QModelIndex());
        messageTableModel_->setData(index, messageId, Qt::EditRole);
        index = messageTableModel_->index(rowCount, 1, QModelIndex());
        messageTableModel_->setData(index, dataSet_.getProtocol(), Qt::EditRole);
        index = messageTableModel_->index(rowCount, 2, QModelIndex());
        messageTableModel_->setData(index, dataSet_.getMessageName(), Qt::EditRole);
        index = messageTableModel_->index(rowCount, 3, QModelIndex());
        messageTableModel_->setData(index, dataSet_.getMessageDesc(), Qt::EditRole);

        //messageData_.append(dataSet_);

    }

}

void OgMessagePage :: existingMessage()
{
    ExistingMessageDialog existingMessageDialog(this);
    if(existingMessageDialog.exec())
    {
        pLineProtocol_->setText(existingMessageDialog.getProtcolName());

        //TODO: Filter the view based on the protocol selected in the dialog box
        // Add the new message in the database and reflect the view too
    }
}

void OgMessagePage :: updateMessage(const QModelIndex index)
{
    NewMessageDialog updateMessageDialog(this);

    //Disable the combo box for the protocol table
    updateMessageDialog.createButton->setText(QObject::tr("Update"));
    updateMessageDialog.setWindowTitle(QObject::tr("Update Message"));

    if(! index.isValid())
    {
        QMessageBox msgBox;
        msgBox.information(this,"Error",QString("Invalid message index"), QMessageBox::Ok);
    }

    int row = index.row();
    QModelIndex dataIndex;

    dataSet_    = messageData_.value(row);

    updateMessageDialog.setProtocolName(dataSet_.getProtocol());
    updateMessageDialog.protocolComboBox->setDisabled(true);
    updateMessageDialog.setMessageName(dataSet_.getMessageName());
    updateMessageDialog.setMessageDesc(dataSet_.getMessageDesc());

    if( updateMessageDialog.exec() )
    {
        QString messageName     = updateMessageDialog.getMessageName();
        QString descr           = updateMessageDialog.getMessageDesc();

        //Create the update message req and send it to the database
        //TODO: Handle the error condition here

        dataSet_.setMessageName(messageName);
        dataSet_.setMessageDesc(descr);

        dSet_->clear();
        editMessageRequest(dataSet_);
        client_->setRequest(dSet_);
        client_->sendRequest();

        //The response is populated in the variable of class OgClient, fetch it
        if (! editMessageResponse(client_->getRequest()))
        {
            QMessageBox msg;
            msg.information(this, "Error", "Error occured while updating data");

            return;
        }

        dataIndex = messageTableModel_->index(row, MESSAGE, QModelIndex());
        messageTableModel_->setData(dataIndex, messageName, Qt::EditRole);
        dataIndex = messageTableModel_->index(row, DESCRIPTION, QModelIndex());
        messageTableModel_->setData(dataIndex, descr, Qt::EditRole);

    }

}

void OgMessagePage::deleteMessage(const QModelIndex index)
{
    if(! index.isValid())
    {
        QMessageBox msgBox;
        msgBox.information(this,"Error",QString("Invalid message index"), QMessageBox::Ok);
    }

    int row = index.row();
    dataSet_ = messageData_.value(row);

    int retValue = QMessageBox::critical(this, QObject::tr("Delete Message"), QString(QObject::tr("Do you want to delete %1 message")).arg(dataSet_.getMessageName()),
                                         QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);


    if( QMessageBox::Ok == retValue )
    {

        //Create the delete message req and send it to the database
        //TODO: Handle the error condition here
        dSet_->clear();
        delMessageRequest(dataSet_.getMessageID());

        client_->setRequest(dSet_);
        client_->sendRequest();

        //The response is populated in the variable of class OgClient, fetch it
        if (! delMessageResponse(client_->getRequest()))
        {
            QMessageBox msg;
            msg.information(this, "Error", "Error occured while deleting data");
            return;
        }

        messageTableModel_->removeRows(row, 1, QModelIndex());

    }
}

void OgMessagePage :: getMessageRequest()
{
    std::string request = "REQUEST";
    std::string args    = "ARGS";
    std::string reqid   = "REQUESTID";
    std::string method  = "METHOD";
    std::string protocol= "PROTOCOLID";

    dSet_->add<int>(request, reqid, 101 );
    dSet_->add<std::string>(request, method, "getMessage");

    //Add the args section
    PropertiesPtr propPtr(new Properties());
    dSet_->add<PropertiesPtr>(request, args, propPtr);

    LOG_DEBUG("Created the args section");

    dSet_->add<std::string>(args, protocol, "ALL");

}

void OgMessagePage :: addMessageRequest(MessageDataset messageData)
{
    std::string request = "REQUEST";
    std::string args    = "ARGS";
    std::string reqid   = "REQUESTID";
    std::string method  = "METHOD";
    std::string id      = "PROTOCOLID";
    std::string message = "MESSAGE";
    std::string desc    = "DESCRIPTION";
    std::string created = "CREATED";

    dSet_->add<int>(request, reqid, 101 );
    dSet_->add<std::string>(request, method, "addNewMessage");

    //Add the args section
    PropertiesPtr propPtr(new Properties());
    dSet_->add<PropertiesPtr>(request, args, propPtr);

    LOG_DEBUG("Created the args section");

    dSet_->add<std::string>(args, id, messageData.getProtocolId().toStdString());
    dSet_->add<std::string>(args, message, messageData.getMessageName().toStdString());
    dSet_->add<std::string>(args, desc, messageData.getMessageDesc().toStdString());
    dSet_->add<std::string>(args, created, "user1");
}

void OgMessagePage :: editMessageRequest(MessageDataset messageData)
{
    std::string request = "REQUEST";
    std::string args    = "ARGS";
    std::string reqid   = "REQUESTID";
    std::string method  = "METHOD";
    std::string id      = "PROTOCOLID";
    std::string message = "MESSAGE";
    std::string messageid   = "MESSAGEID";
    std::string descr       = "DESCRIPTION";
    std::string created = "CREATED";

    dSet_->add<int>(request, reqid, 101 );
    dSet_->add<std::string>(request, method, "editMessage");

    //Add the args section
    PropertiesPtr propPtr(new Properties());
    dSet_->add<PropertiesPtr>(request, args, propPtr);

    LOG_DEBUG("Created the args section");

    dSet_->add<std::string>(args, id, messageData.getProtocolId().toStdString());
    dSet_->add<std::string>(args, messageid, messageData.getMessageID().toStdString());
    dSet_->add<std::string>(args, message, messageData.getMessageName().toStdString());
    dSet_->add<std::string>(args, descr, messageData.getMessageDesc().toStdString());
    dSet_->add<std::string>(args, created, "user1");

}

void OgMessagePage :: delMessageRequest(QString messageId)
{
    std::string request = "REQUEST";
    std::string args    = "ARGS";
    std::string reqid   = "REQUESTID";
    std::string method  = "METHOD";
    std::string messageid   = "MESSAGEID";

    dSet_->add<int>(request, reqid, 101 );
    dSet_->add<std::string>(request, method, "delMessage");

    //Add the args section
    PropertiesPtr propPtr(new Properties());
    dSet_->add<PropertiesPtr>(request, args, propPtr);

    LOG_DEBUG("Created the args section");

    dSet_->add<std::string>(args, messageid, messageId.toStdString());
}

void OgMessagePage :: getAllMessageForProtocolRequest(QString protocolId)
{
    std::string request = "REQUEST";
    std::string args    = "ARGS";
    std::string reqid   = "REQUESTID";
    std::string method  = "METHOD";
    std::string id      = "PROTOCOLID";
    std::string message = "MESSAGE";

    dSet_->add<int>(request, reqid, 101 );
    dSet_->add<std::string>(request, method, "getMessage");

    //Add the args section
    PropertiesPtr propPtr(new Properties());
    dSet_->add<PropertiesPtr>(request, args, propPtr);

    LOG_DEBUG("Created the args section");

    dSet_->add<std::string>(args, id, protocolId.toStdString());
    dSet_->add<std::string>(args, message, "ALL");

}


bool OgMessagePage :: getMessageResponse(DataSet<TextSerializer> *dSet)
{
    MessageDataset dataset;

    //Add the first empty row
    dataset.clearData();
    messageData_.clear();
    messageData_.append(dataset);

    std::string response= "REQUEST";
    std::string reqid   = "REQUESTID";
    std::string result  = "RESULTSET";
    std::string status  = "STATUS";
    std::string statMessage = "STATMESSAGE";

    //Check whether the request was successful or not
    std::string* stat = dSet->get<std::string>(response, status);
    if(! stat)
    {
        LOG_ERROR("The request was not successfully executed");
        std::string* statMes = dSet->get<std::string>(response, statMessage);
        LOG_ERROR(*statMes);
        return false;
    }

    //Check the request ID
    int *reqID   = dSet->get<int>(response, reqid);
    if(NULL == reqID)
    {
        LOG_ERROR("REQUESTID not found in the response");
        return false;
    }

    //Get the RESULTSET properties pointer
    PropertiesPtr* resultSet = dSet->get<PropertiesPtr>(response, result);

    if(NULL == resultSet)
    {
        LOG_ERROR("RESULTSET not found in the response");
        return false;
    }

    StringsPtr* record;

    for(Properties::iterator pIter = (*resultSet)->begin(); pIter != (*resultSet)->end();pIter++)
    {
         record = boost::any_cast<StringsPtr>(&(*pIter).second);

         dataset.clearData();

         for(Strings::iterator iter = (*record)->begin(); iter != (*record)->end(); iter++)
         {
            //Extract values to dataset
            dataset.setProtocol(QString::fromStdString(*iter++));
            dataset.setMessageID(QString::fromStdString(*iter++));
            dataset.setProtocolId(QString::fromStdString(*iter++));
            dataset.setMessageName(QString::fromStdString(*iter++));
            dataset.setMessageDesc(QString::fromStdString(*iter));
         }

        messageData_.append(dataset);
    }

    //Add the last empty row
    dataset.clearData();
    messageData_.append(dataset);

    LOG_DEBUG("Got PropertiesPtr");

    return true;
}

bool OgMessagePage :: addMessageResponse(DataSet<TextSerializer> *dSet, QString& messageId)
{
    std::string response    = "REQUEST";
    std::string reqid       = "REQUESTID";
    std::string mId         = "MESSAGEID";
    std::string RESULT      = "RESULTSET";
    std::string status      = "STATUS";
    std::string statMessage = "STATMESSAGE";

    //Check whether the request was successful or not
    std::string* stat = dSet->get<std::string>(response, status);
    if(! stat)
    {
        LOG_ERROR("The request was not successfully executed");
        std::string* statMes = dSet->get<std::string>(response, statMessage);
        LOG_ERROR(*statMes);
        return false;
    }

    //Check the request ID
    int *reqID   = dSet->get<int>(response, reqid);
    if(NULL == reqID)
    {
        LOG_ERROR("REQUESTID not found in the response");
        return false;
    }

    PropertiesPtr* propPtr = dSet->get<PropertiesPtr>(response, RESULT);
    if(NULL == propPtr)
    {
        LOG_ERROR("RESULTSET not found in the response");
        return false;
    }

    ///////////////////////////////////change this
    std::string* messageID = dSet->get<std::string>(RESULT, mId);

    if(NULL == messageID)
    {
        LOG_DEBUG("MESSAGEID not found in the response");
        return false;
    }
    messageId = QString::fromStdString(*messageID);
   // messageId = *messageID;

    return true;
}

bool OgMessagePage :: editMessageResponse(DataSet<TextSerializer> *dSet)
{

    std::string response    = "REQUEST";
    std::string reqid       = "REQUESTID";
    std::string RESULT      = "RESULTSET";
    std::string status      = "STATUS";
    std::string statMessage = "STATMESSAGE";

    //Check whether the request was successful or not
    std::string* stat = dSet->get<std::string>(response, status);
    if(! stat->compare("false"))
    {
        LOG_ERROR("The request was not successfully executed");
        std::string* statMes = dSet->get<std::string>(response, statMessage);
        LOG_ERROR(*statMes);
        return false;
    }

    //Check the request ID
    int *reqID   = dSet->get<int>(response, reqid);
    if(NULL == reqID)
    {
        LOG_ERROR("REQUESTID not found in the response");
        return false;
    }

    return true;
}

bool OgMessagePage :: delMessageResponse(DataSet<TextSerializer> *dSet)
{
    std::string response= "REQUEST";
    std::string reqid   = "REQUESTID";
    std::string status      = "STATUS";
    std::string statMessage = "STATMESSAGE";

    //Check whether the request was successful or not
    std::string* stat = dSet->get<std::string>(response, status);
    if( ! stat->compare("false") )
    {
        LOG_ERROR("The request was not successfully executed");
        std::string* statMes = dSet->get<std::string>(response, statMessage);
        LOG_ERROR(*statMes);
        return false;
    }

    //Check the request ID
    int *reqID   = dSet->get<int>(response, reqid);
    if(NULL == reqID)
    {
        LOG_ERROR("REQUESTID not found in the response");
        return false;
    }

    return true;
}

bool OgMessagePage :: getAllMessageForProtocolResponse(DataSet<TextSerializer> *dSet)
{
    std::string response= "REQUEST";
    std::string reqid   = "REQUESTID";
    std::string result  = "RESULTSET";
    std::string status  = "STATUS";
    std::string statMessage = "STATMESSAGE";

    //Check whether the request was successful or not
    std::string* stat = dSet_->get<std::string>(response, status);
    if(! stat )
    {
        LOG_ERROR("The request was not successfully executed");
        std::string* statMes = dSet_->get<std::string>(response, statMessage);
        if(statMes)
            LOG_ERROR(*statMes);
        return false;
    }
    else
    {
        if( 0 == stat->compare("false") )
        {
            LOG_ERROR("The request was not successfully executed");
            std::string* statMes = dSet_->get<std::string>(response, statMessage);
            if(statMes)
                LOG_ERROR(*statMes);
            return false;
        }
    }

    //Check the request ID
    int *reqID   = dSet_->get<int>(response, reqid);
    if(NULL == reqID)
    {
        LOG_ERROR("REQUESTID not found in the response");
        return false;
    }

    //Get the RESULTSET properties pointer
    PropertiesPtr* resultSet = dSet_->get<PropertiesPtr>(response, result);

    if(NULL == resultSet)
    {
        LOG_ERROR("RESULTSET not found in the response, so no : of records is zero");
        messageData_.clear();
        pRowButton_->hide();
        return true;
    }

    //QList <MessageDataset> * mDataset = new QList<MessageDataset>;
    //Add the first empty row

    MessageDataset dataset;
    dataset.clearData();
    messageData_.clear();
    messageData_.append(dataset);

    StringsPtr* record;

    for(Properties::iterator pIter = (*resultSet)->begin(); pIter != (*resultSet)->end();pIter++)
    {
         record = boost::any_cast<StringsPtr>(&(*pIter).second);

         dataset.clearData();

         for(Strings::iterator iter = (*record)->begin(); iter != (*record)->end(); iter++)
         {
            //Extract values to dataset
            dataset.setProtocol(QString::fromStdString(*iter++));
            dataset.setMessageID(QString::fromStdString(*iter++));
            dataset.setProtocolId(QString::fromStdString(*iter++));
            dataset.setMessageName(QString::fromStdString(*iter++));
            dataset.setMessageDesc(QString::fromStdString(*iter));
         }

        messageData_.append(dataset);
    }

    //Add the last empty row
    dataset.clearData();
    messageData_.append(dataset);

    return true;

}

QString OgMessagePage :: cellClickedImpl(QModelIndex index, QString& protocolId, QString& messageId)
{
    QString messageName     = "";
    QString rightPaneLabel  = "";

    if( index.isValid())
    {
        int row     = index.row();
        int column  = index.column();

        int rowCount = messageData_.size();

        dataSet_        = messageData_.at(row);
        protocolId      = dataSet_.getProtocolId();
        messageId       = dataSet_.getMessageID();

        messageName.append(dataSet_.getProtocol());
        messageName.append(" ");
        messageName.append(dataSet_.getMessageName());

        if( (row > 0 && row < rowCount-1 ) && column == messageTableModel_->TAGLINK )
        {

            rightPaneLabel.append(QObject::tr(" Tags"));
            rightPaneLabel.prepend(messageName);

            pUi_->rightPaneStackedWidget->setCurrentIndex(2);
            pUi_->rightPaneLabel->setText(rightPaneLabel);

            //Create get all tags for a particular message req and send it to the database
            //TODO error handling, this function shd not have a synchronized wait
            //getAllTagForMessageRequest(protocolId, messageId);
        }
        else if(( row > 0 && row < rowCount-1 ) && column == messageTableModel_->EDITICON)
        {
            updateMessage(index);
        }
        else if(( row > 0 && row < rowCount-1 ) && column == messageTableModel_->DELETEICON)
        {
            deleteMessage(index);
        }

    }
   return messageName;
}


void OgMessagePage::enteredImpl(const QModelIndex &index)
{
    if(index.isValid())
    {
        int column      = index.column();
        int row         = index.row();
        int rowCount    = messageData_.size();

        if( (column >= messageTableModel_->TAGLINK) && (row > 0 && row < rowCount) )
        {
            pUi_->messageTable->setCursor(QCursor(Qt::PointingHandCursor));
        }
        else
            pUi_->messageTable->setCursor(QCursor(Qt::ArrowCursor));
    }
}


QString OgMessagePage :: getIDFilter()
{
    return pLineID_->text();
}

QString OgMessagePage :: getProtocolFilter()
{
    return pLineProtocol_->text();
}

QString OgMessagePage :: getMessageFilter()
{
    return pLineMessage_->text();
}

void OgMessagePage :: setProtocolID(QString pId)
{
    protocolID_ = pId;
}

void OgMessagePage :: setIDFilter(QString id)
{
    pLineID_->setText(id);
}

void OgMessagePage :: setProtocolFilter(QString protocol)
{
    pLineProtocol_->setText(protocol);
}

void OgMessagePage :: setMessageFilter(QString message)
{
    pLineMessage_->setText(message);
}

/*
void OgMessagePage :: setMessageDataset(QList<MessageDataset> *mDataList)
{
    messageData_.clear();
    memcpy(&messageData_,mDataList, mDataList->size());
    messageTableModel_->setMessageDataset(messageData_);
}
*/
bool OgMessagePage :: getAllMessageForProtocol(QString pId)
{
    //Create get all messages for a particular protocol req and send it to the database

    //Fetch the request
    dSet_->clear();
    getAllMessageForProtocolRequest(pId);
    //Set the request to the client class
    client_->setRequest(dSet_);
    client_->sendRequest();

    //The response is populated in the variable of class OgClient, fetch it
    if (! getAllMessageForProtocolResponse(client_->getRequest()))
    {
        QMessageBox msg;
        msg.information(this, "Error", "Error occured while fetching data");
        return false;
    }

}

int OgMessagePage :: getRowCount()
{
    //return pUi_->messageTable->rowCount();
}

int OgMessagePage :: getColumnCount()
{
    //return pUi_->messageTable->columnCount();
}


