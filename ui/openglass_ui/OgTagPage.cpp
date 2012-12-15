/*
File name   :   OgTagPage.cpp
Desc        :   OgTagPage Class description
Author      :   Renjini
Version     :   1.0
*/


#include "OgTagPage.h"


int OgTagPage::tableChanged=1;

OgTagPage :: OgTagPage(Ui::OgAdminwindow* pUiArg, OgClient *client, QWidget * parent) : QWidget(parent), pUi_(pUiArg), client_(client)
{
    pLineID_            = new QLineEdit(this);
    pLineProtocol_      = new QLineEdit(this);
    pLineTag_           = new QLineEdit(this);
    pRowButton_         = new QPushButton(this);
    tagTableModel_      = new TagTableModel(tagData_, this);

    pRowButton_->setFixedSize(55,30);
    pRowButton_->setIcon(QIcon("://images//edit_add.ico"));
    pRowButton_->setPalette(QPalette(Qt::darkGray ));

    dSet_ = new DataSet<TextSerializer> ("REQUEST");

    createActions();
    createMenu();

    //Registering the command pattern method for giving "list of all tags not in a particular protocol"
    //for those who want this service
    Fog::Common::CommandRouter::getInstance()->registerCmd( "AllTagsNotInTheProtocol", 23,
                                                   "Gives the list of all tags not in a particular protocol",55,
                                                   boost::bind(&OgTagPage::getAllTagNotInProtocolForNewTagDialog,
                                                   this,_1));
    Fog::Common::CommandRouter::getInstance()->registerCmd( "AllTagsInAllTheProtocols", 24,
                                                   "Gives the list of all tags in all protocols",44,
                                                   boost::bind(&OgTagPage::getAllTagInAllProtocolsForNewTagDialog,
                                                   this,_1));


}

OgTagPage :: ~OgTagPage()
{
    delete pRowButton_;
    delete pAddMenu_;
    delete pAddNewAction_;
    //delete pAddExistingAction_;
    if(dSet_)
        delete dSet_;
}

void OgTagPage :: createActions()
{
    pAddNewAction_ = new QAction(QWidget::tr("&Add Tag"),this);
    //pAddNewAction_->setIcon();
    pAddNewAction_->setToolTip(QObject::tr("Add new tag or select an existing one"));
    pAddNewAction_->setShortcut(QObject::tr("Ctrl+N"));
    QObject::connect(pAddNewAction_, SIGNAL(triggered()), this, SLOT(newTag()));

    /*
    pAddExistingAction_ = new QAction(QWidget::tr("&Add Existing..."),this);
    //pAddExistingAction_->setIcon();
    pAddExistingAction_->setToolTip(QObject::tr("Add existing message"));
    pAddExistingAction_->setShortcut(QObject::tr("Ctrl+E"));
    QObject::connect(pAddExistingAction_, SIGNAL(triggered()), this, SLOT(existingMessage()) );*/
}

void OgTagPage :: createMenu()
{
    pAddMenu_           = new QMenu();
    pAddMenu_->addAction(pAddNewAction_);
    //pAddMenu_->addAction(pAddExistingAction_);
    pRowButton_->setMenu(pAddMenu_);
}

/*
  Reads the tags.txt file and populates the protocol table model.
  This model is then shown to user in the UI protocol table.
*/

bool OgTagPage::setTableData()
{

    protocolID_.clear();        //User has not selected any protocol
    messageID_.clear();         //User has not selected any message

    //Open the file
    InputFile tagFile("//appl//openglass//ui//openglass_ui//inputfiles//tags.txt");
    if(! tagFile.open() )
        return false;

    //Populate the header labels in the model
    QStringList headerLabel = tagFile.getHeaderLabel();
    tagTableModel_->setTagTableHeader(headerLabel);


    dSet_->clear();
    //Fetch the request
    getTagRequest();
    //Set the request to the client class
    client_->setRequest(dSet_);
    client_->sendRequest();

    //The response is populated in the variable of class OgClient, fetch it
    if (! getTagResponse(client_->getRequest()))
    {
        QMessageBox msg;
        msg.information(this, "Error", "Error occured while fetching data");
        return false;
    }

    //tagTableModel_->setTagDataset(tagData_);

    tagFile.close();

    return true;
}

void OgTagPage :: initializeTable()
{

    bool result;

    protocolID_.clear();        //User has not selected any protocol
    messageID_.clear();         //User has not sleected any message

    if(pUi_->rightPaneStackedWidget->isHidden())
        pUi_->rightPaneStackedWidget->show();


    pUi_->rightPaneLabel->setText("All Tags");
    pUi_->rightPaneStackedWidget->setCurrentIndex(2);

    //Populate data in the model
    setTableData();

    QModelIndex index;

    if(tableChanged == 1)
    {
        tableChanged = 0;

        //Set the model
        pUi_->tagTable->setModel(tagTableModel_);

         //Set the LineEdits in the first row
        index = tagTableModel_->index(0, 0);
        pUi_->tagTable->setIndexWidget(index, pLineID_);
        index = tagTableModel_->index(0, 1);
        pUi_->tagTable->setIndexWidget(index, pLineProtocol_);
        index = tagTableModel_->index(0, 2);
        pUi_->tagTable->setIndexWidget(index, pLineTag_);

        //Set the row button
        int row = tagTableModel_->rowCounter();
        index = tagTableModel_->index(row-1, 0);
        pUi_->tagTable->setIndexWidget(index, pRowButton_);

    }

    //Clear the lineEdits
    pLineID_->clear();
    pLineProtocol_->clear();
    pLineTag_->clear();

    //Connect signals and slots

    //Handle enter SIGNAL for tag table
    connect(pUi_->tagTable, SIGNAL(entered(QModelIndex)), this, SLOT(enteredImpl(QModelIndex)));
    //Handle cellClicked SIGNAL to tag table
    connect(pUi_->tagTable, SIGNAL(clicked(QModelIndex)), this, SLOT(cellClickedImpl(QModelIndex)));
}

