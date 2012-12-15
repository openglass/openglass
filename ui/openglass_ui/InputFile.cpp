/*
File name   :   InputFile.cpp
Desc        :   InputFile Class description
                This class reads a file that stores the details of protocols/messages/tags
                The format of the file is:
                -----------------------------------------------------------
                RowNo   =    23      (Row 1 : Number of rows to be displayed)
                ColNo   =    5       (Row 2 : Number of cols to be displayed)
                ID  Protocol     Version     (Row 3 : Header list of the table)
                101 ARCA    1.1              (Row 4 : data row 1)
                101 ARCA    1.2              (Row 5 : data row 2)
                ----
                ---
                --
                ------------------------------------------------------------
Author      :   Renjini
Version     :   1.0
*/

#include "InputFile.h"

int InputFile::pos = 0;

InputFile::InputFile()
{

}

InputFile::InputFile(const QString file)
{
    fHandle_    = new QFile(file);
}

InputFile::~InputFile()
{
    if(fHandle_)
        delete fHandle_;

    if(fileData_)
        delete fileData_;
}

bool InputFile::open()
{
    if( !fHandle_ )
        return false;
    else
        fHandle_->open(QIODevice::ReadWrite | QIODevice::Text);

    fileData_   = new QTextStream(fHandle_);

    return true;
}

void InputFile::close()
{
    if (fHandle_ )
        fHandle_->close();
}

/*
The row count is in the first line of the file
Eg: RowNo   =   1
Seek to the begining of the file. Read the first line. Split the line with "tab" and populate
the stringlist. Return the row number, which is the last index in the stringlist.
*/

int InputFile::getRowCount()
{
    int rows;

    if(! fileData_)
        return 0;
    fileData_->seek(0);
    QString data = fileData_->readLine();
    QStringList dataList = data.split("\\t");

    if(dataList.isEmpty())
        return 0;

    QString str = dataList.at(dataList.size() - 1);

    return str.toInt();
}

/*
The column count is in the second line of the file
Eg: ColNo   =   1
Seek to the begining of the file. Read the second line. Split the line with "tab" and populate
the stringlist. Return the column number, which is the last index in the stringlist.
*/

int InputFile::getColCount()
{
    int cols;

    if(! fileData_)
        return 0;
    fileData_->seek(0);
    QString data            = fileData_->readLine();
    data                    = fileData_->readLine();
    QStringList dataList    = data.split("\\t");

    if(dataList.isEmpty())
        return 0;

    QString str = dataList.at(dataList.size() - 1);

    return str.toInt();
}

/*
The header labels are in the third line in the file
Eg: ID  Protocol    Version
Seek to the begining of the file. Read the third line. Split the line with "tab" and populate
the stringlist. Return the stringlist containing the header labels.
*/

QStringList InputFile::getHeaderLabel()
{
    QStringList headerLabels;

    if(fileData_)
    {
        fileData_->seek(0);
        QString data    = fileData_->readLine();
        data            = fileData_->readLine();
        data            = fileData_->readLine();
        headerLabels    = data.split("\t");
    }
    return headerLabels;
}

QStringList InputFile::getFirstRow()
{
    QStringList protocol;
    QString data;

    if(fileData_)
    {
        fileData_->seek(0);

        //Disgard the first three rows, read the fourth row where
        //the data starts
        for(int i=0; i<=3; i++)
            data = fileData_->readLine();

        protocol        = data.split("\t");
        InputFile::changeFilePos(fileData_->pos());
    }

    return protocol;
}

QStringList InputFile::getNextRow()
{
    QStringList protocol;
    QString data;

    if(fileData_)
    {
        fileData_->seek(pos);

        data        = fileData_->readLine();
        protocol    = data.split("\t");
        InputFile::changeFilePos(fileData_->pos());
    }

    return protocol;
}

void InputFile::changeFilePos(const qint64 pos)
{
    InputFile::pos = pos;
}
