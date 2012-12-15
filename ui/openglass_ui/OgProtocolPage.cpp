/*
File name   :   OgProtocolPage.cpp
Desc        :   OgProtocolPage Class description
Author      :   Renjini
Version     :   1.0
*/


#include "OgProtocolPage.h"


int OgProtocolPage::tableChanged=1;

OgProtocolPage :: OgProtocolPage(Ui::OgAdminwindow* pUiArg, OgClient* client, QWidget * parent) : QWidget(parent), pUi_(pUiArg), client_(client)
{
    pLineID_            = new QLineEdit(this);
    pLineProtocol_      = new QLineEdit(this);
    pRowButton_         = new QPushButton(this);
    protocolProxyModel_ = new QSortFilterProxyModel(this);
    protocolTableModel_ = new ProtocolTableModel(protocolData_, this);

    std::string request = "REQUEST";
    dSet_               = new DataSet <TextSerializer> (request);

    pRowButton_->setFixedSize(55,30);
    pRowButton_->setIcon(QIcon("://images//edit_add.ico"));
    pRowButton_->setPalette(QPalette(Qt::darkGray ));
    pRowButton_->setAutoFillBackground(true);
    pLineID_->setAutoFillBackground(true);
    pLineProtocol_->setAutoFillBackground(true);

    Logger::create("openglass.log");


    createActions();
    createMenu();

    //Registering the command pattern method for giving "list of all protocols" for those who want this service
    Fog::Common::CommandRouter::getInstance()->registerCmd( "AllProtocols", 12,
                                                   "Gives the list of all protocols",31,
                               boost::bind(&OgProtocolPage::getAllProtocols,
                                           this,_1));

}

OgProtocolPage :: ~OgProtocolPage()
{
    if(pAddMenu_)
        delete pAddMenu_;

    if(pAddNewAction_)
        delete pAddNewAction_;

    if(protocolProxyModel_)
        delete protocolProxyModel_;

    if(protocolTableModel_)
        delete protocolTableModel_;

    if(dSet_)
        delete dSet_;
}

/*
  Reads the protocols.txt file and populates the protocol table model.
  This model is then shown to user in the UI protocol table.
*/

bool OgProtocolPage::setTableData()
{
    //Open the file
    InputFile protocolFile("//appl//openglass//ui//openglass_ui//inputfiles//protocols.txt");
    if(! protocolFile.open() )
        return false;

    //Populate the header labels in the model
    QStringList headerLabel = protocolFile.getHeaderLabel();
    protocolTableModel_->setProtocolTableHeader(headerLabel);


    dSet_->clear();

    //Fetch the request
    getProtocolRequest();
    //Set the request to the client class
    client_->setRequest(dSet_);
    if ( ! client_->sendRequest())
    {
        QMessageBox msg;
        msg.information(this, "Connection Error", "Server not connected");
        //msg.exec();
        return false;
    }

    //The response is populated in the variable of class OgClient, fetch it
    if (! getProtocolResponse(client_->getRequest()))
    {
        QMessageBox msg;
        msg.information(this, "Error", "Error occured while fetching data");
        //msg.exec();
        return false;
    }

    protocolFile.close();

    //protocolTableModel_->setProtocolDataset(protocolData_);

    return true;
}

void OgProtocolPage :: createActions()
{
    pAddNewAction_ = new QAction(QWidget::tr("&Add New"),this);
    //pAddNewAction_->setIcon();
    pAddNewAction_->setToolTip(QObject::tr("Add new protocol"));
    pAddNewAction_->setShortcut(QObject::tr("Ctrl+N"));
    QObject::connect(pAddNewAction_, SIGNAL(triggered()), this, SLOT(newProtocol()) );

}

void OgProtocolPage :: createMenu()
{
    pAddMenu_           = new QMenu();
    pAddMenu_->addAction(pAddNewAction_);
    pRowButton_->setMenu(pAddMenu_);
}
/*
  Use removerows and insertrows in getProtocolResponse()
  then add the buttons and linedits
  */