/* Registered method for newTagDialog class, to give the list of all tags in all the protocols */
bool OgTagPage :: getAllTagInAllProtocolsForNewTagDialog(DataSet<TextSerializer> &dSet)
{
    dSet_->clear();

    getTagRequest();
    client_->setRequest(dSet_);
    client_->sendRequest();

    if ( ! getTagResponse() )
    {
        return false;
    }
    //Get the tagDataForProtocol_ list and send it across to the newtagDialog class
    dSet.add<QList <TagDataset> *>("REQUEST", "TAGDATASET", &tagDataForNewDialog_);

}

bool OgTagPage :: getTagResponse()
{
    tagDataForNewDialog_.clear();

    TagDataset dataset;

    std::string response= "REQUEST";
    std::string reqid   = "REQUESTID";
    std::string result  = "RESULTSET";
    std::string status  = "STATUS";
    std::string statMessage = "STATMESSAGE";

    //Check whether the request was successful or not
    std::string* stat = dSet_->get<std::string>(response, status);
    if( ! stat)
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
        tagDataForNewDialog_.clear();
        //pRowButton_->hide();
        return true;
    }

    StringsPtr* record;

    for(Properties::iterator pIter = (*resultSet)->begin(); pIter != (*resultSet)->end();pIter++)
    {
         record = boost::any_cast<StringsPtr>(&(*pIter).second);

         dataset.clearData();

         for(Strings::iterator iter = (*record)->begin(); iter != (*record)->end(); iter++)
         {
            //Extract values to dataset only if the tag does not belong to the protocol

            dataset.setProtocolId(QString::fromStdString(*iter++));
            dataset.setTagID(QString::fromStdString(*iter++));
            dataset.setProtocol(QString::fromStdString(*iter++));
            dataset.setTagName(QString::fromStdString(*iter++));
            dataset.setTagDesc(QString::fromStdString(*iter++));
            dataset.setTagLength(QString::fromStdString(*iter++));
            dataset.setTagType(QString::fromStdString(*iter));
           // dataset.setDestDatatype(QString::fromStdString(*iter++));
           // dataset.setJustifyWith(QString::fromStdString(*iter++));
            //dataset.setTagEditable(QString::fromStdString(*iter++));


         }

         tagDataForNewDialog_.append(dataset);
    }

    LOG_DEBUG("Got PropertiesPtr");

    return true;


}


/* Registered method for newTagDialog class, to give the list of all tags **not present** in a particular protocol */
bool OgTagPage :: getAllTagNotInProtocolForNewTagDialog(DataSet<TextSerializer> &dSet)
{
    std::string* protocolId = dSet.get<std::string>("REQUEST", "PROTOCOLID");

    if(! protocolId)
        return false;

    dSet_->clear();

    getTagRequest();
    client_->setRequest(dSet_);
    client_->sendRequest();

    if ( ! getTagResponse( QString::fromStdString(*protocolId) ) )
    {
        return false;
    }

    //Get the tagDataForProtocol_ list and send it across to the newtagDialog class
    dSet.add<QList <TagDataset> *>("REQUEST", "TAGDATASET", &tagDataForNewDialog_);

}

/* Get the list of tags not present in a particular protocol, save the list in a separate tagDataset */
bool OgTagPage :: getTagResponse( QString protocolId )
{
    tagDataForNewDialog_.clear();

    TagDataset dataset;

    std::string response= "REQUEST";
    std::string reqid   = "REQUESTID";
    std::string result  = "RESULTSET";
    std::string status  = "STATUS";
    std::string statMessage = "STATMESSAGE";

    //Check whether the request was successful or not
    std::string* stat = dSet_->get<std::string>(response, status);
    if( ! stat)
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
        tagDataForNewDialog_.clear();
        //pRowButton_->hide();
        return true;
    }

    StringsPtr* record;
    bool found;

    for(Properties::iterator pIter = (*resultSet)->begin(); pIter != (*resultSet)->end();pIter++)
    {
         found = false;

         record = boost::any_cast<StringsPtr>(&(*pIter).second);

         dataset.clearData();

         for(Strings::iterator iter = (*record)->begin(); iter != (*record)->end(); iter++)
         {
            //Extract values to dataset only if the tag does not belong to the protocol
            if( 0 == protocolId.compare(QString::fromStdString(*iter)) )
            {
                found = true;
                break;
            }

            dataset.setProtocolId(QString::fromStdString(*iter++));
            dataset.setTagID(QString::fromStdString(*iter++));
            dataset.setProtocol(QString::fromStdString(*iter++));
            dataset.setTagName(QString::fromStdString(*iter++));
            dataset.setTagDesc(QString::fromStdString(*iter++));
            dataset.setTagLength(QString::fromStdString(*iter++));
            dataset.setTagType(QString::fromStdString(*iter));
           // dataset.setDestDatatype(QString::fromStdString(*iter++));
           // dataset.setJustifyWith(QString::fromStdString(*iter++));
            //dataset.setTagEditable(QString::fromStdString(*iter++));


         }

         if(! found)
            tagDataForNewDialog_.append(dataset);
    }

    LOG_DEBUG("Got PropertiesPtr");

    return true;


}

