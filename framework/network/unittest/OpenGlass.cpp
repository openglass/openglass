#include "OpenGlass.h"

#include "common/Command.h"

namespace FC=Fog::Common;
namespace Fog {

    OpenGlass::OpenGlass(const char* configFileName, const char* logFileName)
        : BaseGlass(configFileName, logFileName)
    {
        FC::CommandRouter::getInstance()->registerCmd( "testMethod", 10, 
                "just a test",11, 
                boost::bind(&OpenGlass::testMethod, 
                    this, 
                    _1));
        FC::CommandRouter::getInstance()->registerCmd( "testMethod2", 11, 
                "just a test2",12, 
                boost::bind(&OpenGlass::testMethod2, 
                    this, 
                    _1));

        FC::CommandRouter::getInstance()->registerCmd( "exit", 4, 
                "Stopping the application",25, 
                boost::bind(&OpenGlass::onStop, this, _1));
    }

    bool OpenGlass::initialize()
    {
        // Initialize base
        BaseGlass::initialize();
        return true;
    }

    void OpenGlass::testMethod(DataSet<TextSerializer>& ds1)
    { 
        std::cout << "called testmethod 1yehhhh" << std::endl; 
        int* reqIdPtr= ds1.get<int>("REQUEST", "REQUESTID");
        if (reqIdPtr)
        {
            std::cout << " request id is" << *reqIdPtr << "\n";
            ds1.add<std::string>("REQUEST", "STATUS", "true");
            return;
        }
        else
        {
            std::cout << " unable to retrieve request id is" << "\n";
            ds1.add<std::string>("REQUEST", "STATUS", "false");
            ds1.add<std::string>("REQUEST", "MSG", "Unable to retriev id");
            return;
        }
    }

    void OpenGlass::testMethod2(DataSet<TextSerializer>& ds1)
    { 
        std::cout << "called testmethod 2yehhhh" << std::endl; 
        int* reqIdPtr= ds1.get<int>("REQUEST", "REQUESTID");
        if (reqIdPtr)
        {
            std::cout << " request id is" << *reqIdPtr << "\n";
            ds1.add<std::string>("REQUEST", "STATUS", "true");
            return;
        }
        else
        {
            std::cout << " unable to retrieve request id is" << "\n";
            ds1.add<std::string>("REQUEST", "STATUS", "false");
            ds1.add<std::string>("REQUEST", "MSG", "Unable to retriev id");
            return;
        }
    }

    void OpenGlass::onStop(DataSet<TextSerializer>& ds)
    { 
        std::cout << "Stopping Open Glass " << std::endl; 
        boost::mutex::scoped_lock lock(stop_);
        stopEvent_.notify_one();
    }

    void OpenGlass::run()
    {
        // wait for the exit command to be called 
        boost::mutex::scoped_lock lock(stop_);
        stopEvent_.wait(lock);
        // stop rmi
        BaseGlass::stop();
        std::cout << "Stopping Base Glass" << std::endl; 
    }

}// Fog

