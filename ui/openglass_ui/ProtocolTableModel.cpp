/*
File name   :   ProtocolTableModel.cpp
Desc        :   ProtocolTableModel Class declaration
Author      :   Renjini
Version     :   1.0
*/

#include "ProtocolTableModel.h"
#include "ProtocolDataset.h"


ProtocolTableModel::ProtocolTableModel(QList <ProtocolDataset> &protData,QObject *parent) : QAbstractTableModel(parent), protocolDataset_(protData)
{

}

ProtocolTableModel::~ProtocolTableModel()
{

}

int ProtocolTableModel::rowCount(const QModelIndex &parent) const
{
    return protocolDataset_.count();
}

int ProtocolTableModel::columnCount(const QModelIndex &parent) const
{
    ProtocolDataset temp;

    /*if(! protocolDataset_.empty() )
    {
        temp = protocolDataset_.at(0);
        return temp.getColumnCount();
    }*/

    return temp.getColumnCount();
}

QVariant ProtocolTableModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int column = index.column();

    if (!index.isValid())
        return QVariant();

    //Set the alignment
    if (role == Qt::TextAlignmentRole)
    {
        if(column >= MESSAGELINK)
            return int(Qt::AlignHCenter | Qt::AlignVCenter);

        else if(column == PROTOCOLID || column == PROTOCOLVERSION)
            return int(Qt::AlignRight | Qt::AlignVCenter);
    }

    //Set the data in the rows (excluding the first and last row)
    else if (role == Qt::DisplayRole)
    {
        QString item = itemAt(row,column);
        return item;
    }

    //Underline the "messages" and "tags" labels in the rows (excluding the first and last row)
    else if (role == Qt::FontRole)
    {
        if(column == MESSAGELINK || column == TAGLINK)
        {
            QFont font;
            font.setUnderline(true);

            return font;
        }
    }
    /*else if (role == Qt::ForegroundRole)
    {
        if(column == MESSAGELINK)
        {
            return QColor(Qt::red);
        }
    }
*/
    //Set Icons in the rows (excluding the first and last row)
    else if( role == Qt::DecorationRole )
    {
        int rowCount        = protocolDataset_.size();
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

bool ProtocolTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.isValid() && role == Qt::EditRole)
    {
        int row = index.row();
        int col = index.column();

        ProtocolDataset dataSet = protocolDataset_.value(row);

        if(col == PROTOCOLID)
            dataSet.setProtocolID(value.toString());
        else if(col == PROTOCOLNAME)
            dataSet.setProtocolName(value.toString());
        else if(col == PROTOCOLVERSION)
            dataSet.setProtocolVersion(value.toString());
        else if(col == PROTOCOLTYPE)
            dataSet.setProtocolType(value.toString());

        protocolDataset_.replace(row, dataSet);

        Q_EMIT dataChanged(index,index);
        return true;
    }
    else if(index.isValid() && role == Qt::ForegroundRole)
    {
        data(index, role);
        Q_EMIT dataChanged(index,index);
        return true;
    }

    return false;
}

Qt::ItemFlags ProtocolTableModel::flags(const QModelIndex &index) const
 {
     if (!index.isValid())
         return Qt::ItemIsEnabled;

     return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
 }

bool ProtocolTableModel::insertRows(int position, int rows, const QModelIndex &index)
{
    ProtocolDataset dataSet;

    beginInsertRows(QModelIndex(), position, position+rows-1);

    for(int row=0; row<rows; row++)
        protocolDataset_.insert(position, dataSet);

    endInsertRows();
    return true;
}

bool ProtocolTableModel::removeRows(int position, int rows, const QModelIndex &index)
{
    beginRemoveRows(QModelIndex(), position,  position+rows-1);

    for(int row=0; row<rows; row++)
        protocolDataset_.removeAt(position);

    endRemoveRows();
    return true;
}

void ProtocolTableModel::setProtocolDataset(const QList<ProtocolDataset> &dataSet)
{
    protocolDataset_ = dataSet;
    //reset();
}

void ProtocolTableModel::setProtocolTableHeader(const QStringList &header)
{
    protocolTableHeader_ = header;
}

QString ProtocolTableModel::itemAt(int row, int column) const
{
    ProtocolDataset dataSetRecord;
    int rowCount        = protocolDataset_.size();

    /*
      Check the row numbers before returning values to be populated in the table.
      The first row of the table contains the filter LineEdits
      The last row should contain the menu button
      Valid data is returned for all the other rows excluding the above two.
    */
    if(row >= 1 && row < (rowCount - 1))
    {
        dataSetRecord = protocolDataset_.at(row);

        if(column == PROTOCOLID)
            return dataSetRecord.getProtocolID();
        else if(column == PROTOCOLNAME)
            return dataSetRecord.getProtocolName();
        else if(column == PROTOCOLVERSION)
            return dataSetRecord.getProtocolVersion();
        else if(column == MESSAGELINK)
            return "messages";              //message label
        else if(column == TAGLINK)
            return "tags";                  //tags label
    }

    return "";
}

QString ProtocolTableModel :: headerAt(int section) const
{
    /*
      The end 4 columns does not have header labels
      Return empty string for those
    */
    if( section > PROTOCOLVERSION )
        return " ";
    else
        return protocolTableHeader_.at(section);
}

QVariant ProtocolTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{

    if( orientation == Qt::Horizontal && role == Qt::DisplayRole )
    {
        return headerAt(section);
    }

    return QVariant();
}

int ProtocolTableModel::rowCounter()
{
   return protocolDataset_.count();
}

void ProtocolTableModel::setProtocolType(int row, QString type)
{
    ProtocolDataset temp;

    temp = protocolDataset_.value(row);
    temp.setProtocolType(type);

    protocolDataset_.replace(row, temp);
}