void OgTagPage :: newTag()
{
    NewTagDialog newTagDialog(this);

    int     Generic         = BOTH;
    int     tagComboIndex   = 0;
    QString tagID;

    //Set the protocolId and messageId if any,
    newTagDialog.setProtocolID(protocolID_);
    newTagDialog.setMessageID(messageID_);

    //Populate the protocol comboBox
    if(! newTagDialog.populateProtocolComboBox())
    {
        QMessageBox msg;
        msg.information(this, "Error", "Service not available");
        newTagDialog.close();
    }

    //User has come from message page (Viewing tags for a particular message)
    //protocol combobox disabled. Tag combobox will show all the tags in all protocols
    //Allowed to enter the non-generic properties of the tag
    if((! protocolID_.isEmpty()) && (! messageID_.isEmpty()))
    {
        newTagDialog.messageBasedGroupBox->setEnabled(true);
        newTagDialog.generalGroupBox->setEnabled(true);
        newTagDialog.protocolComboBox->setDisabled(true);
        newTagDialog.tagComboBox->setEnabled(true);
        Generic = NONGENERIC;
    }
    //User has come from protocol page (Viewing tags for a particular protocol)
    //protocol combobox disabled. Tag combobox will show all the tags that are ***not*** present in the selected protocol
    //Allowed to enter only the generic properties of the tag
    else if((! protocolID_.isEmpty()) && (messageID_.isEmpty()))
    {
        newTagDialog.messageBasedGroupBox->setDisabled(true);
        newTagDialog.generalGroupBox->setEnabled(true);
        newTagDialog.protocolComboBox->setDisabled(true);
        newTagDialog.tagComboBox->setEnabled(true);
        Generic = GENERIC;
    }
    //User has invoked the left pane button (Viewing tags for all protocol)
    //Select the protocol, then tagCombobox shd display the list of tags that are not in the protocol
    //Allowed to enter the generic properties of the tag
    else if((protocolID_.isEmpty()) && (messageID_.isEmpty()))
    {
        newTagDialog.messageBasedGroupBox->setDisabled(true);
        newTagDialog.generalGroupBox->setEnabled(true);
        newTagDialog.protocolComboBox->setEnabled(true);
        Generic = GENERIC;
    }

    //Fill the comboBox with tags available for protocol/message, in effect tagData_ gets populated
    //Population of tag combo box is done from here only when user comes from protocol / message page.
    newTagDialog.setGenericity(Generic);

    if( ! protocolID_.isEmpty() || ! messageID_.isEmpty() )
    {
        newTagDialog.populateTagComboBox(protocolID_);
    }

    if(newTagDialog.exec())
    {
        QModelIndex index;
        int rowCount = ( tagTableModel_->rowCounter() - 1 );
        /*
        QMessageBox msgBox;
        msgBox.information(0,"Information",QString("Rows %1").arg(rowCount), QMessageBox::Ok);
        */

        //Get the comboBox index selected by the user
        tagComboIndex = newTagDialog.getTagComboBoxIndex();

        //Get the tag details from the dialog box
        //Assign it in the message dataset and send it across the network module

        dataSet_.clearData();

        LOG_DEBUG("Generic state = ");
        LOG_DEBUG(Generic);

        //Check the state, it could have been changed in newTagDialog
        Generic = newTagDialog.getGenericity();

        if( Generic == GENERIC || Generic == BOTH )
        {
            //Need to get the protocol ID from the comboBox, whenuser comes from left pane
            //Else if user comes from message or protocol page, protocolID will be already set in
            //protocolID_

            if(! protocolID_.isEmpty())
                dataSet_.setProtocolId(protocolID_);        //user comes from protocol page and message page
            else
                dataSet_.setProtocol(newTagDialog.getProtocol()); //user comes from left pane

            dataSet_.setProtocol(newTagDialog.getProtocolName());
            dataSet_.setTagName(newTagDialog.getTagName());
            dataSet_.setTagDesc(newTagDialog.getTagDesc());
            dataSet_.setTagLength(newTagDialog.getTagLength());
            dataSet_.setTagType(newTagDialog.getTagType());
            dataSet_.setDestDatatype(newTagDialog.getDestDatatype());
            dataSet_.setJustifyWith(newTagDialog.getJustifyWith());
            dataSet_.setTagEditable(newTagDialog.getTagEditable());

        }

        if ( Generic == NONGENERIC || Generic == BOTH )
        {
            dataSet_.setMessageId(messageID_);
            dataSet_.setTagReq(newTagDialog.getTagReq());
            dataSet_.setTagSeq(newTagDialog.getTagSeq());
            dataSet_.setTagOffset(newTagDialog.getTagOffset());
            dataSet_.setTagDefValue(newTagDialog.getTagDefValue());

        }

        //Send the request
        dSet_->clear();
        addTagRequest(dataSet_, Generic);
        client_->setRequest(dSet_);
        client_->sendRequest();

        //Create the add message req and send it to the database
        //TODO: Handle the error condition here

        if(! addTagResponse(client_->getRequest(), tagID))
        {
            QMessageBox msg;
            msg.information(this, "Error", "Error occured while adding data");

            return;
        }

        tagTableModel_->insertRows(rowCount, 1, QModelIndex());
        index = tagTableModel_->index(rowCount, 0, QModelIndex());
        //Set the Tag ID received back from the database
        tagTableModel_->setData(index, tagID, Qt::EditRole);
        index = tagTableModel_->index(rowCount, 1, QModelIndex());
        tagTableModel_->setData(index, dataSet_.getProtocol(), Qt::EditRole);
        index = tagTableModel_->index(rowCount, 2, QModelIndex());
        tagTableModel_->setData(index, dataSet_.getTagName(), Qt::EditRole);
        index = tagTableModel_->index(rowCount, 3, QModelIndex());
        tagTableModel_->setData(index, dataSet_.getTagDesc(), Qt::EditRole);
        index = tagTableModel_->index(rowCount, 4, QModelIndex());
        tagTableModel_->setData(index, dataSet_.getTagLength(), Qt::EditRole);
        index = tagTableModel_->index(rowCount, 5, QModelIndex());
        tagTableModel_->setData(index, dataSet_.getTagType(), Qt::EditRole);
        index = tagTableModel_->index(rowCount, 6, QModelIndex());
        tagTableModel_->setData(index, dataSet_.getTagDefValue(), Qt::EditRole);

    }

}


