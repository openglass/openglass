#ifndef NEWPROTOCOLDIALOG_H
#define NEWPROTOCOLDIALOG_H

/*
File name   :   NewProtocolDialog.h
Desc        :   NewProtocolDialog Class declaration
Author      :   Renjini
Version     :   1.0
*/


#include <QtGui/QMainWindow>
#include <QtGui>
#include <QRegExp>

//#include "qobject.h"

#include "ui_NewProtocolDialog.h"

class NewProtocolDialog : public QDialog, public Ui::NewProtocolDialog
{
    Q_OBJECT

public:
    NewProtocolDialog(QWidget *parent);
    ~NewProtocolDialog();

    QString getProtocolName();   //gets the protocol name
    QString getProtocolDesc();  //gets protocol descr
    QString getDetailedDesc();  //gets detailed descr
    QString getProtocolVersion();       //gets the protocol version
    QString getProtocolType();          //Gets the protocol type

    void setProtocolName(QString);   //sets the protocol name
    void setProtocolDesc(QString);  //sets protocol descr
    void setDetailedDesc(QString);  //sets detailed descr
    void setProtocolVersion(QString);       //sets the protocol version
    void setProtocolType(QString);  //sets the protocol type

private Q_SLOTS:
    void enableCreateButtonForProtocol(QString);  //enables create button only if protocol name is not null
    void enableCreateButtonForVersion(QString);  //enables create button only if protocol version is not null
    void enableCreateButtonForType(QString);  //enables create button only if protocol type is not null

private:
    QRegExp     regExpType;
    QRegExpValidator*  typeValidator;

    QRegExp     regExpVersion;
    QRegExpValidator*  versionValidator;

    QRegExp     regExpName;
    QRegExpValidator*  nameValidator;
};

#endif // NEWPROTOCOLDIALOG_H
