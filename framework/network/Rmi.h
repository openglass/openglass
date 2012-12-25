#ifndef RMI_H
#define RMI_H

#include <string.h>
#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

#include "common/DataSet.h"
#include "common/TextSerializer.h"
#include "common/Command.h"
#include "Packetizer.h"
#include "RmiConnection.h"
#include "Request.h"
#include "TCPClient.h"
#include "MultiServer.h"

/**
 * @brief Accepts registration of commands from the client and with instance of
 * this class any object within this process space will be able to call this
 * class.
 * 
 */

namespace Fog {
namespace Network{

typedef boost::shared_ptr<Fog::Network::TCPConnection> TCPConnectionPtr;
typedef boost::shared_ptr<Fog::Network::CPacketizer> CmdPacketizer;
typedef boost::shared_ptr<Fog::Network::MultiServer> MultiSrvrPtr;
typedef boost::shared_ptr<Fog::Network::TCPClient> TCPCliPtr;
typedef boost::shared_ptr<Fog::Network::RmiConnection> RmiConnPtr;

/**
 * @brief Rmi class accepts registration of methods from any class in the 
 * process space. It maintains this registry and can invoke this method 
 * as a remote request as well as a client request.
 *
 * @param Server any server class which has a policy of invoking onAccept 
 * signal  with a TCPConnection session.
 * @param Client
 */

typedef boost::shared_ptr<Request>  ReqPtr;
typedef boost::shared_ptr<DataSet<TextSerializer> >  DSptr;
typedef std::map<int32_t, ReqPtr>    IdRequestMap; // TODO check if we can use hashmap
class Rmi
{
public:
    Rmi();
    void initialize(PropertiesPtr& prop);

    /**
    * @brief will call remote method synchronously
    *
    * @param request
    * @return returns a Data Set pointer. Its upto client to check if this is
    * NULL or not. This dsptr can be empty in case of timed our requests. 
    */

    DSptr invokeRemoteMethod(DataSet<TextSerializer> & ds);
    /**
    * @brief will call remote method asynchronously
    *
    * @param ds
    *
    * @return  will return a Request Object. Client can use the request object
    * and then call getReply to get the actual reply DataSet that the server
    * responded with. 
    */
    ReqPtr invokeRemoteMethodAsync(DataSet<TextSerializer> & ds);

    void stop();

private:
    void processReply(char* data, size_t size);
    void acceptConnection(TCPConnectionPtr);
    void onConnect(TCPConnectionPtr conn);
    TCPCliPtr       clientStub_;
    MultiSrvrPtr    serverStub_; 
    Fog::Common::CommandRouter* cmdRouter_;
    PropertiesPtr packetizerSection_;
    PropertiesPtr connSection_;
    std::vector<RmiConnPtr> connections_;
    IdRequestMap requests_;
    CmdPacketizer cliPkter_;
    TCPConnectionPtr serverConn_;
    bool debug_;
    boost::mutex lock_;
    int32_t reqId_;
};

} // Network
} // Fog
#endif // RMI_H
