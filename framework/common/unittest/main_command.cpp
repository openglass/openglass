#include <iostream>
#include <list>
#include <boost/any.hpp>
#include <sstream>
#include <fstream>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>

#include "common/DataSet.h"
#include "common/TextSerializer.h"
#include "common/CommonDefs.h"
#include "common/Logger.h"
#include "common/Command.h"

namespace FC=Fog::Common;


class test
{
public: 
        test() : arbit(5){
            FC::CommandRouter::getInstance()->registerCmd( "testMethod", 10, 
                                                           "just a test",11, 
                                       boost::bind(&test::testMethod, 
                                                   this, _1));
            FC::CommandRouter::getInstance()->registerCmd( "testMethod2", 11, 
                                                           "just a test2",12, 
                                       boost::bind(&test::testMethod2, 
                                                   this, _1));
        }

private
        void testMethod(DataSet<TextSerializer>& ds1)
        { 
            std::cout << "called testmethod yehhhh" << std::endl; 
            PropertiesPtr* resultSet = ds1.get<PropertiesPtr>("response", "resultset");
            for( Properties::iterator iter = (*resultSet)->begin(); 
                    iter != (*resultSet)->end(); 
                    iter++)
            {
                LOG_DEBUG("In the Loooooop");
                StringsPtr* record= boost::any_cast<StringsPtr>(&(*iter).second);
                Strings::iterator iter=(*record)->begin();
                while ( iter != (*record)->end())
                {
                    std::cout << *iter << std::endl;
                    iter++;
                }
            }
        }

        void testMethod2(DataSet<TextSerializer>& ds)
        { std::cout << "called test method 2 yehhhh" << std::endl; }

        int32_t arbit;
};


int main()
{
    test t1;

    std::string request("response{ requestid=1 method=testMethod nor=10  resultset{ result=(\"arca\" \"100\" \"hello\") result2=(\"arca\" \"102\" \"hello\")}}");

    DataSet<TextSerializer> ds1;
    ds1.deserialize(request.c_str());
    std::string* met = ds1.get<std::string>("response", "method");
    if (met)
        std::cout << *met << std::endl;

    FC::CommandRouter::getInstance()->invokeMethod((*met).c_str(), ds1);
    FC::CommandRouter::getInstance()->invokeMethod("testMethod2", ds1);
}
