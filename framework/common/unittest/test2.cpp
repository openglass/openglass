#include <iostream>
#include <list>
#include <boost/any.hpp>
#include <sstream>
#include <fstream>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/lexical_cast.hpp>

#include "common/DataSet.h"
#include "common/TextSerializer.h"
#include "common/CommonDefs.h"
#include "common/Logger.h"

int main()
{
#if 0
    std::ifstream is;
    is.open("test_config2");
    is.seekg(0, std::ios::end);
    long length=is.tellg();
    is.seekg(0, std::ios::beg);

    char* buffer=new char[length];
    is.read(buffer, length);

    std::string arbit(buffer);

    DataSet<TextSerializer> ds1;
    ds1.deserialize(buffer);
    std::string serialized=ds1.serialize();
    std::cout << serialized.c_str() << std::endl;
    std::string section="openGlass";
    std::string serverD="serverDetails";
    std::string noOfT="noofthreads";
    PropertiesPtr* serverDetails = ds1.get<PropertiesPtr>(section, serverD);
    if (serverDetails)
    {
        std::ostringstream tmp;
        Properties::iterator ret;
        ret=(*serverDetails)->find(std::string("comm"));
        if (ret != (*serverDetails)->end())
        {
            std::string* str=boost::any_cast<std::string>(&(ret->second));
            if (str == NULL)
            {
                LOG_ERROR("Value not as expected");
            }
            tmp << "comm is " << str->c_str() << std::endl;
            std::cout << tmp.str() << std::endl;
        }
    }
    int* noofthreads=ds1.get<int>(section, noOfT);
    if(noofthreads)
        std::cout << " no of threads = " << *noofthreads << std::endl;
    int* arvit = ds1.get<int>("response", "requestid");
    std::cout << "request id is" << *arvit << std::endl;

#endif 
#if 0
    DataSet<TextSerializer> ds1("request");

    ds1.add<std::string>("request", "requestId", "101");

    PropertiesPtr prop(new Properties());
    ds1.add<PropertiesPtr>("request","args", prop);
    ds1.add<std::string>("args","protocol", "1.1");

    std::string* prot=ds1.get<std::string>("args", "protocol");
    if (prot)
        std::cout << *prot << std::endl;

    std::string serialized=ds1.serialize();
    std::cout << serialized.c_str() << std::endl;
    DataSet<TextSerializer> ds2;
    ds2.deserialize(serialized.c_str());
    std::string* prot2 = ds2.get<std::string>("args", "protocol");
    if (!prot2)
        std::cout << "unable to deserialize prot" << std::endl;
    std::cout << (*prot2).c_str() << std::endl;

#endif 

    std::string request("response{ requestid=1 method=getTags nor=10  resultset{ result=(\"100\" \"arca\" \"hello\") result2=(\"102\"  \"arca\" \"hello\")}}");

    DataSet<TextSerializer> ds1;
    ds1.deserialize(request.c_str());
    std::string* met = ds1.get<std::string>("response", "method");
    if (met)
        std::cout << *met << std::endl;

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