void OgProtocolPage :: initializeTable()
{

    bool result;

    QString rightPaneLabel;
    QString protocolName;

    if(pUi_->rightPaneStackedWidget->isHidden())
        pUi_->rightPaneStackedWidget->show();

    //Populate data in the model
    setTableData();


    pUi_->rightPaneStackedWidget->setCurrentIndex(0);

    //These are needed to be done at the initial run
    if(tableChanged == 1)
    {
        tableChanged = 0;
        //Set the model
        pUi_->protocolTable->setModel(protocolTableModel_);

        //Set the LineEdits in the first row
        QModelIndex index;
        index = protocolTableModel_->index(0, 0);
        pUi_->protocolTable->setIndexWidget(index, pLineID_);
        index = protocolTableModel_->index(0, 1);
        pUi_->protocolTable->setIndexWidget(index, pLineProtocol_);

        //Set the row button
        int row = protocolTableModel_->rowCounter();
        index   = protocolTableModel_->index(row-1, 0);
        pUi_->protocolTable->setIndexWidget(index, pRowButton_);

    }

    //Setting right pane label
    //Check the protocol filter before updating
    protocolName = pLineProtocol_->text();
    if(! protocolName.isEmpty())
    {
        rightPaneLabel.append(QObject::tr(" Protocol"));
        rightPaneLabel.prepend(protocolName);
    }
    else
    {
        rightPaneLabel.append(QObject::tr("All Protocols"));
    }

    pUi_->rightPaneLabel->setText(rightPaneLabel);

        //QMessageBox msgBox;
        //  msgBox.information(this,"Information",QString("Rows %1").arg(row), QMessageBox::Ok);

    //Connect the signals to the slots

    //Handle enter SIGNAL for protocol table
    connect(pUi_->protocolTable,SIGNAL(entered(QModelIndex)), this, SLOT(enteredImpl(QModelIndex)));


    //filter lineedits signal slot connection
    //connect(pLineID_, SIGNAL(textChanged(QString)), this, SLOT(pLineIDTextChanged(QString)));
    //connect(pLineProtocol_, SIGNAL(textChanged(QString)), this, SLOT(pLineProtocolTextChanged(QString)));

}

/*******************************************************************************************
 *
 * UI input handling functions for adding , updating and deleting protocols
 *
 *
 ********************************************************************************************/

void OgProtocolPage :: newProtocol()
{
    NewProtocolDialog newProtocolDialog(this);

    if(newProtocolDialog.exec())
    {
        QModelIndex index;
        int rowCount = ( protocolTableModel_->rowCounter() - 1 );

        //Get the protocol details from the dialog box
        //Assign it in the protocol dataset
        //Create the add protocol req and send it to the database
        //Handle the error condition here

        dataSet_.clearData();
        //For new protocol, ID will be assigned in the database (auto-increment)
        dataSet_.setProtocolName(newProtocolDialog.getProtocolName());
        dataSet_.setProtocolType(newProtocolDialog.getProtocolType());
        dataSet_.setProtocolVersion(newProtocolDialog.getProtocolVersion());

        //Send the request
        dSet_->clear();
        addProtocolRequest(dataSet_);
        client_->setRequest(dSet_);
        client_->sendRequest();

        //The response is populated in the variable of class OgClient, fetch it
        int protocolId;
        if (! addProtocolResponse(client_->getRequest(), protocolId))
        {
            QMessageBox msg;
            msg.information(this, "Error", "Error occured while adding data");

            return;
        }

        //Update the model
        protocolTableModel_->insertRows(rowCount, 1, QModelIndex());

        //Set the protocol Id received from the database, as it is auto-incremental
        index = protocolTableModel_->index(rowCount, ID, QModelIndex());
        protocolTableModel_->setData(index, protocolId, Qt::EditRole);

        index = protocolTableModel_->index(rowCount, PROTOCOL, QModelIndex());
        protocolTableModel_->setData(index, dataSet_.getProtocolName(), Qt::EditRole);

        index = protocolTableModel_->index(rowCount, VERSION, QModelIndex());
        protocolTableModel_->setData(index, dataSet_.getProtocolVersion(), Qt::EditRole);

        protocolTableModel_->setProtocolType(rowCount, dataSet_.getProtocolType());
    }
}

