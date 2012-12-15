/*
File name   :   ExistingTagDialog.cpp
Desc        :   ExistingTagDialog Class description
Author      :   Renjini
Version     :   1.0
*/

#include "ExistingTagDialog.h"

ExistingTagDialog :: ExistingTagDialog(QWidget *parent) : QDialog(parent)
{
    setupUi(this);

    connect(tagTable, SIGNAL(cellClicked(int,int)), this, SLOT(toggleAddButtonImpl(int,int)));
    connect(addButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

ExistingTagDialog :: ~ExistingTagDialog()
{

}

QString ExistingTagDialog :: getProtcolName()
{
    return protocolComboBox->currentText();
}

QString ExistingTagDialog :: getMessageName()
{
    QString str;
    return str;
    //TODO Get the message name from the selected row in the table
}

QString ExistingTagDialog :: getMessageDesc()
{
    QString str;
    return str;
    //TODO Get the message descr from the selected row in the table
}

void ExistingTagDialog :: setTagTable()
{
   //TODO set the table for data
}

void ExistingTagDialog :: setTagTableData()
{
    //TODO set table data
}

void ExistingTagDialog :: toggleAddButtonImpl(int row, int col)
{
    if( (row != 0 ) && (col != 0) )
        addButton->setEnabled(true);
    else
        addButton->setEnabled(false);
}
