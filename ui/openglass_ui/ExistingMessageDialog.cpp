/*
File name   :   ExistingMessageDialog.cpp
Desc        :   ExistingMessageDialog Class description
Author      :   Renjini
Version     :   1.0
*/

#include "ExistingMessageDialog.h"

ExistingMessageDialog :: ExistingMessageDialog(QWidget *parent) : QDialog(parent)
{
    setupUi(this);

    addButton->setEnabled(false);

    connect(messageTable, SIGNAL(cellClicked(int,int)), this, SLOT(toggleAddButtonImpl(int,int)));
    connect(addButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

ExistingMessageDialog :: ~ExistingMessageDialog()
{

}

QString ExistingMessageDialog :: getProtcolName()
{
    return protocolComboBox->currentText();
}

QString ExistingMessageDialog :: getMessageName()
{
    QString str;
    return str;
    //TODO Get the message name from the selected row in the table
}

QString ExistingMessageDialog :: getMessageDesc()
{
    QString str;
    return str;
    //TODO Get the message descr from the selected row in the table
}

void ExistingMessageDialog :: setMessageTable()
{
   //TODO set the table for data
}

void ExistingMessageDialog :: setMessageTableData()
{
    //TODO set table data
}

void ExistingMessageDialog :: toggleAddButtonImpl(int row, int col)
{
    if( (row != 0 ) && (col != 0) )
        addButton->setEnabled(true);
    else
        addButton->setEnabled(false);
}
