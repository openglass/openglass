/*
File name   :   NewProtocolDialog.cpp
Desc        :   NewProtocolDialog Class description
Author      :   Renjini
Version     :   1.0
*/


#include "NewProtocolDialog.h"

NewProtocolDialog :: NewProtocolDialog(QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    createButton->setEnabled(false);

    //constraints for lineedits
    protocolLineEdit->setMaxLength(49);
    versionLineEdit->setMaxLength(10);
    typeLineEdit->setMaxLength(20);

    regExpName.setPattern("^[^0123456789]{1,1}[^&,%]{1,48}");
    regExpName.setCaseSensitivity(Qt::CaseInsensitive);
    nameValidator   = new QRegExpValidator(regExpName, this);
    protocolLineEdit->setValidator(nameValidator);

    regExpType.setPattern("^[A-Z]{1,20}$");
    regExpType.setCaseSensitivity(Qt::CaseInsensitive);
    typeValidator   = new QRegExpValidator(regExpType, this);
    typeLineEdit->setValidator(typeValidator);

    regExpVersion.setPattern("^[0-9]{1,10}[.][0-9]{1,8}[.][0-9]{1,6}[.][0-9]{1,4}[.][0-9]{1,2}[.][0-9]$");
    regExpVersion.setCaseSensitivity(Qt::CaseInsensitive);
    versionValidator   = new QRegExpValidator(regExpVersion, this);
    versionLineEdit->setValidator(versionValidator);

    connect(protocolLineEdit, SIGNAL(textChanged(QString)), this, SLOT(enableCreateButtonForProtocol(QString)));
    connect(typeLineEdit, SIGNAL(textChanged(QString)), this, SLOT(enableCreateButtonForType(QString)));
    connect(versionLineEdit, SIGNAL(textChanged(QString)), this, SLOT(enableCreateButtonForVersion(QString)));
    connect(createButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

NewProtocolDialog :: ~NewProtocolDialog()
{

}

QString NewProtocolDialog :: getProtocolName()
{
    return protocolLineEdit->text();
}

QString NewProtocolDialog :: getProtocolDesc()
{
    return descriptionLineEdit->text();
}

QString NewProtocolDialog :: getDetailedDesc()
{
    return detailDescTextEdit->toPlainText();
}

QString NewProtocolDialog :: getProtocolVersion()
{
    return versionLineEdit->text();
}

QString NewProtocolDialog :: getProtocolType()
{
    return typeLineEdit->text();
}

void NewProtocolDialog :: setProtocolName(QString protocol)
{
    protocolLineEdit->setText(protocol);
}

void NewProtocolDialog :: setProtocolDesc(QString desc)
{
    descriptionLineEdit->setText(desc);
}

void NewProtocolDialog :: setDetailedDesc(QString detailed)
{
    detailDescTextEdit->setText(detailed);
}

void NewProtocolDialog :: setProtocolVersion(QString version)
{
    versionLineEdit->setText(version);
}

void NewProtocolDialog :: setProtocolType(QString type)
{
    typeLineEdit->setText(type);
}

void NewProtocolDialog :: enableCreateButtonForProtocol(QString protocol)
{
    if(protocol.isEmpty())
        createButton->setEnabled(false);
    else
    {
        if ( !(getProtocolVersion().isEmpty()) && !(getProtocolType().isEmpty()) )
            createButton->setEnabled(true);
    }
}

void NewProtocolDialog :: enableCreateButtonForType(QString type)
{
    if(type.isEmpty())
        createButton->setEnabled(false);
    else
    {
        if ( !(getProtocolVersion().isEmpty()) && !(getProtocolName().isEmpty()) )
            createButton->setEnabled(true);
    }
}

void NewProtocolDialog :: enableCreateButtonForVersion(QString version)
{
    if(version.isEmpty())
        createButton->setEnabled(false);
    else
    {
        if ( !(getProtocolName().isEmpty()) && !(getProtocolType().isEmpty()) )
            createButton->setEnabled(true);
    }
}

