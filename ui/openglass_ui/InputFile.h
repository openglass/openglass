#ifndef INPUTFILE_H
#define INPUTFILE_H

/*
File name   :   InputFile.h
Desc        :   InputFile Class declaration
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
                Note : The words in the file should be separated with tabs ("\t")

Author      :   Renjini
Version     :   1.0
*/

#include "QtGui"
#include "QIODevice"

class InputFile
{
private:
    QFile* fHandle_;
    QTextStream* fileData_;
    static int pos;
    static void changeFilePos(const qint64);
public:
    InputFile();
    InputFile(const QString file);
    ~InputFile();
    bool open();
    void close();
    int getRowCount();
    int getColCount();
    QStringList getHeaderLabel();
    QStringList getFirstRow();
    QStringList getNextRow();



};

#endif // INPUTFILE_H
