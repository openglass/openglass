#include <iostream>
#include <list>
#include <boost/any.hpp>
#include <sstream>
#include <fstream>
#include <string>

#include <boost/shared_ptr.hpp>
#include <common/DataSet.h>
#include <common/TextSerializer.h>
#include <common/CommonDefs.h>


#include <model/Tag.h>
#include <model/Message.h>
#include <model/TestCase.h>
#include <datasource/DataSourceMgr.h>


int main()
{
    Fog::Model::TestCase test_;
    DataSet<TextSerializer> dataSet("REQUEST");

    std::cout<<"Deserialize the request\n\n";
    std::cout<<"requestID\n\n";

    dataSet.add<std::string>("REQUEST", "METHOD", "initTestCase");

    boost::shared_ptr<Fog::Datasource::DataSourceMgr> dsMgrptr_;
    boost::shared_ptr<DataSet<TextSerializer> > ds;
    dsMgrptr_.reset(new Fog::Datasource::DataSourceMgr());
    dsMgrptr_->initialize(ds);

   //Add the args section
    PropertiesPtr argsProp(new Properties());
    argsProp->insert(std::make_pair<std::string, int>("TESTID", 1));
   
    dataSet.add<PropertiesPtr>("REQUEST", "ARGS", argsProp);

    std::cout<<"Created the request\n\n";
    test_.initialize(dataSet);
    return 0;
}
