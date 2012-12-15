#include "BaseGlass.h"

//BaseGlass* Fog::baseGlass = NULL;

namespace Fog {

    BaseGlass::BaseGlass(const char* configFileName, const char* logFileName)
//        :configFileName_(configFileName), logFileName_(logFileName), cmdRouter_(0)
        :configFileName_(configFileName), logFileName_(logFileName)
    {
        if (!configFileName_ | !logFileName_)   // TODO - need to make serializer 
                                                // completely independent of Log 
        {
            throw std::runtime_error("Config file and log file required to initailize BaseGlass ");
        }
//        baseGlass = this;
//        cmdRouter_=FC::CommandRouter::getInstance();
    }

    bool BaseGlass::initialize()
    {
        // set up logger
        Logger::create(logFileName_);

        // read config file
        std::ifstream is;
        is.open(configFileName_);
        is.seekg(0, std::ios::end);
        long length=is.tellg();
        is.seekg(0, std::ios::beg);

        char* buffer=new char[length];
        is.read(buffer, length);

        std::string config(buffer);

        config_.deserialize(buffer);

        PropertiesPtr* rmiDetails = config_.get<PropertiesPtr>("openGlass", "rmi");
        if (!rmiDetails )
        {
            LOG_ERROR("Rmi Details not present in config.");
            throw std::runtime_error("Unable to initailize BaseGlass ");  // Throw an error later
        }

        // initialize rmi 
        rmi_.reset(new Fog::Network::Rmi());
        rmi_->initialize(*rmiDetails);
        return true;
    }
    void BaseGlass::stop()
    {
        // stopping rmi
        rmi_->stop();
        std::ostringstream tmp;
        tmp << "Rmi Stopped" << "\n";
        LOG_DEBUG(tmp.str());
    }

}// Fog

