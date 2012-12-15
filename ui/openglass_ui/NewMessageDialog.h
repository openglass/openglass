#ifndef NEWMESSAGEDIALOG_H
#define NEWMESSAGEDIALOG_H

/*
File name   :   NewMessageDialog.h
Desc        :   NewMessageDialog Class declaration
Author      :   Renjini
Version     :   1.0
*/


#include <QtGui/QMainWindow>
#include <QtGui>
//#include "qobject.h"

#include "ui_NewMessageDialog.h"

class NewMessageDialog : public QDialog, public Ui::NewMessageDialog
{
    Q_OBJECT

    public:
    NewMessageDialog(QWidget *parent);
    ~NewMessageDialog();

    QString getProtocolName();  //gets the protocol name
    QString getMessageName();   //gets the message name
    QString getMessageDesc();   //gets the message descr
    QString getDetailedDesc();  //gets the detailed descr

    void setProtocolName(QString); //sets the protocol name
    void setMessageName(QString);  //sets message name
    void setMessageDesc(QString);  //sets message descr
    void setDetailedDesc(QString); //sets message detailed descr.


    private Q_SLOTS:
    void toggleCreateButtonImpl(QString);   //enables create button only if message name is not null

    private:
    QRegExp             regExpName;
    QRegExpValidator*   nameValidator;


};


#endif // NEWMESSAGEDIALOG_H