void OgProtocolPage :: updateProtocol(const QModelIndex index)
{
    NewProtocolDialog updateProtocolDialog(this);

    updateProtocolDialog.createButton->setText(QObject::tr("Update"));
    updateProtocolDialog.setWindowTitle(QObject::tr("Update Protocol"));

    if(! index.isValid())
    {
        QMessageBox msgBox;
        msgBox.information(this,"Error",QString("Invalid protocol index"), QMessageBox::Ok);
        msgBox.exec();
    }

    int row = index.row();
    QModelIndex dataIndex;

    dataSet_    = protocolData_.value(row);
    updateProtocolDialog.setProtocolName(dataSet_.getProtocolName());
    updateProtocolDialog.setProtocolVersion(dataSet_.getProtocolVersion());
    updateProtocolDialog.setProtocolType(dataSet_.getProtocolType());

    if( updateProtocolDialog.exec() )
    {
        //Get the user input values from the dialog box
        QString protocolName    = updateProtocolDialog.getProtocolName();
        QString version         = updateProtocolDialog.getProtocolVersion();
        QString type            = updateProtocolDialog.getProtocolType();

        dataSet_.setProtocolName(protocolName);
        dataSet_.setProtocolVersion(version);
        dataSet_.setProtocolType(type);

        //Send the request
        dSet_->clear();
        editProtocolRequest(dataSet_);
        client_->setRequest(dSet_);
        client_->sendRequest();

        //The response is populated in the variable of class OgClient, fetch it
        if (! editProtocolResponse(client_->getRequest()))
        {
            QMessageBox msg;
            msg.information(this, "Error", "Error occured while updating data");

            return;
        }

        //If Successful update the model
        dataIndex = protocolTableModel_->index(row, PROTOCOL, QModelIndex());
        protocolTableModel_->setData(dataIndex, protocolName, Qt::EditRole);
        dataIndex = protocolTableModel_->index(row, VERSION, QModelIndex());
        protocolTableModel_->setData(dataIndex, version, Qt::EditRole);

        protocolTableModel_->setProtocolType(row, type);
    }
}

void OgProtocolPage::deleteProtocol(const QModelIndex index)
{
    if(! index.isValid())
    {
        QMessageBox msgBox;
        msgBox.information(this,"Error",QString("Invalid protocol index"), QMessageBox::Ok);
    }

    int row = index.row();
    dataSet_ = protocolData_.value(row);

    int retValue = QMessageBox::critical(this, QObject::tr("Delete Protocol"), QString(QObject::tr("Do you want to delete %1 protocol")).arg(dataSet_.getProtocolName()),
                                         QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);


    //Create the delete protocol req and send it to the database
    //Handle the error condition here

    if( QMessageBox::Ok == retValue )
    {
        //Send the request
        dSet_->clear();
        delProtocolRequest(dataSet_.getProtocolID());

        client_->setRequest(dSet_);
        client_->sendRequest();

        //The response is populated in the variable of class OgClient, fetch it
        if (! delProtocolResponse(client_->getRequest()))
        {
            QMessageBox msg;
            msg.information(this, "Error", "Error occured while deleting data");

            return;
        }

        //Update the model
        protocolTableModel_->removeRows(row, 1, QModelIndex());

    }

}

/*******************************************************************************************
 *
 * Database request handling functions for adding , updating and deleting protocols
 *
 *
 *******************************************************************************************/

/*
  * The request is populated to the DataSet variable in the class OgProtocolPage
  */

