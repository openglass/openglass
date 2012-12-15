#ifndef MESSAGETABLEMODEL_H
#define MESSAGETABLEMODEL_H

/*
File name   :   MessageTableModel.h
Desc        :   MessageTableModel Class declaration
Author      :   Renjini
Version     :   1.0
*/

#include <QAbstractTableModel>

#include "MessageDataset.h"


/*
This class represents the model to be assigned to the protocol table
*/

class MessageTableModel : public QAbstractTableModel
{
public:
    MessageTableModel(QList <MessageDataset> &,QObject *parent = 0);
    ~MessageTableModel();

    //setting the data structure (input) for the model
    void setMessageDataset(const QList<MessageDataset> &dataset);
    void setMessageTableHeader(const QStringList &head);

    //Populating the data into the model
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role) const;

    //Resizing the model
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());

    int rowCounter();
    //void resetView();

    enum colName_
    {
        MESSAGEID       = 0,
        PROTOCOL        = 1,
        MESSAGENAME     = 2,
        MESSAGEDESC     = 3,
        TAGLINK         = 4,
        EDITICON        = 5,
        DELETEICON      = 6
    };

private:
    QString itemAt(int row, int col) const;
    QString headerAt(int) const;
    QList <MessageDataset> &messageDataset_;
    QStringList messageTableHeader_;

};



#endif // MESSAGETABLEMODEL_H
