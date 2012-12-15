#ifndef PROTOCOL_H
#define PROTOCOL_H

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
* @brief gateway class for protocol table
*/
class Protocol
{
public:
    Protocol();

    /**
     * @brief Data set will contain all the values required to create a row
     */
    bool addProtocol(DataSet<TextSerializer> & ds);

    /**
     * @brief Data set will contain the new values
     */
    bool updateProtocol(DataSet<TextSerializer> & ds);

    /**
     * @brief Data set will contain all the values required to create a row
     * ret 
     */
    bool delProtocol(DataSet<TextSerializer> & ds);

    /**
     * @brief to fid data from the table. The resulting rows/data will be added
     * to the dataset itself thereby making the interface consistent
     */
    bool findAllProtocols(DataSet<TextSerializer> & ds);
    void initialize(DSptr);
private:
    std::string req_;
    std::string args_;
    std::string stat_;
    std::string msg_;
    std::string protocol_;
    std::string protocolName_;
    std::string version_;
    std::string type_;
    std::string created_;
    std::string resultSet_;
};

} // datasource
} // fog

#endif 
