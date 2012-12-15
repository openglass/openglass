#ifndef TAGTABLEMODEL_H
#define TAGTABLEMODEL_H


/*
File name   :   TagTableModel.h
Desc        :   TagTableModel Class declaration
Author      :   Renjini
Version     :   1.0
*/

#include <QAbstractTableModel>

#include "TagDataset.h"


/*
This class represents the model to be assigned to the tag table
*/

class TagTableModel : public QAbstractTableModel
{
public:
    TagTableModel(QList <TagDataset> &, QObject *parent = 0);
    ~TagTableModel();

    //setting the data structure (input) for the model
    void setTagDataset(const QList<TagDataset> &dataset);
    void setTagTableHeader(const QStringList &head);

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

    //Updating dataset columns that are not present in the view
    void setTagDestDatatype(int, QString);
    void setTagJustifyWith(int, QString);
    void setTagEditable(int, QString);
    void setTagRequired(int, QString);
    void setTagOffset(int, QString);
    void setTagSequence(int, QString);

    enum colName_
    {
        TAGID       = 0,
        PROTOCOL    = 1,
        TAGNAME     = 2,
        TAGDESC     = 3,
        TAGLEN      = 4,
        TAGTYPE     = 5,
        TAGVALUE    = 6,
        EDITICON    = 7,
        DELETEICON  = 8
    };
private:
    QString itemAt(int row, int col) const;
    QString headerAt(int) const;
    QList <TagDataset> &tagDataset_;
    QStringList tagTableHeader_;


};

#endif // TAGTABLEMODEL_H