void OgTagPage :: existingTag()
{
    ExistingTagDialog existingTagDialog(this);
    if(existingTagDialog.exec())
    {
        //pLineProtocol_->setText(existingMessageDialog.getProtcolName());

        //TODO: Filter the view based on the protocol selected in the dialog box
        // Add the new message in the database and reflect the view too
    }
}

void OgTagPage::updateTag(const QModelIndex index)
{
    NewTagDialog updateTagDialog(this);

    int row = index.row();
    QModelIndex dataIndex;

    dataSet_.clearData();
    dataSet_    = tagData_.value(row);

    updateTagDialog.createButton->setText(QObject::tr("Update"));
    updateTagDialog.setWindowTitle(QObject::tr("Update Tag"));

    //Disable the combo box for the protocol table
    updateTagDialog.protocolComboBox->setItemText(0, dataSet_.getProtocol());
    updateTagDialog.protocolComboBox->setDisabled(true);
    updateTagDialog.tagComboBox->setDisabled(true);

    int Generic = BOTH;

    //User has come from message page (Viewing tags for a particular message)
    if((! protocolID_.isEmpty()) && (! messageID_.isEmpty()))
    {
        updateTagDialog.generalGroupBox->setDisabled(true);
        updateTagDialog.messageBasedGroupBox->setEnabled(true);
        Generic = NONGENERIC;
    }
    //User has come from protocol page (Viewing tags for a particular protocol)
    else if((! protocolID_.isEmpty()) && (messageID_.isEmpty()))
    {
        updateTagDialog.messageBasedGroupBox->setDisabled(true);
        updateTagDialog.generalGroupBox->setEnabled(true);
        Generic = GENERIC;
    }
    //User has invoked the left pane button (Viewing tags for all protocol)
    else if((protocolID_.isEmpty()) && (messageID_.isEmpty()))
    {
        updateTagDialog.messageBasedGroupBox->setDisabled(true);
        updateTagDialog.generalGroupBox->setEnabled(true);
        Generic = GENERIC;
    }

    if(! index.isValid())
    {
        QMessageBox msgBox;
        msgBox.information(this,"Error",QString("Invalid tag index"), QMessageBox::Ok);
    }



    updateTagDialog.setProtocolName(dataSet_.getProtocol());
    updateTagDialog.setTagName(dataSet_.getTagName());
    updateTagDialog.setTagDesc(dataSet_.getTagDesc());
    updateTagDialog.setDestDatatype(dataSet_.getDestDatatype());
    updateTagDialog.setTagType(dataSet_.getTagType());
    updateTagDialog.setTagLength(dataSet_.getTagLength());
    updateTagDialog.setJustifyWith(dataSet_.getJustifyWith());
    updateTagDialog.setTagEditable(dataSet_.getTagEditable());
    updateTagDialog.setTagReq(dataSet_.getTagReq());
    updateTagDialog.setTagOffset(dataSet_.getTagOffset());
    updateTagDialog.setTagSeq(dataSet_.getTagSeq());
    updateTagDialog.setTagDefValue(dataSet_.getTagDefValue());
    //Update possible values too

    if( updateTagDialog.exec() )
    {
        //Can make this based on the generic / non generic feature availability
        QString tagName         = updateTagDialog.getTagName();
        QString desc            = updateTagDialog.getTagDesc();
        QString des_type        = updateTagDialog.getDestDatatype();
        QString type            = updateTagDialog.getTagType();
        QString length          = updateTagDialog.getTagLength();
        QString justify         = updateTagDialog.getJustifyWith();
        QString editable        = updateTagDialog.getTagEditable();
        QString req             = updateTagDialog.getTagReq();
        QString offset          = updateTagDialog.getTagOffset();
        QString seq             = updateTagDialog.getTagSeq();
        QString def             = updateTagDialog.getTagDefValue();


        if(Generic == GENERIC)
        {
            //Protocol ID is the same as in the dataSet since we dont give the user the ability to select a diff protocol
            dataSet_.setTagName(tagName);
            dataSet_.setTagDesc(desc);
            dataSet_.setDestDatatype(des_type);
            dataSet_.setTagType(type);
            dataSet_.setTagLength(length);
            dataSet_.setJustifyWith(justify);
            dataSet_.setTagEditable(editable);
        }
        else if(Generic == NONGENERIC)
        {
            //Protocol ID is the same as in the dataSet since we dont give the user the ability to select a diff protocol
            //Message ID is also present in the dataSet
            dataSet_.setTagReq(req);
            dataSet_.setTagOffset(offset);
            dataSet_.setTagSeq(seq);
            dataSet_.setTagDefValue(def);
        }

        //Create the update message req and send it to the database
        //TODO: Handle the error condition here

        //Send the request
        dSet_->clear();
        editTagRequest(dataSet_, Generic);
        client_->setRequest(dSet_);
        client_->sendRequest();

        if(! editTagResponse(client_->getRequest()))
        {
            QMessageBox msg;
            msg.information(this, "Error", "Error occured while updating data");

            return;
        }

        //If Successful update the model

        if(Generic == GENERIC)
        {
            dataIndex = tagTableModel_->index(row, TAGNAME, QModelIndex());
            tagTableModel_->setData(dataIndex, tagName, Qt::EditRole);
            dataIndex = tagTableModel_->index(row, DESCRIPTION, QModelIndex());
            tagTableModel_->setData(dataIndex, desc, Qt::EditRole);
            dataIndex = tagTableModel_->index(row, LENGTH, QModelIndex());
            tagTableModel_->setData(dataIndex, length, Qt::EditRole);
            dataIndex = tagTableModel_->index(row, TYPE, QModelIndex());
            tagTableModel_->setData(dataIndex, type, Qt::EditRole);

            tagTableModel_->setTagDestDatatype(row, des_type);
            tagTableModel_->setTagJustifyWith(row, justify);
            tagTableModel_->setTagEditable(row, editable);
        }
        else if (Generic == NONGENERIC)
        {
            dataIndex = tagTableModel_->index(row, VALUE, QModelIndex());
            tagTableModel_->setData(dataIndex, def, Qt::EditRole);

            tagTableModel_->setTagRequired(row, req);
            tagTableModel_->setTagOffset(row, offset);
            tagTableModel_->setTagSequence(row, seq);
        }
    }
}

