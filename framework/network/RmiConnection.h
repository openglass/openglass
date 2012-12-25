#ifndef RMI_CONNECTION_H
#define RMI_CONNECTION_H

#include <string.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <map>
#include <vector>

#include "common/DataSet.h"
#include "common/TextSerializer.h"
#include "common/Command.h"
#include "Packetizer.h"
#include "TCPClient.h"
#include "MultiServer.h"

namespace FC=Fog::Common;

namespace Fog {
namespace Network{

typedef boost::shared_ptr<Fog::Network::TCPConnection> TCPConnectionPtr;
typedef boost::shared_ptr<Fog::Network::CPacketizer> CmdPacketizer;

class RmiConnection
{
public:
    RmiConnection(TCPConnectionPtr conn, CmdPacketizer cmdPacket) : 
        requestId_(0), conn_(conn), cmdPacketizer_(cmdPacket), debug_(true) {}
    ~RmiConnection();

    void initialize(PropertiesPtr& rmiConnSection);
    void processCmd(char* data, size_t size);

    void setRequestId(uint32_t theReqId) { requestId_=theReqId; }
    uint32_t getRequestId() {return requestId_; }
    void stop();
private:
    void sendResponse(DataSet<TextSerializer>& ds);
    int                 requestId_;
    TCPConnectionPtr    conn_;
    CmdPacketizer       cmdPacketizer_;
    FC::CommandRouter* cmdRouter_;
    bool debug_;
};

} // Network
} // Fog
#endif // RMI_CONNECTION_H
