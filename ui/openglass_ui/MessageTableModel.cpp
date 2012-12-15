/*
File name   :   MessageTableModel.cpp
Desc        :   MessageTableModel Class declaration
Author      :   Renjini
Version     :   1.0
*/

#include "MessageTableModel.h"
#include "MessageDataset.h"


MessageTableModel::MessageTableModel(QList <MessageDataset> &data, QObject *parent) : QAbstractTableModel(parent), messageDataset_(data)
{

}

MessageTableModel::~MessageTableModel()
{

}

int MessageTableModel::rowCount(const QModelIndex &parent) const
{
    return messageDataset_.count();
}

int MessageTableModel::columnCount(const QModelIndex &parent) const
{
    MessageDataset temp;


    return temp.getColumnCount();
}

QVariant MessageTableModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int column = index.column();

    if (!index.isValid())
        return QVariant();

    //Set the alignment
    if (role == Qt::TextAlignmentRole)
    {
        if(column >= TAGLINK)
            return int(Qt::AlignHCenter | Qt::AlignVCenter);

        else if(column == MESSAGEID)
            return int(Qt::AlignRight | Qt::AlignVCenter);
    }

    //Set the data in the rows (excluding the first and last row)
    else if (role == Qt::DisplayRole)
    {
        QString item = itemAt(row,column);
        return item;
    }

    //Underline the "tags" labels in the rows (excluding the first and last row)
    else if (role == Qt::FontRole)
    {
        if(column == TAGLINK)
        {
            QFont font;
            font.setUnderline(true);

            return font;
        }
    }

    //Set Icons in the rows (excluding the first and last row)
    else if( role == Qt::DecorationRole )
    {
        int rowCount        = messageDataset_.size();
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

bool MessageTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.isValid() && role == Qt::EditRole)
    {
        int row = index.row();
        int col = index.column();

        MessageDataset dataSet = messageDataset_.value(row);

        if(col == MESSAGEID)
            dataSet.setMessageID(value.toString());
        else if(col == PROTOCOL)
            dataSet.setProtocol(value.toString());
        else if(col == MESSAGENAME)
            dataSet.setMessageName(value.toString());
        else if(col == MESSAGEDESC)
            dataSet.setMessageDesc(value.toString());

        messageDataset_.replace(row, dataSet);

        Q_EMIT dataChanged(index,index);
        return true;
    }
    return false;
}

bool MessageTableModel::insertRows(int position, int rows, const QModelIndex &index)
{
    MessageDataset dataSet;

    beginInsertRows(QModelIndex(), position, position+rows-1);

    for(int row=0; row<rows; row++)
        messageDataset_.insert(position, dataSet);

    endInsertRows();
    return true;
}

bool MessageTableModel::removeRows(int position, int rows, const QModelIndex &index)
{
    beginRemoveRows(QModelIndex(), position,  position+rows-1);

    for(int row=0; row<rows; row++)
        messageDataset_.removeAt(position);

    endRemoveRows();
    return true;
}


void MessageTableModel::setMessageDataset(const QList<MessageDataset> &dataSet)
{
    messageDataset_ = dataSet;
    reset();
}

void MessageTableModel::setMessageTableHeader(const QStringList &header)
{
    messageTableHeader_ = header;
}

QString MessageTableModel::itemAt(int row, int column) const
{
    MessageDataset dataSetRecord;
    int rowCount        = messageDataset_.size();

    /*
      Check the row numbers before returning values to be populated in the table.
      The first row of the table contains the filter LineEdits
      The last row should contain the menu button
      Valid data is returned for all the other rows excluding the above two.
    */
    if(row >= 1 && row < (rowCount - 1))
    {
        dataSetRecord = messageDataset_.at(row);

        if(column == MESSAGEID)
            return dataSetRecord.getMessageID();
        else if(column == MESSAGENAME)
            return dataSetRecord.getMessageName();
        else if(column == MESSAGEDESC)
            return dataSetRecord.getMessageDesc();
        else if(column == PROTOCOL)
            return dataSetRecord.getProtocol();
        else if(column == TAGLINK)
            return "tags";          //tags label
    }

    return "";
}

QString MessageTableModel :: headerAt(int section) const
{
    /*
      The end 3 columns does not have header labels
      Return empty string for those
    */

    if( section > MESSAGEDESC )
        return " ";
    else
        return messageTableHeader_.at(section);
}

QVariant MessageTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{

    if( orientation == Qt::Horizontal && role == Qt::DisplayRole )
    {
        return headerAt(section);
    }

    return QVariant();
}

int MessageTableModel::rowCounter()
{
   return messageDataset_.count();
}