void OgTagPage::deleteTag(const QModelIndex index)
{
    if(! index.isValid())
    {
        QMessageBox msgBox;
        msgBox.information(this,"Error",QString("Invalid message index"), QMessageBox::Ok);
        return;
    }

    int row = index.row();
    dataSet_ = tagData_.value(row);

    int retValue = QMessageBox::critical(this, QObject::tr("Critical"), QObject::tr("Do you want to delete the tag"),
                                         QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);

    if( QMessageBox::Ok == retValue )
    {

        //Send the request
        dSet_->clear();

        //Create the delete tag req and send it to the database
        //TODO: Handle the error condition here
        //Need to handle the error condition here.

        //User has come to the tag page fromleft pane, need to delete the tag from protocol and messages containing the tags
        if( (protocolID_.isEmpty()) && (messageID_.isEmpty()) )
        {
            delTagRequest("ALL", dataSet_.getTagID());
        }
        //User has come to the tag page from message page, need to delete the tag from only the message
        else if( (! protocolID_.isEmpty()) && (! messageID_.isEmpty()) )
        {
            delTagRequest(dataSet_.getProtocolMsgTagId(),dataSet_.getTagID());
        }
        //User has come to the tag page from protocol page, need to delete the tag from protocol and messages containing the tags
        else if ( !protocolID_.isEmpty() )
        {
            delTagRequest("ALL",dataSet_.getTagID());
        }

        client_->setRequest(dSet_);
        client_->sendRequest();

        //The response is populated in the variable of class OgClient, fetch it
        if (! delTagResponse(client_->getRequest()))
        {
            QMessageBox msg;
            msg.information(this, "Error", "Error occured while deleting data");

            return;
        }

        tagTableModel_->removeRows(row, 1, QModelIndex());

    }
}



bool OgTagPage :: getAllTagForProtocol(QString pId)
{
    //Create get all tags for a particular protocol req and send it to the database

    //Fetch the request
    dSet_->clear();

    getAllTagForProtocolRequest(pId);
    //Set the request to the client class
    client_->setRequest(dSet_);
    client_->sendRequest();

    //The response is populated in the variable of class OgClient, fetch it
    if (! getAllTagForProtocolResponse(client_->getRequest()))
    {
        QMessageBox msg;
        msg.information(this, "Error", "Error occured while fetching data");
        return false;
    }
}

bool OgTagPage :: getAllTagForMessage(QString pId, QString mId)
{
    //Create get all tags for a particular protocol req and send it to the database

    //Fetch the request
    dSet_->clear();

    getAllTagForMessageRequest(pId, mId);
    //Set the request to the client class
    client_->setRequest(dSet_);
    client_->sendRequest();

    //The response is populated in the variable of class OgClient, fetch it
    if (! getAllTagForMessageResponse(client_->getRequest()))
    {
        QMessageBox msg;
        msg.information(this, "Error", "Error occured while fetching data");
        return false;
    }
}

void OgTagPage::getTagRequest()
{
    std::string request = "REQUEST";
    std::string args    = "ARGS";
    std::string reqid   = "REQUESTID";
    std::string method  = "METHOD";
    std::string protocol= "PROTOCOLID";
    std::string message = "MESSAGEID";

    dSet_->add<int>(request, reqid, 101 );
    dSet_->add<std::string>(request, method, "getTag");

    //Add the args section
    PropertiesPtr propPtr(new Properties());
    dSet_->add<PropertiesPtr>(request, args, propPtr);

    LOG_DEBUG("Created the args section");

    dSet_->add<std::string>(args, protocol, "ALL");
    dSet_->add<std::string>(args, message, "ALL");
}

bool OgTagPage :: getTagResponse(DataSet<TextSerializer> *dSet)
{
    TagDataset dataset;

    //Add the first empty row
    dataset.clearData();
    tagData_.clear();
    tagData_.append(dataset);

    std::string response= "REQUEST";
    std::string reqid   = "REQUESTID";
    std::string result  = "RESULTSET";
    std::string status  = "STATUS";
    std::string statMessage = "STATMESSAGE";

    //Check whether the request was successful or not
    std::string* stat = dSet->get<std::string>(response, status);
    if( ! stat)
    {
        LOG_ERROR("The request was not successfully executed");
        std::string* statMes = dSet->get<std::string>(response, statMessage);
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
        LOG_ERROR("RESULTSET not found in the response, so no : of records is zero");
        tagData_.clear();
        pRowButton_->hide();
        return true;
    }

    StringsPtr* record;

    for(Properties::iterator pIter = (*resultSet)->begin(); pIter != (*resultSet)->end();pIter++)
    {
         record = boost::any_cast<StringsPtr>(&(*pIter).second);

         dataset.clearData();

         for(Strings::iterator iter = (*record)->begin(); iter != (*record)->end(); iter++)
         {
            //Extract values to dataset
            dataset.setProtocolId(QString::fromStdString(*iter++));
            dataset.setTagID(QString::fromStdString(*iter++));
            dataset.setProtocol(QString::fromStdString(*iter++));
            dataset.setTagName(QString::fromStdString(*iter++));
            dataset.setTagDesc(QString::fromStdString(*iter++));
            dataset.setTagLength(QString::fromStdString(*iter++));
            dataset.setTagType(QString::fromStdString(*iter));
           // dataset.setDestDatatype(QString::fromStdString(*iter++));
           // dataset.setJustifyWith(QString::fromStdString(*iter++));
            //dataset.setTagEditable(QString::fromStdString(*iter++));
         }

        tagData_.append(dataset);
    }

    //Add the last empty row
    dataset.clearData();
    tagData_.append(dataset);

    LOG_DEBUG("Got PropertiesPtr");

    return true;

}

