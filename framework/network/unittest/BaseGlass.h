#ifndef BASEGLASS_H
#define BASEGLASS_H

#include <boost/shared_ptr.hpp>

#include "network/Rmi.h"
#include "common/DataSet.h"
#include "common/TextSerializer.h"
#include "common/Command.h"

namespace FN=Fog::Network;
namespace FC=Fog::Common;

namespace Fog {

typedef boost::shared_ptr<Fog::Network::Rmi> RmiPtr;

/**
 * @brief class that acts as a base class to any openglass application. This
 * class provides the following services to its children:
 * config reading 
 * log creation
 * rmi facility
 * it exposes its this pointer as a global so basically everyone can have access
 * to all it services from anywhere within the process space
 */
class BaseGlass
{
public:
    BaseGlass(const char* configFileName, const char* logFileName);
    virtual ~BaseGlass() {}
    virtual bool initialize();
    /**
     * @brief children will implement according to their wishes and manage the
     * workflow
     */
    virtual void run()=0;
    DataSet<TextSerializer>* getConfig() {return &config_;}
    RmiPtr getRmi() {return rmi_;}
    virtual void stop();
protected:
    RmiPtr rmi_;
    const char* configFileName_;
    const char* logFileName_;
    DataSet<TextSerializer> config_;
//    FC::CommandRouter* cmdRouter_;

};
extern BaseGlass* baseGlass;

}// Fog

#endif // BASEGLASS_H
