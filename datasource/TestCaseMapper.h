#ifndef TEST_CASE_GW_H
#define TEST_CASE_GW_H

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

#include <opencommon/ModelInfo.h>


typedef boost::shared_ptr<DataSet<TextSerializer> > DSptr;
typedef boost::shared_ptr<TestCaseMsgInfo > MsgInfoPtr;
typedef boost::shared_ptr<std::vector<MsgInfoPtr> > TestMsgsListPtr;

namespace Fog {
namespace Datasource {

/**
* @brief gateway class for tag table
*/
class TestCaseMapper
{
public:
    TestCaseMapper();

    /**
     * @brief Data set will contain all the values required to create a row
     */
    //bool addTestCase(DataSet<TextSerializer> & ds) {}

    /**
     * @brief Data set will contain the new values
     */
    //bool updateTestCase(DataSet<TextSerializer> & ds) {}

    /**
     * @brief Data set will contain all the values required to create a row
     * ret
     */
    //bool delTestCase(DataSet<TextSerializer> & ds) {}

    /**
     * @brief to find data from the table. The resulting rows/data will be added
     * to the dataset itself thereby making the interface consistent
     */
    //bool findTestCase(DataSet<TextSerializer> & ds) {}

    /**
     * @brief to find data from the table. The resulting rows/data will be added
     * to the dataset itself thereby making the interface consistent
     */
    bool findTestMsgs(DataSet<TextSerializer> & ds);

    void initialize(DSptr);

private:
    std::string req_;
    std::string args_;
    std::string stat_;
    std::string statMsg_;
};
} // datasource
} // fog

#endif // TEST_CASE_GW_H
