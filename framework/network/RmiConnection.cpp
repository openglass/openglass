#include "RmiConnection.h"
#include "common/Logger.h"

namespace Fog {
namespace Network{

void RmiConnection::initialize(PropertiesPtr& rmiConnSection)
{
    cmdRouter_=FC::CommandRouter::getInstance();
    debug_=true;
}

RmiConnection::~RmiConnection()
{
    stop();
}

void RmiConnection::stop()
{
    conn_->Close();
}

void RmiConnection::processCmd(char* data, size_t size)
{
    // create DataSet
    if (debug_)
    {
        std::ostringstream tmp;
        tmp << " Incoming Requet - " << data << std::endl;
        LOG_DEBUG(tmp.str());
    }
    DataSet<TextSerializer> inRequest;
    const char *request="REQUEST";
    const char *requestId="REQUESTID";
    const char *method="METHOD";
    const char *msg="STATMESSAGE";
    const char* stat="STATUS";

    inRequest.deserialize(data);
    int* id = inRequest.get<int>(request, requestId);
    std::string* methodStr=inRequest.get<std::string>(request, method);
    // if no method  then return with status false
    if (!id || !methodStr)
    {
        // return map with status false
        inRequest.add<std::string>(request, stat, "false");
        inRequest.add<std::string>(request, msg, "No Request Id or Method defined in request");
        // serialise and send back the dataset to remote
       sendResponse(inRequest);
       return;
    }

    if (debug_)
    {
        std::ostringstream tmp;
        tmp << " Invoking method " << (*methodStr).c_str() << std::endl;
        LOG_DEBUG(tmp.str());
    }

    // invoke the method using command router
    cmdRouter_->invokeMethod(methodStr->c_str(), inRequest);
    sendResponse(inRequest);
    return;
}

void RmiConnection::sendResponse(DataSet<TextSerializer>& ds)
{
    // serialize the dataset
    // // TODO - need to create raw logging mechanis.. sending and receiving

    std::string serializedDs=ds.serialize();
    int32_t reqLength=serializedDs.size();
    char length[4+1];
    memset(length, 0, sizeof(length));
    sprintf(length, "%04d", reqLength);
    serializedDs.insert(0, length);
    conn_->send(serializedDs.c_str(), serializedDs.size());

    if (debug_)
    {
        std::ostringstream tmp;
        tmp << "Outgoing Data queued - " << serializedDs.c_str() << "\n";
        LOG_DEBUG(tmp.str());
    }
}

} // Network
} // Fog
