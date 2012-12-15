#include "OpenGlass.h"

#include "common/Command.h"

namespace FC=Fog::Common;
namespace Fog {

    OpenGlass::OpenGlass(const char* configFileName, const char* logFileName)
        : BaseGlass(configFileName, logFileName)
    {
        FC::CommandRouter::getInstance()->registerCmd( "exit", 4, 
                "Stopping the application",25, 
                boost::bind(&OpenGlass::onStop, this, _1));
        dsMgrptr_.reset(new Fog::Datasource::DataSourceMgr());
    }

    bool OpenGlass::initialize()
    {
        // Initialize base
        BaseGlass::initialize();

        // right now we do not have any propterties to s
        // sen
        DSptr p;
        dsMgrptr_->initialize(p);

        return true;
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

