#include "DataSourceMgr.h"

#include <boost/bind.hpp>
#include <sstream>
#include <pqxx/except>
#include <pqxx/transaction>

namespace Fog {
namespace Datasource {

/**
* @brief gateway class for protocol table
*/
DataSourceMgr::DataSourceMgr() 
{
    protocolPtr_.reset(new Protocol());
    msgPtr_.reset(new  Message());
    tagPtr_.reset(new  Tag());
    testCaseMapperPtr_.reset(new  TestCaseMapper());

    (this->OnInitialize).connect(boost::bind(&Fog::Datasource::Protocol::initialize, 
                                    &(*protocolPtr_), _1));
    (this->OnInitialize).connect(boost::bind(&Fog::Datasource::Message::initialize,
                                    &(*msgPtr_), _1));
    (this->OnInitialize).connect(boost::bind(&Fog::Datasource::Tag::initialize,
                                    &(*tagPtr_), _1));
    (this->OnInitialize).connect(boost::bind(&Fog::Datasource::TestCaseMapper::initialize,
                                    &(*testCaseMapperPtr_), _1));
}

/**
 * @brief Data set will contain all the values required to create a row
 */
void DataSourceMgr::initialize(DSptr p)
{
    OnInitialize(p);
}
    
} // datasource
} // fog
