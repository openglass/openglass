#ifndef TAG_H
#define TAG_H

#include <string>
#include <common/DataSet.h>
#include <common/TextSerializer.h>
#include <common/CommonDefs.h>
#include <common/Logger.h>
#include "Connection.h"
#include "common/Command.h"

namespace FC=Fog::Common;


#include <sstream>
#include <pqxx/except>
#include <pqxx/transaction>
#include <boost/bind.hpp>


typedef boost::shared_ptr<DataSet<TextSerializer> > DSptr;

namespace Fog {
namespace Datasource {

/**
* @brief gateway class for tag table
*/
class Tag
{
public:
    Tag();

    /**
     * @brief Data set will contain all the values required to create a row
     */
    bool addTag(DataSet<TextSerializer> & ds);

    /**
     * @brief Data set will contain the new values
     */
    bool updateTag(DataSet<TextSerializer> & ds);

    /**
     * @brief Data set will contain all the values required to create a row
     * ret
     */
    bool delTag(DataSet<TextSerializer> & ds);

    /**
     * @brief to find data from the table. The resulting rows/data will be added
     * to the dataset itself thereby making the interface consistent
     */
    bool findAllTags(DataSet<TextSerializer> & ds);

    void initialize(DSptr);

private:
    std::string req_;
    std::string args_;
    std::string generic_;
    std::string nonGeneric_;
    std::string both_;
    std::string state_;
    std::string stat_;
    std::string statMsg_;
    std::string protocolId_;
    std::string messageId_;
    std::string protMsgTagId_;
    std::string tagId_;
    std::string tagName_;
    std::string tagDesc_;
    std::string tagLen_;
    std::string tagType_;
    std::string tagDestType_;
    std::string tagJustify_;
    std::string tagEditable_;   //Editble for whom
    std::string tagOffset_;
    std::string tagReq_;        //The importance of this property is in while making a message???
    std::string tagValue_;
    std::string tagSeq_;
    std::string created_;


    enum {
            GENERIC     = 1,
            NONGENERIC  = 2,
            BOTH        = 3
         };

};
} // datasource
} // fog

#endif // TAG_H