void OgTagPage :: getAllTagForProtocolRequest(QString pId)
{
    std::string request = "REQUEST";
    std::string args    = "ARGS";
    std::string reqid   = "REQUESTID";
    std::string method  = "METHOD";
    std::string protocol= "PROTOCOLID";
    std::string message = "MESSAGEID";

    dSet_->add<int>(request, reqid, 101 );
    dSet_->add<std::string>(request, method, "getTag");

    //Add the args section
    PropertiesPtr propPtr(new Properties());
    dSet_->add<PropertiesPtr>(request, args, propPtr);

    LOG_DEBUG("Created the args section");

    dSet_->add<std::string>(args, protocol, pId.toStdString());
    dSet_->add<std::string>(args, message, "ALL");
}

/* This response is quite similar to getTagResponse */
bool OgTagPage :: getAllTagForProtocolResponse(DataSet<TextSerializer> *dSet)
{
    return getTagResponse(dSet);
}

void OgTagPage :: getAllTagForMessageRequest(QString pId, QString mId)
{
    std::string request = "REQUEST";
    std::string args    = "ARGS";
    std::string reqid   = "REQUESTID";
    std::string method  = "METHOD";
    std::string protocol= "PROTOCOLID";
    std::string message = "MESSAGEID";

    dSet_->add<int>(request, reqid, 101 );
    dSet_->add<std::string>(request, method, "getTag");

    //Add the args section
    PropertiesPtr propPtr(new Properties());
    dSet_->add<PropertiesPtr>(request, args, propPtr);

    LOG_DEBUG("Created the args section");

    dSet_->add<std::string>(args, protocol, pId.toStdString());
    dSet_->add<std::string>(args, message, mId.toStdString());
}

bool OgTagPage :: getAllTagForMessageResponse(DataSet<TextSerializer> *dSet)
{
    TagDataset dataset;

    //Add the first empty row
    dataset.clearData();
    tagData_.clear();
    tagData_.append(dataset);

    std::string response= "REQUEST";
    std::string reqid   = "REQUESTID";
    std::string result  = "RESULTSET";
    std::string status  = "STATUS";
    std::string statMessage = "STATMESSAGE";

    //Check whether the request was successful or not
    std::string* stat = dSet->get<std::string>(response, status);
    if(! stat )
    {
        LOG_ERROR("The request was not successfully executed");
        std::string* statMes = dSet->get<std::string>(response, statMessage);
        if(statMes)
            LOG_ERROR(*statMes);
        return false;
    }
    else
    {
        if( 0 == stat->compare("false") )
        {
            LOG_ERROR("The request was not successfully executed");
            std::string* statMes = dSet->get<std::string>(response, statMessage);
            if(statMes)
                LOG_ERROR(*statMes);
            return false;
        }
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
        LOG_ERROR("RESULTSET not found in the response, so no : of records is zero");
        tagData_.clear();
        pRowButton_->hide();
        return true;
    }

    StringsPtr* record;

    for(Properties::iterator pIter = (*resultSet)->begin(); pIter != (*resultSet)->end();pIter++)
    {
         record = boost::any_cast<StringsPtr>(&(*pIter).second);

         dataset.clearData();

         for(Strings::iterator iter = (*record)->begin(); iter != (*record)->end(); iter++)
         {
            //Extract values to dataset
            dataset.setProtocolId(QString::fromStdString(*iter++));
            dataset.setTagID(QString::fromStdString(*iter++));
            dataset.setProtocol(QString::fromStdString(*iter++));
            dataset.setTagName(QString::fromStdString(*iter++));
            dataset.setTagDesc(QString::fromStdString(*iter++));
            dataset.setTagLength(QString::fromStdString((*iter++)));
            dataset.setTagType(QString::fromStdString((*iter++)));
            //dataset.setDestDatatype(QString::fromStdString(*iter++));
            //dataset.setJustifyWith(QString::fromStdString(*iter++));
            //dataset.setTagEditable(QString::fromStdString(*iter++));

            dataset.setMessageId(QString::fromStdString(*iter++));
            dataset.setMessageName(QString::fromStdString((*iter++)));
            dataset.setProtocolMsgTagId(QString::fromStdString((*iter++)));
            dataset.setTagOffset(QString::fromStdString(*iter++));
            dataset.setTagReq(QString::fromStdString(*iter++));
            dataset.setTagDefValue(QString::fromStdString(*iter++));
            dataset.setTagSeq(QString::fromStdString(*iter));

         }

        tagData_.append(dataset);
    }

    //Add the last empty row
    dataset.clearData();
    tagData_.append(dataset);

    LOG_DEBUG("Got PropertiesPtr");

    //tagTableModel_->setTagDataset(tagData_);

    return true;
}

