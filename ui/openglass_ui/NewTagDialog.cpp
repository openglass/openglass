/*
File name   :   NewTagDialog.cpp
Desc        :   NewTagDialog Class description
Author      :   Renjini
Version     :   1.0
*/


#include "NewTagDialog.h"

NewTagDialog :: NewTagDialog(QWidget *parent) : QDialog(parent)
{
    setupUi(this);

    genericity  = GENERIC;
    dataSet_.setName("REQUEST");

    createButton->setEnabled(false);

    //Protocol has to be selected
    tagComboBox->setDisabled(true);

    connect(tagNameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(toggleCreateButtonImpl(QString)));
    connect(createButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(protocolComboBox, SIGNAL(activated(int)), this, SLOT(activateTagComboBox(int)));
    connect(tagComboBox, SIGNAL(activated(int)), this, SLOT(selectionChanged(int)));

}

NewTagDialog :: ~NewTagDialog()
{

}


bool NewTagDialog :: populateProtocolComboBox()
{
    QString protocolName;
    //Call the registered service of OgProtocolPage class to get the list of protocols

    dataSet_.clear();
    Fog::Common::CommandRouter::getInstance()->invokeMethod("AllProtocols", dataSet_);

    protocolData_ = dataSet_.get<QList <ProtocolDataset> *>("REQUEST", "PROTOCOLDATASET");

    //Populate the protocol combobox
    if(! protocolData_)
        return false;

    int rowCount = (*protocolData_)->size();
    ProtocolDataset data;

    //In protocolDataset list 0th and last element is blank (omit those)
    for(int row = 0; row < rowCount-1; row++)
    {
        data.clearData();
        data = (*protocolData_)->value(row);
        //The combo box will not have those blank fields, the 0th element will not be blank, hence (row-1)
        if(row == 0)
        {
            protocolComboBox->addItem(QObject::tr("Select a protocol"));
            continue;
        }
        protocolComboBox->addItem(data.getProtocolName());
    }

    if(! protocolID_.isEmpty())
    {
        //Get the protocol name for the corresponding protocol ID to display in the UI and in the combo box

        int rowCount = (*protocolData_)->size();

        for(int row = 1; row < rowCount - 1; row++)
        {
            if( 0 == protocolID_.compare( (*protocolData_)->value(row).getProtocolID() ) )
            {
                //protocolName = (*protocolData_)->value(row).getProtocolName();
                //protocolComboBox->setItemText(0, protocolName);
                protocolComboBox->setCurrentIndex(row);
            }
        }
    }
    return true;
}

/*
 When the protocol is selected in the protocol combo box, if the index is valid,
 activate the tag combo box. And populate it with all the tags in that protocol.
  */
void NewTagDialog :: activateTagComboBox(int index)
{
    if(index > 0)
    {
        if(! tagComboBox->isEnabled())
            tagComboBox->setEnabled(true);

        //Find out the protocol Id from the protocol list
        QString protocolId = (*protocolData_)->value(index).getProtocolID();

        populateTagComboBox(protocolId);
    }
    else if (index <= 0)
    {
        tagComboBox->setEnabled(false);
    }
}

bool NewTagDialog :: populateTagComboBox(QString pId)
{

    //Call the registered service of OgTagPage class to get the list of tags
    // dataset = REQUEST{ PROTOCOLID = "" }

    dataSet_.clear();

    if( genericity == GENERIC )
    {
        //get all tags that are not present in the protocol.
        dataSet_.add<std::string>("REQUEST", "PROTOCOLID", pId.toStdString());

        Fog::Common::CommandRouter::getInstance()->invokeMethod("AllTagsNotInTheProtocol", dataSet_);

    }
    else
    {
        //get all tags in all the protocols to list in the tag combobox
        //If the user selects a tag belonging to another protocol, then allow him to change the generic and non- generic
        //properties. Else if he selects a tag in the protocol selected, allow him to change only the non-generic properties

        Fog::Common::CommandRouter::getInstance()->invokeMethod("AllTagsInAllTheProtocols", dataSet_);

    }

    tagData_ = dataSet_.get<QList <TagDataset> *>("REQUEST", "TAGDATASET");

    //Populate the protocol combobox
    if(! tagData_)
        return false;

    int         rowCount = (*tagData_)->size();
    TagDataset  data;
    QString     itemName;

    for(int row = 0; row < rowCount-1; row++)
    {
        data.clearData();
        itemName.clear();

        data = (*tagData_)->value(row);

        if(row == 0)
        {
            tagComboBox->addItem(QObject::tr("New tag"));
            continue;
        }

        //Each itemname will have the tag name and the protocol name
        itemName.append(data.getProtocol());
        itemName.append(" = ");
        itemName.append(data.getTagName());

        tagComboBox->addItem(itemName);
    }

    return true;

}

void NewTagDialog :: setProtocolID(QString pID)
{
    protocolID_ = pID;
}

void NewTagDialog :: setMessageID(QString mID)
{
    messageID_ = mID;
}

void NewTagDialog :: selectionChanged(int index)
{
    //Handle any selection change in the comboBox
    tagComboBoxIndex = index;
    QList <TagDataset> tagDataset;

    tagDataset = **tagData_;


    setProtocolName(tagDataset[index].getProtocol());
    setTagName(tagDataset[index].getTagName());
    setTagDesc(tagDataset[index].getTagDesc());
    setTagLength(tagDataset[index].getTagLength());
    setTagType(tagDataset[index].getTagType());
    setDestDatatype(tagDataset[index].getDestDatatype());
    setJustifyWith(tagDataset[index].getJustifyWith());
    setTagEditable(tagDataset[index].getTagEditable());

    if(genericity == NONGENERIC)
    {
        //Check whether the user selected the tag belonging to the protocol he selected
        //If yes, then allow the user to change the non-generic properties only.
        //Because we need not add the tag again to protocol_all_tags table as it already there under a particular protocol
        if( 0 == protocolID_.compare(tagDataset[index].getProtocolId()) )
        {
            generalGroupBox->setDisabled(true);
            needBothSection = false;
        }
        else
        {
            generalGroupBox->setDisabled(false);
            needBothSection = true;
        }

        setTagReq(tagDataset[index].getTagReq());
        setTagSeq(tagDataset[index].getTagSeq());
        setTagOffset(tagDataset[index].getTagOffset());
        setTagDefValue(tagDataset[index].getTagDefValue());
    }

}

void NewTagDialog :: setGenericity(int genericity)
{
    this->genericity = genericity;
}

int NewTagDialog :: getGenericity()
{
    if(needBothSection)
        this->genericity = BOTH;

    return this->genericity;
}

/*void NewTagDialog :: setTagData(QList<TagDataset> &data)
{
    tagData_ = data;
}*/

void NewTagDialog :: setTagComboBoxIndex(int index)
{
    tagComboBoxIndex = index;
}

int NewTagDialog :: getTagComboBoxIndex()
{
    return tagComboBoxIndex;
}

void NewTagDialog :: toggleCreateButtonImpl(QString tagName)
{
    if(tagName.isEmpty())
        createButton->setEnabled(false);
    else
        createButton->setEnabled(true);
}

QString NewTagDialog::getProtocol()
{
    int index = protocolComboBox->currentIndex();

    ProtocolDataset data;

    data = (*protocolData_)->value(index);

    return data.getProtocolID();

}

QString NewTagDialog::getProtocolName()
{
    return protocolComboBox->currentText();
}

QString NewTagDialog::getTagName()
{
    return tagNameLineEdit->text();
}

QString NewTagDialog::getTagDesc()
{
    return descriptionLineEdit->text();
}

QString NewTagDialog::getDestDatatype()
{
    return destTypeLineEdit->text();
}

QString NewTagDialog::getTagType()
{
    return tagTypeLineEdit->text();
}

QString NewTagDialog::getTagLength()
{
    return lengthLineEdit->text();
}

QString NewTagDialog::getJustifyWith()
{
    return justifyLineEdit->text();
}

QString NewTagDialog::getTagEditable()
{
    QString editable;
    if( editableYesRButton->isChecked() )
    {
        editable = "Y";
    }
    else
    {
        editable = "N";
    }

    return editable;
}

QString NewTagDialog::getTagReq()
{
    QString tagReq;
    if ( reqYesRButton->isChecked() && ! reqNoRButton->isChecked())
    {
        tagReq = "Y";
    }
    else if ( ! reqYesRButton->isChecked() && reqNoRButton->isChecked())
    {
        tagReq = "N";
    }

    return tagReq;
}

QString NewTagDialog::getTagOffset()
{
    return offsetLineEdit->text();
}

QString NewTagDialog::getTagDefValue()
{
    return defaultValLineEdit->text();
}

QString NewTagDialog::getTagSeq()
{
    return seqNumberLineEdit->text();
}

void NewTagDialog::setProtocolName(QString protocol)
{
    int index =  protocolComboBox->findText(protocol, Qt::MatchExactly);

    if( -1 == index )
        index = 0;
    protocolComboBox->setCurrentIndex(index);
}

void NewTagDialog::setTagName(QString name)
{
    tagNameLineEdit->setText(name);
}

void NewTagDialog::setTagDesc(QString description)
{
    descriptionLineEdit->setText(description);
}

void NewTagDialog::setDestDatatype(QString datatype)
{
    destTypeLineEdit->setText(datatype);
}

void NewTagDialog::setTagType(QString type)
{
    tagTypeLineEdit->setText(type);
}

void NewTagDialog::setTagLength(QString len)
{
    lengthLineEdit->setText(len);
}

void NewTagDialog::setJustifyWith(QString justify)
{
    justifyLineEdit->setText(justify);
}

void NewTagDialog::setTagEditable(QString edit)
{
    if(edit.compare("Y", Qt::CaseInsensitive))
    {
        editableYesRButton->setChecked(true);
        editableNoRButton->setChecked(false);
    }
    else if(edit.compare("N", Qt::CaseInsensitive))
    {
        editableNoRButton->setChecked(true);
        editableYesRButton->setChecked(false);

    }
}

void NewTagDialog::setTagReq(QString req)
{
    if( 0 == req.compare("Y", Qt::CaseInsensitive))
    {
        reqYesRButton->setChecked(true);
        reqNoRButton->setChecked(false);
    }
    else if( 0 == req.compare("N", Qt::CaseInsensitive))
    {
        reqNoRButton->setChecked(true);
        reqYesRButton->setChecked(false);
    }
}

void NewTagDialog::setTagOffset(QString offset)
{
    offsetLineEdit->setText(offset);
}

void NewTagDialog::setTagSeq(QString seq)
{
    seqNumberLineEdit->setText(seq);
}

void NewTagDialog::setTagDefValue(QString value)
{
    defaultValLineEdit->setText(value);
}
