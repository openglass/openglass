/*
File name   :   TagTableModel.cpp
Desc        :   TagTableModel Class declaration
Author      :   Renjini
Version     :   1.0
*/

#include "TagTableModel.h"
#include "TagDataset.h"


TagTableModel::TagTableModel(QList <TagDataset> &data, QObject *parent) : QAbstractTableModel(parent), tagDataset_(data)
{

}

TagTableModel::~TagTableModel()
{

}

int TagTableModel::rowCount(const QModelIndex &parent) const
{
    return tagDataset_.count();
}

int TagTableModel::columnCount(const QModelIndex &parent) const
{
    TagDataset temp;

    /*if(! tagDataset_.empty() )
    {
        temp = tagDataset_.at(0);
        return temp.getColumnCount();
    }
    */
    return temp.getColumnCount();
}

QVariant TagTableModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int column = index.column();

    if (!index.isValid())
        return QVariant();

    //Set the alignment
    if (role == Qt::TextAlignmentRole)
    {
        if(column > TAGID)
            return int(Qt::AlignLeft | Qt::AlignVCenter);
        /*else if(column ==  TAGID || column == TAGLEN || column == TAGVALUE)
            return int(Qt::AlignRight | Qt::AlignVCenter);*/
    }

    //Set the data in the rows (excluding the first and last row)
    else if (role == Qt::DisplayRole)
    {
        QString item = itemAt(row,column);
        return item;
    }

    //Set Icons in the rows (excluding the first and last row)
    else if( role == Qt::DecorationRole )
    {
        int rowCount        = tagDataset_.size();
        if( row >= 1 && row < (rowCount - 1) )
        {
            if( column == DELETEICON )
                return QIcon("://images//deletered.png");
            else if ( column == EDITICON )
                return QIcon("://images//package_editors.ico");
        }
    }
    return QVariant();
}

bool TagTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.isValid() && role == Qt::EditRole)
    {
        int row = index.row();
        int col = index.column();

        TagDataset dataSet = tagDataset_.value(row);

        if(col == TAGID)
            dataSet.setTagID(value.toString());
        else if(col == PROTOCOL)
            dataSet.setProtocol(value.toString());
        else if(col == TAGNAME)
            dataSet.setTagName(value.toString());
        else if(col == TAGDESC)
            dataSet.setTagDesc(value.toString());
        else if(col == TAGLEN)
            dataSet.setTagLength(value.toString());
        else if(col == TAGTYPE)
            dataSet.setTagType(value.toString());
        else if(col == TAGVALUE)
            dataSet.setTagDefValue(value.toString());


        tagDataset_.replace(row, dataSet);

        Q_EMIT dataChanged(index,index);
        return true;
    }
    return false;
}

bool TagTableModel::insertRows(int position, int rows, const QModelIndex &index)
{
    TagDataset dataSet;

    beginInsertRows(QModelIndex(), position, position+rows-1);

    for(int row=0; row<rows; row++)
        tagDataset_.insert(position, dataSet);

    endInsertRows();
    return true;
}

bool TagTableModel::removeRows(int position, int rows, const QModelIndex &index)
{
    beginRemoveRows(QModelIndex(), position,  position+rows-1);

    for(int row=0; row<rows; row++)
        tagDataset_.removeAt(position);

    endRemoveRows();
    return true;
}


void TagTableModel::setTagDataset(const QList<TagDataset> &dataSet)
{
    tagDataset_ = dataSet;
    reset();
}

void TagTableModel::setTagTableHeader(const QStringList &header)
{
    tagTableHeader_ = header;
}

QString TagTableModel::itemAt(int row, int column) const
{
    TagDataset dataSetRecord;
    int rowCount        = tagDataset_.size();

    /*
      Check the row numbers before returning values to be populated in the table.
      The first row of the table contains the filter LineEdits
      The last row should contain the menu button
      Valid data is returned for all the other rows excluding the above two.
    */
    if(row >= 0 && row < (rowCount - 1))
    {
        dataSetRecord = tagDataset_.at(row);

        if(column == TAGID)
            return dataSetRecord.getTagID();
        else if(column == TAGNAME)
            return dataSetRecord.getTagName();
        else if(column == TAGDESC)
            return dataSetRecord.getTagDesc();
        else if(column == TAGLEN)
            return dataSetRecord.getTagLength();
        else if(column == TAGTYPE)
            return dataSetRecord.getTagType();
        else if(column == TAGVALUE)
            return dataSetRecord.getTagDefValue();
        else if(column == PROTOCOL)
            return dataSetRecord.getProtocol();
    }

    return "";
}

QString TagTableModel :: headerAt(int section) const
{
    /*
      The end 2 columns does not have header labels
      Return empty string for those
    */

    if( section > TAGVALUE )
        return " ";
    else
        return tagTableHeader_.at(section);
}

QVariant TagTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{

    if( orientation == Qt::Horizontal && role == Qt::DisplayRole )
    {
        return headerAt(section);
    }

    return QVariant();
}

int TagTableModel::rowCounter()
{
    tagDataset_.count();
}

void TagTableModel::setTagDestDatatype(int row, QString datatype)
{
    TagDataset temp;

    temp = tagDataset_.value(row);
    temp.setDestDatatype(datatype);

    tagDataset_.replace(row, temp);
}

void TagTableModel::setTagJustifyWith(int row, QString justify)
{
    TagDataset temp;

    temp = tagDataset_.value(row);
    temp.setJustifyWith(justify);

    tagDataset_.replace(row, temp);
}

void TagTableModel::setTagEditable(int row, QString edit)
{
    TagDataset temp;

    temp = tagDataset_.value(row);
    temp.setTagEditable(edit);

    tagDataset_.replace(row, temp);
}

void TagTableModel::setTagRequired(int row, QString req)
{
    TagDataset temp;

    temp = tagDataset_.value(row);
    temp.setTagReq(req);

    tagDataset_.replace(row, temp);
}

void TagTableModel::setTagOffset(int row, QString offset)
{
    TagDataset temp;

    temp = tagDataset_.value(row);
    temp.setTagOffset(offset);

    tagDataset_.replace(row, temp);
}

void TagTableModel::setTagSequence(int row, QString seq)
{
    TagDataset temp;

    temp = tagDataset_.value(row);
    temp.setTagSeq(seq);

    tagDataset_.replace(row, temp);
}