//From message page => tag page => new tag => protocolID and messageID available & (NONGENERIC properties shown)
//From protocol page => tag page => new tag => protocolID available & (GENERIC properties shown)
//From left pane  => tag page => new tag => protocolID not available - so query the
//protocol id with the help of the protocol name & (GENERIC properties shown)

void OgTagPage::addTagRequest(TagDataset tagData, int genericity)
{
    std::string request = "REQUEST";
    std::string args    = "ARGS";
    std::string reqid   = "REQUESTID";
    std::string method  = "METHOD";
    std::string generic = "GENERIC";
    std::string nongeneric  = "NONGENERIC";
    std::string state       = "STATE";
    std::string id          = "PROTOCOLID";
    std::string tag         = "TAG";
    std::string desc        = "DESCRIPTION";
    std::string des_type    = "DEST_DATATYPE";
    std::string tag_type    = "TAGTYPE";
    std::string length      = "TAGLENGTH";
    std::string justify     = "JUSTIFYWITH";
    std::string editable    = "EDITABLE";
    std::string req         = "REQUIRED";
    std::string offset      = "OFFSET";
    std::string seq         = "SEQNO";
    std::string def         = "DEFVALUE";
    std::string created     = "CREATED";
    std::string protocolMsgTagId    = "PROTOCOLMSGTAGID";


    dSet_->add<int>(request, reqid, 101 );
    dSet_->add<std::string>(request, method, "addNewTag");

    //Included this to know whether only generic or non- generic or both generic and nongeneric prop are passed
    if(genericity == GENERIC)
        dSet_->add<std::string>(request, state, generic);
    else if (genericity == NONGENERIC)
        dSet_->add<std::string>(request, state, nongeneric);
    else if (genericity == BOTH)
        dSet_->add<std::string>(request, state, "BOTH");


    //Not adding the args section

    //PropertiesPtr propPtr( new Properties() );
    //dSet_->add<PropertiesPtr>(request, args, propPtr);

    //////////////////TODO: Check whether textserializer supports nesting of sections

    if( (genericity == GENERIC) || (genericity == BOTH) )
    {
        PropertiesPtr genPropPtr( new Properties() );
        dSet_->add<PropertiesPtr>(request, generic, genPropPtr);

        LOG_DEBUG("Created the generic section");

        dSet_->add<std::string>(generic, id, tagData.getProtocolId().toStdString());
        dSet_->add<std::string>(generic, tag, tagData.getTagName().toStdString());
        dSet_->add<std::string>(generic, desc, tagData.getTagDesc().toStdString());
        //dSet_->add<std::string>(generic, des_type, tagData.getDestDatatype().toStdString());
        dSet_->add<std::string>(generic, tag_type, tagData.getTagType().toStdString());
        dSet_->add<std::string>(generic, length, tagData.getTagLength().toStdString());
        //dSet_->add<std::string>(generic, justify, tagData.getJustifyWith().toStdString());
        //dSet_->add<std::string>(generic, editable, tagData.getTagEditable().toStdString());
        dSet_->add<std::string>(generic, created, "user1");

    }

    if( (genericity == NONGENERIC) || (genericity == BOTH) )
    {       
        PropertiesPtr nonGenPropPtr( new Properties());
        dSet_->add<PropertiesPtr>(request, nongeneric, nonGenPropPtr);

        LOG_DEBUG("Created the non-generic section");

        dSet_->add<std::string>(nongeneric, protocolMsgTagId, tagData.getProtocolMsgTagId().toStdString());
        dSet_->add<std::string>(nongeneric, req, tagData.getTagReq().toStdString());
        dSet_->add<std::string>(nongeneric, offset, tagData.getTagOffset().toStdString());
        dSet_->add<std::string>(nongeneric, seq, tagData.getTagSeq().toStdString());
        dSet_->add<std::string>(nongeneric, def, tagData.getTagDefValue().toStdString());
        dSet_->add<std::string>(nongeneric, created, "user1");
        //Have to set possible values
    }
}

bool OgTagPage :: addTagResponse(DataSet<TextSerializer> *dSet, QString &tagId)
{
    std::string response    = "REQUEST";
    std::string reqid       = "REQUESTID";
    std::string tagID       = "TAGID";
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
    int* temp = dSet->get<int>(RESULT, tagID);

    if(NULL == temp)
    {
        LOG_DEBUG("TAGID not found in the response");
        return false;
    }
    tagId = QString::fromUtf16((ushort*) temp);

    return true;
}

void OgTagPage::delTagRequest(QString protMsgId, QString tagId)
{
    std::string request = "REQUEST";
    std::string args    = "ARGS";
    std::string reqid   = "REQUESTID";
    std::string method  = "METHOD";
    std::string protMsgTagId   = "PROTOCOLMSGTAGID";
    std::string tagid       = "TAGID";

    dSet_->add<int>(request, reqid, 101 );
    dSet_->add<std::string>(request, method, "delTag");

    //Add the args section
    PropertiesPtr propPtr(new Properties());
    dSet_->add<PropertiesPtr>(request, args, propPtr);

    LOG_DEBUG("Created the args section");

    //messageId not empty if tag is deleted from the message, in that case MESSAGEID is also included
    //else only TAGID is sent

    dSet_->add<std::string>(args, tagid, tagId.toStdString());

    dSet_->add<std::string>(args, protMsgTagId, protMsgId.toStdString());
}

bool OgTagPage  :: delTagResponse(DataSet<TextSerializer> *dSet)
{
    std::string response= "REQUEST";
    std::string reqid   = "REQUESTID";
    std::string status      = "STATUS";
    std::string statMessage = "STATMESSAGE";

    //Check whether the request was successful or not
    std::string* stat = dSet->get<std::string>(response, status);
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
    int *reqID   = dSet->get<int>(response, reqid);
    if(NULL == reqID)
    {
        LOG_ERROR("REQUESTID not found in the response");
        return false;
    }

    return true;
}


