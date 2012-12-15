#ifndef EXISTINGTAGDIALOG_H
#define EXISTINGTAGDIALOG_H

/*
File name   :   ExistingTagDialog.h
Desc        :   ExistingTagDialog Class declaration
Author      :   Renjini
Version     :   1.0
*/


#include <QtGui/QMainWindow>
#include <QtGui>
//#include "qobject.h"

#include "ui_ExistingTagDialog.h"

class ExistingTagDialog : public QDialog, public Ui::ExistingTagDialog
{
    Q_OBJECT

    public:
    ExistingTagDialog(QWidget *parent);
    ~ExistingTagDialog();

    QString getProtcolName();   //gets the protocol name
    QString getMessageName();   //gets the message name selected
    QString getMessageDesc();   //gets the message descr selected

    private:
    void setTagTable();     //create the table
    void setTagTableData(); //set the data into the table

    private Q_SLOTS:
    void toggleAddButtonImpl(int,int);  //enables ADD button only when a message is selected

};

#endif // EXISTINGTAGDIALOG_H