bool OgProtocolPage :: getProtocolRequest()
{

    std::string request = "REQUEST";
    std::string args    = "ARGS";
    std::string reqid   = "REQUESTID";
    std::string method  = "METHOD";
    std::string protocolId      = "PROTOCOLID";

    dSet_->add<int>(request, reqid, 101 );
    dSet_->add<std::string>(request, method, "getProtocol");

    //Add the args section
    PropertiesPtr propPtr(new Properties());
    if( ! dSet_->add<PropertiesPtr>(request, args, propPtr) )
    {
        LOG_ERROR("ARGS Properties could not be added to the request");
        return false;
    }

    LOG_DEBUG("Created the args section");

    dSet_->add<std::string>(args, protocolId, "ALL");

    std::string req = dSet_->serialize();
    if(req.empty())
    {
        LOG_ERROR("DataSet Serialize returned empty string");
        return false;
    }

    LOG_DEBUG(req);

    return true;
    //Add the length of the req at the beginning and Send the req to TCPClient
    //Right now I am doing it in the OgClient SendRequest function.
}

void OgProtocolPage :: addProtocolRequest(ProtocolDataset protocolData)
{
    std::string request = "REQUEST";
    std::string args    = "ARGS";
    std::string reqid   = "REQUESTID";
    std::string method  = "METHOD";
    std::string protocol    = "PROTOCOL";
    std::string type        = "TYPE";
    std::string version     = "VERSION";
    std::string created     = "CREATED";

    dSet_->add<int>(request, reqid, 101 );
    dSet_->add<std::string>(request, method, "addProtocol");

    //Add the args section

    PropertiesPtr propPtr( new Properties() );
    dSet_->add<PropertiesPtr>(request, args, propPtr);

    LOG_DEBUG("Created the args section");


    dSet_->add<std::string>(args, protocol, protocolData.getProtocolName().toStdString());
    dSet_->add<std::string>(args, type, protocolData.getProtocolType().toStdString());
    dSet_->add<std::string>(args, version, protocolData.getProtocolVersion().toStdString());
    dSet_->add<std::string>(args, created, "user1");

    std::string req = dSet_->serialize();
    if(req.empty())
    {
        LOG_ERROR("DataSet Serialize returned empty string");
    }
    LOG_DEBUG(req);

}

void OgProtocolPage :: editProtocolRequest(ProtocolDataset protocolData)
{
    std::string request = "REQUEST";
    std::string args    = "ARGS";
    std::string reqid   = "REQUESTID";
    std::string method  = "METHOD";
    std::string id      = "PROTOCOLID";
    std::string protocol    = "PROTOCOL";
    std::string type        = "TYPE";
    std::string version     = "VERSION";
    std::string created     = "CREATED";

    LOG_DEBUG("Created the first section");

    dSet_->add<int>(request, reqid, 101 );
    dSet_->add<std::string>(request, method, "editProtocol");

    LOG_DEBUG("Created reqid and method");

    //Add the args section
    PropertiesPtr propPtr(new Properties());
    dSet_->add<PropertiesPtr>(request, args, propPtr);

    LOG_DEBUG("Created the args section");

    //////////////////////////////////////change this
    dSet_->add<std::string>(args, id, protocolData.getProtocolID().toStdString());
    dSet_->add<std::string>(args, protocol, protocolData.getProtocolName().toStdString());
    dSet_->add<std::string>(args, type, protocolData.getProtocolType().toStdString());
    dSet_->add<std::string>(args, version, protocolData.getProtocolVersion().toStdString());
    dSet_->add<std::string>(args, created, "user1");

}

void OgProtocolPage :: delProtocolRequest(QString protocolId)
{
    std::string request = "REQUEST";
    std::string args    = "ARGS";
    std::string reqid   = "REQUESTID";
    std::string method  = "METHOD";
    std::string id      = "PROTOCOLID";

    dSet_->add<int>(request, reqid, 101 );
    dSet_->add<std::string>(request, method, "delProtocol");

    //Add the args section
    PropertiesPtr propPtr(new Properties());
    dSet_->add<PropertiesPtr>(request, args, propPtr);

    LOG_DEBUG("Created the args section");

    dSet_->add<std::string>(args, id, protocolId.toStdString());

}


