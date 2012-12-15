#ifndef MESSAGE_H
#define MESSAGE_H


#include <string>
#include <common/DataSet.h>
#include <common/TextSerializer.h>
#include <common/CommonDefs.h>
#include <common/Logger.h>
#include "Connection.h"
#include "common/Command.h"
#include "opencommon/ModelInfo.h"

namespace FC        = Fog::Common;

#include <sstream>
#include <pqxx/except>
#include <pqxx/transaction>
#include <pqxx/tuple>
#include <boost/bind.hpp>



namespace Fog {
namespace Datasource {

typedef boost::shared_ptr<DataSet<TextSerializer> > DSptr;
typedef boost::shared_ptr<TagInfo> TagInfoPtr;
typedef boost::shared_ptr < std::vector< TagInfoPtr > > MessageTagInfoListPtr;

/**
* @brief gateway class for message table
*/
class Message
{
public:
    Message();

    /**
     * @brief Data set will contain all the values required to create a row
     */
    bool addMessage(DataSet<TextSerializer> & ds);

    /**
     * @brief Data set will contain the new values
     */
    bool updateMessage(DataSet<TextSerializer> & ds);

    /**
     * @brief Data set will contain all the values required to create a row
     * ret
     */
    bool delMessage(DataSet<TextSerializer> & ds);

    /**
     * @brief to find data from the table. The resulting rows/data will be added
     * to the dataset itself thereby making the interface consistent
     */
    bool findAllMessages(DataSet<TextSerializer> & ds);

    //register all the methods
    void initialize(DSptr);

    //returns the list of tags in a testcase message, parameter is an output parameter
    bool getTestCaseMsgTagInfo(DataSet<TextSerializer> &dSet);


private:
    std::string req_;
    std::string args_;
    std::string stat_;
    std::string statMsg_;
    std::string protocolId_;
    std::string messageId_;
    std::string messageName_;
    std::string messageDesc_;
    std::string created_;
    std::string testCaseMId_;
    std::string tagInfoList_;
};

} // datasource
} // fog


#endif // MESSAGE_H
