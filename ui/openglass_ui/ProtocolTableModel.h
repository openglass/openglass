#ifndef PROTOCOLTABLEMODEL_H
#define PROTOCOLTABLEMODEL_H

/*
File name   :   ProtocolTableModel.h
Desc        :   ProtocolTableModel Class declaration
Author      :   Renjini
Version     :   1.0
*/


#include <QAbstractTableModel>

#include "ProtocolDataset.h"


/*
This class represents the model to be assigned to the protocol table
*/

class ProtocolTableModel : public QAbstractTableModel
{
public:
    ProtocolTableModel(QList <ProtocolDataset> &, QObject *parent = 0);
    ~ProtocolTableModel();

    //setting the data structure (input) for the model
    void setProtocolDataset(const QList<ProtocolDataset> &dataset);
    void setProtocolTableHeader(const QStringList &head);

    void setProtocolType(int row, QString type);

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
    Qt::ItemFlags flags(const QModelIndex &index) const;


    enum colName_
    {
        PROTOCOLID      = 0,
        PROTOCOLNAME    = 1,
        PROTOCOLVERSION = 2,
        MESSAGELINK     = 3,
        TAGLINK         = 4,
        EDITICON        = 5,
        DELETEICON      = 6,
        PROTOCOLTYPE    = 7
    };
private:
    QString itemAt(int row, int col) const;
    QString headerAt(int) const;
    QList <ProtocolDataset> &protocolDataset_;
    QStringList protocolTableHeader_;

};

#endif // PROTOCOLTABLEMODEL_H