//From message page = nongeneric
//From protocol page = generic
//From left pane = generic
void OgTagPage::editTagRequest(TagDataset tagData, int genericity)
{
    std::string request = "REQUEST";
    std::string args    = "ARGS";
    std::string reqid   = "REQUESTID";
    std::string method  = "METHOD";
    std::string id      = "TAGID";
    std::string generic     = "GENERIC";
    std::string nongeneric  = "NONGENERIC";
    std::string req         = "REQUIRED";
    std::string offset      = "OFFSET";
    std::string seq         = "SEQNO";
    std::string def         = "DEFVALUE";
    std::string tagname     = "TAG";
    std::string desc        = "DESCRIPTION";
    std::string des_type    = "DEST_DATATYPE";
    std::string type        = "TAGTYPE";
    std::string length      = "TAGLENGTH";
    std::string justify     = "JUSTIFYWITH";
    std::string editable    = "EDITABLE";
    std::string protocolMsgTagId    = "PROTOCOLMSGTAGID";


    dSet_->add<int>(request, reqid, 101 );
    dSet_->add<std::string>(request, method, "editTag");

    //Add the args section
    //PropertiesPtr propPtr(new Properties());
    //dSet_->add<PropertiesPtr>(request, args, propPtr);

    //LOG_DEBUG("Created the args section");


    if( (genericity == GENERIC) || (genericity == BOTH) )
    {
        //Add the GENERIC section
        PropertiesPtr genPropPtr(new Properties());
        dSet_->add<PropertiesPtr>(request, generic, genPropPtr);
        LOG_DEBUG("Created the GENERIC section");

        dSet_->add<std::string>(generic, id, tagData.getTagID().toStdString());
        dSet_->add<std::string>(generic, tagname, tagData.getTagName().toStdString());
        dSet_->add<std::string>(generic, desc, tagData.getTagDesc().toStdString());
        //dSet_->add<std::string>(generic, des_type, tagData.getDestDatatype().toStdString());
        dSet_->add<std::string>(generic, type, tagData.getTagType().toStdString());
        dSet_->add<std::string>(generic, length, tagData.getTagLength().toStdString());
        //dSet_->add<std::string>(generic, justify, tagData.getJustifyWith().toStdString());
        //dSet_->add<std::string>(generic, editable, tagData.getTagEditable().toStdString());
    }
    else if ( (genericity == NONGENERIC) || (genericity == BOTH) )
    {

        //Add the NONGENERIC section
        PropertiesPtr nonGenPropPtr(new Properties());
        dSet_->add<PropertiesPtr>(request, nongeneric, nonGenPropPtr);

        dSet_->add<std::string>(nongeneric, protocolMsgTagId, tagData.getProtocolMsgTagId().toStdString());
        dSet_->add<std::string>(nongeneric, req, tagData.getTagReq().toStdString());
        dSet_->add<std::string>(nongeneric, offset, tagData.getTagOffset().toStdString());
        dSet_->add<std::string>(nongeneric, seq, tagData.getTagSeq().toStdString());
        dSet_->add<std::string>(nongeneric, def, tagData.getTagDefValue().toStdString());
        //Have to set possible values
    }

}

bool OgTagPage :: editTagResponse(DataSet<TextSerializer> *dSet)
{
    std::string response= "REQUEST";
    std::string reqid   = "REQUESTID";
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

void OgTagPage::enteredImpl(const QModelIndex &index)
{
    if(index.isValid())
    {
        int column      = index.column();
        int row         = index.row();
        int rowCount    = tagData_.size();

        if( (column >= tagTableModel_->EDITICON) && (row > 0 && row < rowCount - 1) )
        {
            pUi_->tagTable->setCursor(QCursor(Qt::PointingHandCursor));
        }
        else
            pUi_->tagTable->setCursor(QCursor(Qt::ArrowCursor));
    }
}

void OgTagPage::cellClickedImpl(const QModelIndex index)
{
    if( index.isValid())
    {
        int row     = index.row();
        int column  = index.column();
        int rowCount    = tagData_.size();

        if((row > 0 && row < rowCount - 1) && column == tagTableModel_->EDITICON)
            updateTag(index);

        else if((row > 0 && row < rowCount - 1) && column == tagTableModel_->DELETEICON)
            deleteTag(index);
    }
}

QString OgTagPage :: getIDFilter()
{
    return pLineID_->text();
}

QString OgTagPage :: getProtocolFilter()
{
    return pLineProtocol_->text();
}

QString OgTagPage :: getTagFilter()
{
    return pLineTag_->text();
}

void OgTagPage :: setIDs(QString pId, QString mId)
{
    protocolID_ = pId;
    messageID_  = mId;
}

void OgTagPage :: setIDFilter(QString id)
{
    pLineID_->setText(id);
}

void OgTagPage :: setProtocolFilter(QString protocol)
{
    pLineProtocol_->setText(protocol);
}

void OgTagPage :: setTagFilter(QString message)
{
    pLineTag_->setText(message);
}

int OgTagPage :: getRowCount()
{
    //return pUi_->messageTable->rowCount();
}

int OgTagPage :: getColumnCount()
{
    //return pUi_->messageTable->columnCount();
}

void OgTagPage :: disableAddNewTagAction()
{
    if(pAddNewAction_->isEnabled())
        pAddNewAction_->setDisabled(true);
}

void OgTagPage :: enableAddNewTagAction()
{
    if(! pAddNewAction_->isEnabled())
        pAddNewAction_->setEnabled(true);
}
