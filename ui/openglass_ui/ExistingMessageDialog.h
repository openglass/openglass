#ifndef EXISTINGMESSAGEDIALOG_H
#define EXISTINGMESSAGEDIALOG_H

/*
File name   :   ExistingMessageDialog.h
Desc        :   ExistingMessageDialog Class declaration
Author      :   Renjini
Version     :   1.0
*/


#include <QtGui/QMainWindow>
#include <QtGui>
//#include "qobject.h"

#include "ui_ExistingMessageDialog.h"


class ExistingMessageDialog : public QDialog, public Ui::ExistingMessageDialog
{
    Q_OBJECT

    public:
    ExistingMessageDialog(QWidget *parent);
    ~ExistingMessageDialog();

    QString getProtcolName();   //gets the protocol name
    QString getMessageName();   //gets the message name selected
    QString getMessageDesc();   //gets the message descr selected

    private:
    void setMessageTable();     //create the table
    void setMessageTableData(); //set the data into the table

    private Q_SLOTS:
    void toggleAddButtonImpl(int,int);  //enables ADD button only when a message is selected

};


#endif // EXISTINGMESSAGEDIALOG_H