/*******************************************************************************************
 *
 * Database response handling functions for adding , updating and deleting protocols
 *
 *
 *******************************************************************************************/

bool OgProtocolPage :: getProtocolResponse(DataSet<TextSerializer> *dSet)
{

    ProtocolDataset dataset;

    //int rowCount = protocolTableModel_->rowCounter();

    //Remove all the data rom the view
    //protocolTableModel_->removeRows(0, rowCount, QModelIndex());

    //Add the first empty row
    dataset.clearData();
    protocolData_.clear();
    protocolData_.append(dataset);

    // protocolTableModel_->insertRows(0, 1, QModelIndex());

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

    //int row = 1;
    //QModelIndex index;

    for(Properties::iterator pIter = (*resultSet)->begin(); pIter != (*resultSet)->end();pIter++)
    {
         record = boost::any_cast<StringsPtr>(&(*pIter).second);

         dataset.clearData();

         for(Strings::iterator iter = (*record)->begin(); iter != (*record)->end(); iter++)
         {
            //Extract values to dataset
            dataset.setProtocolName(QString::fromStdString(*iter++));
            dataset.setProtocolType(QString::fromStdString(*iter++));
            dataset.setProtocolVersion(QString::fromStdString(*iter++));
            dataset.setProtocolID(QString::fromStdString(*iter));
            //dataset.setProtocolDesc(QString::fromStdString(record[i]);
         }

         /*
         protocolTableModel_->insertRows(row, 1, QModelIndex());

         index = protocolTableModel_->index(row, ID, QModelIndex());
         protocolTableModel_->setData(index, dataset.getProtocolID(), Qt::EditRole);

         index = protocolTableModel_->index(row, PROTOCOL, QModelIndex());
         protocolTableModel_->setData(index, dataset.getProtocolName(), Qt::EditRole);

         index = protocolTableModel_->index(row, VERSION, QModelIndex());
         protocolTableModel_->setData(index, dataset.getProtocolVersion(), Qt::EditRole);

         protocolTableModel_->setProtocolType(row, dataset.getProtocolType());
        */

         protocolData_.append(dataset);
    }

    //Add the last empty row
    dataset.clearData();
    protocolData_.append(dataset);

    /*
    rowCount = protocolTableModel_->rowCounter();
    protocolTableModel_->insertRows(rowCount, 1, QModelIndex());

    //Set the LineEdits in the first row
    index = protocolTableModel_->index(0, 0);
    pUi_->protocolTable->setIndexWidget(index, pLineID_);
    index = protocolTableModel_->index(0, 1);
    pUi_->protocolTable->setIndexWidget(index, pLineProtocol_);

    //Set the row button
    index   = protocolTableModel_->index(rowCount, 0);
    pUi_->protocolTable->setIndexWidget(index, pRowButton_);

    */

    LOG_DEBUG("Got PropertiesPtr");

    return true;

}

bool OgProtocolPage :: addProtocolResponse(DataSet<TextSerializer> *dSet, int& protocolId)
{
    std::string response    = "REQUEST";
    std::string reqid       = "REQUESTID";
    std::string protId      = "PROTOCOLID";
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
    int* protocolID = dSet->get<int>(RESULT, protId);

    if(NULL == protocolID)
    {
        LOG_DEBUG("PROTOCOLID not found in the response");
        return false;
    }
    //protocolId = QString::fromUtf16((ushort*) protocolID);
    protocolId = *protocolID;

    return true;
}

bool OgProtocolPage :: editProtocolResponse(DataSet<TextSerializer> * dSet)
{
    std::string response    = "REQUEST";
    std::string reqid       = "REQUESTID";
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

    return true;
}

