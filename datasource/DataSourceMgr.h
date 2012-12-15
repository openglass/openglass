#ifndef DATASOURCE_MGR_H 
#define DATASOURCE_MGR_H 

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/signal.hpp>
#include <common/DataSet.h>
#include <common/TextSerializer.h>
#include <common/CommonDefs.h>
#include <common/Logger.h>
#include "Connection.h"

#include <sstream>
#include <pqxx/except>
#include <pqxx/transaction>
#include "Protocol.h"
#include "Message.h"
#include "Tag.h"
#include "TestCaseMapper.h"

typedef boost::shared_ptr<DataSet<TextSerializer> > DSptr;
namespace Fog {
namespace Datasource {

/**
* @brief gateway class for protocol table
*/
class DataSourceMgr
{
public:
    DataSourceMgr();

    /**
     * @brief Data set will contain all the values required to create a row
     */
    void initialize(DSptr p );
    boost::signal< void (DSptr) > OnInitialize; // signal that all gateway classes can connect to.

private:
    boost::shared_ptr<Protocol> protocolPtr_;
    boost::shared_ptr<Message> msgPtr_;
    boost::shared_ptr<Tag> tagPtr_;
    boost::shared_ptr<TestCaseMapper> testCaseMapperPtr_;
};

} // datasource
} // fog

#endif  // DATASOURCE_MGR_H 
