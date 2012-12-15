/*
File name   :   NewMessageDialog.cpp
Desc        :   NewMessageDialog Class description
Author      :   Renjini
Version     :   1.0
*/


#include "NewMessageDialog.h"

NewMessageDialog :: NewMessageDialog(QWidget *parent) : QDialog(parent)
{
    setupUi(this);

    createButton->setEnabled(false);

    //constraints for lineedits
    messageLineEdit->setMaxLength(49);
    descriptionLineEdit->setMaxLength(199);

    regExpName.setPattern("^[^0123456789]{1,1}[^&,%]{1,48}");
    regExpName.setCaseSensitivity(Qt::CaseInsensitive);
    nameValidator   = new QRegExpValidator(regExpName, this);
    messageLineEdit->setValidator(nameValidator);

    connect(messageLineEdit, SIGNAL(textChanged(QString)), this, SLOT(toggleCreateButtonImpl(QString)));
    connect(createButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

NewMessageDialog :: ~NewMessageDialog()
{

}

QString NewMessageDialog :: getProtocolName()
{
    return protocolComboBox->currentText();
}

QString NewMessageDialog :: getMessageName()
{
    return messageLineEdit->text();
}

QString NewMessageDialog :: getMessageDesc()
{
    return descriptionLineEdit->text();
}

QString NewMessageDialog :: getDetailedDesc()
{
    return detailedTextEdit->toPlainText();
}

void NewMessageDialog :: setProtocolName(QString name)
{
    protocolComboBox->addItem(name);
}

void NewMessageDialog :: setMessageName(QString name)
{
    messageLineEdit->setText(name);
}

void NewMessageDialog :: setMessageDesc(QString description)
{
    descriptionLineEdit->setText(description);
}

void NewMessageDialog :: toggleCreateButtonImpl(QString message)
{
    if(message.isEmpty())
        createButton->setEnabled(false);
    else
        createButton->setEnabled(true);
}