bool OgProtocolPage :: delProtocolResponse(DataSet<TextSerializer> * dSet)
{
    std::string response= "REQUEST";
    std::string reqid   = "REQUESTID";
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



QString OgProtocolPage :: cellClickedImpl(QModelIndex index, QString& protocolID/*, QList <MessageDataset>* mDataset*/)
{
    QString protocolName = "";
    QString rightPaneLabel = "";

    if( index.isValid())
    {
        int row     = index.row();
        int column  = index.column();

        int rowCount = protocolData_.size();

        dataSet_        = protocolData_.at(row);
        protocolName    = dataSet_.getProtocolName();
        protocolID      = dataSet_.getProtocolID();

        if( ( row > 0 && row < rowCount-1 ) && column == protocolTableModel_->MESSAGELINK ) //message link clicked
        {
            rightPaneLabel.append(QObject::tr(" Messages"));
            rightPaneLabel.prepend(protocolName);

            pUi_->rightPaneStackedWidget->setCurrentIndex(1);
            pUi_->rightPaneLabel->setText(rightPaneLabel);

        }
        else if(row > 0 && column == protocolTableModel_->TAGLINK)  //tag link clicked
        {
            rightPaneLabel.append(QObject::tr(" Tags"));
            rightPaneLabel.prepend(protocolName);

            pUi_->rightPaneStackedWidget->setCurrentIndex(2);
            pUi_->rightPaneLabel->setText(rightPaneLabel);

        }
        else if(( row > 0 && row < rowCount-1 ) && column == protocolTableModel_->EDITICON) //edit button clicked
        {
            updateProtocol(index);
        }
        else if(( row > 0 && row < rowCount-1 ) && column == protocolTableModel_->DELETEICON)   //delete button clicked
        {
            deleteProtocol(index);
        }

    }

    return protocolName;

}
/*
void OgProtocolPage :: cellEnteredImpl(int row, int column)
{
    if((row >0 && row <3) && ( column == 3 || column == 4))
    {
        QLabel* cellLabel    = new QLabel(this);
        cellLabel = (QLabel*)pUi_->protocolTable->cellWidget(row, column);
        cellLabel->setStyleSheet(":hover{color:blue}");
    }
}
*/

void OgProtocolPage::enteredImpl(const QModelIndex &index)
{
    if(index.isValid())
    {
        int column      = index.column();
        int row         = index.row();
        int rowCount    = protocolData_.size();

        if( (column >= protocolTableModel_->MESSAGELINK) && (row > 0 && row < rowCount - 1) )
        {
            pUi_->protocolTable->setCursor(QCursor(Qt::PointingHandCursor));
        }
        else
            pUi_->protocolTable->setCursor(QCursor(Qt::ArrowCursor));
    }
}

void OgProtocolPage::pLineIDTextChanged(QString protocolId)
{
    /*QMessageBox msgBox;
    msgBox.information(this,"Information",QString("Protocol Id %1").arg(protocolId), QMessageBox::Ok);*/
    QRegExp regExp(protocolId, Qt::CaseInsensitive, QRegExp::Wildcard);
    protocolProxy_.setFilterRegExp(regExp);

}

void OgProtocolPage::pLineProtocolTextChanged(QString protocolName)
{
    QMessageBox msgBox;
    msgBox.information(this,"Information",QString("Protocol Name %1").arg(protocolName), QMessageBox::Ok);
}

QString OgProtocolPage :: getIDFilter()
{
    return pLineID_->text();
}

QString OgProtocolPage :: getProtocolFilter()
{
    return pLineProtocol_->text();
}

void OgProtocolPage :: setIDFilter(QString id)
{
    pLineID_->setText(id);
}

void OgProtocolPage :: setProtocolFilter(QString protocol)
{
    pLineProtocol_->setText(protocol);
}

int OgProtocolPage :: getRowCount()
{
    //return pUi_->protocolTable->rowCount();
}

int OgProtocolPage :: getColumnCount()
{
    //return pUi_->protocolTable->columnCount();
}

void OgProtocolPage :: getAllProtocols(DataSet<TextSerializer> &dSet)
{
    LOG_DEBUG("Entered");

    //Get the protocolData_ list and snd it across to the message page
    dSet.add<QList <ProtocolDataset> *>("REQUEST", "PROTOCOLDATASET", &protocolData_);
}

bool ProtocolFilterProxy::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if(sourceRow == 0)
        return true;
    return true;
}
