#include "Rmi.h"

#include "common/CommonDefs.h"

namespace Fog {
namespace Network{

Rmi::Rmi():
    debug_(true),
    reqId_(0)
{
}

void Rmi::initialize(PropertiesPtr& rmiSection)
{
    Properties::iterator ret;

    // initialize multi server
    ret=rmiSection->find(std::string("multiServer"));
    if (ret != rmiSection->end())
    {
        PropertiesPtr* serverDetails=boost::any_cast<PropertiesPtr>(&(ret->second));
        if (serverDetails== NULL)
        {
            LOG_ERROR("multiServer details not found ");
        }
        else
            serverStub_.reset(new Fog::Network::MultiServer(*serverDetails));
    }
    else
    {
        LOG_ERROR("multi server not present in config file");
    }

    // initialize client 
    ret=rmiSection->find(std::string("client"));
    if (ret != rmiSection->end())
    {
        PropertiesPtr* clientDetails=boost::any_cast<PropertiesPtr>(&(ret->second));
        if (clientDetails== NULL)
        {
            LOG_ERROR("client details not found ");
        }
        else
            clientStub_.reset(new Fog::Network::TCPClient(*clientDetails));
    }
    else
    {
        LOG_ERROR("client not present in config file");
    }

    // store the packetizer section 

    ret=rmiSection->find(std::string("cpacketizer"));
    if (ret != rmiSection->end())
    {
        PropertiesPtr* pktPtr=boost::any_cast<PropertiesPtr>(&(ret->second));
        if (pktPtr == NULL)
        {
            LOG_ERROR("unable to extract packetizer section from rmi");
        }
        else
        {
            packetizerSection_=*pktPtr;
            cliPkter_.reset(new Fog::Network::CPacketizer(packetizerSection_));
        }
    }
    else
    {
        LOG_ERROR("packetizer detail not found ");
    }

    // set up the connection for the server and for client on initial connection

    if (serverStub_)
    {
        (serverStub_->OnAccept).connect(boost::bind(&Fog::Network::Rmi::acceptConnection, this, _1));
        serverStub_->startAccept();
    }
    if (clientStub_)
    {
        (clientStub_->OnConnect).connect(boost::bind(&Fog::Network::Rmi::onConnect, this, _1));
        clientStub_->startConnect();
    }
}

/**
* @brief the client calls this method to invoke the remote method at the server
* end
*
* @param ds - data set which contains all the required information for the
* remote method to be executed
* @param isAsync -if async then this method returns a valid ReqPtr which then
* needs to be maintained by the client. Client can continue to work
* asychnronously and when ready can go and wait in the getReply method 
* if async is false then the this method will not return a request instead it
* will wait until 
*
* @return 
*/

DSptr Rmi::invokeRemoteMethod(DataSet<TextSerializer> & ds)
{
    lock_.lock();
    if (!serverConn_)
    {
        lock_.unlock();
        // TODO add error 
        return DSptr();
    }

    reqId_++;
    if(! ds.add<int>("REQUEST", "REQUESTID", reqId_))
    {
        std::ostringstream tmp;
        tmp << "unable to add REQUEST ID " << reqId_ 
            << "to section REQUEST" << std::endl;
        LOG_ERROR(tmp.str());
        return DSptr();
    }

    ReqPtr newRequest(new Request(reqId_)); // will store state of request until reply
                                            // due to asynchronous operation of
                                            // sending
    requests_.insert(std::pair<int32_t, ReqPtr>(reqId_,newRequest ));
    lock_.unlock();

    std::string request = ds.serialize();
    //TODO - should this be a part of the serializer itself or probably as a
    // o/p packetizer

    int32_t reqLength=request.size();
    char length[4+1];
    memset(length, 0, sizeof(length));
    sprintf(length, "%04d", reqLength);
    request.insert(0, length);
    serverConn_->send(request.c_str(), request.size());
    return newRequest->getReply(); // will wait until we receive reply or we timeout
}

ReqPtr Rmi::invokeRemoteMethodAsync(DataSet<TextSerializer>& ds)
{
    lock_.lock();
    if (!serverConn_)
    {
        lock_.unlock();
        // TODO - add error message
        return ReqPtr(); // TODO should we return something more intuitive?
    }

    reqId_++;
    if(! ds.add<int>("REQUEST", "REQUESTID", reqId_))
    {
        std::ostringstream tmp;
        tmp << "unable to add REQUEST ID " << reqId_ 
            << "to section REQUEST" << std::endl;
        LOG_ERROR(tmp.str());
        return ReqPtr();
    }
    ReqPtr newRequest(new Request(reqId_)); // will store state of request until reply
                                            // due to asynchronous operation of
                                            // sending
    requests_.insert(std::pair<int32_t, ReqPtr>(reqId_,newRequest ));
    lock_.unlock();

    std::string request = ds.serialize();

    //TODO - should this be a part of the serializer itself or probably as a
    // o/p packetizer

    int32_t reqLength=request.size();
    char length[4+1];
    memset(length, 0, sizeof(length));
    sprintf(length, "%04d", reqLength);
    request.insert(0, length);

    serverConn_->send(request.c_str(), request.size());
    return newRequest; // the client is responsible for calling 
                       // getReply on the new request when he deems proper
}

/**
* @brief gets called when client gets connected to the server
*
* @param conn - the server connection 
*/

void Rmi::onConnect(TCPConnectionPtr conn)
{
    (conn->OnData).connect(boost::bind(&Fog::Network::CPacketizer::operator(), &(*cliPkter_), _1, _2));
    (cliPkter_->OnPacket).connect(boost::bind(&Fog::Network::Rmi::processReply, this, _1, _2));
    serverConn_=conn;
}

void Rmi::processReply(char* data, size_t size)
{
    // read data and deserialize into a reply dataset

    DSptr   reply(new DataSet<TextSerializer> ());
    reply->deserialize(data);

    // get the request id and then route reply back to that request

    int* requestIdPtr=reply->get<int>("REQUEST", "REQUESTID");
    if (!requestIdPtr)
    {
        std::ostringstream tmp;
        std::string tmpStr (data, size);
        tmp << "Dropping reply. No RequestID found. UI may hang - data: " << tmpStr.c_str() 
            << "\n";
        LOG_ERROR(tmp.str());
        return;
    }

    if (debug_)
    {
        std::cout  << "reply data is - " << data << std::endl;
        std::cout << "the request id is " << *requestIdPtr<< std::endl;
    }

    // find request in map of requests

    IdRequestMap::iterator ret = requests_.find(*requestIdPtr);
    if (ret==requests_.end())
    {
        std::ostringstream tmp;
        std::string tmpStr (data, size);
        tmp << "No request waiting for this reply - : " << tmpStr.c_str() 
            << "\n";
        LOG_ERROR(tmp.str());
        return;
    }
    // get the request associated with this id
    (ret->second)->setReply(reply);
}

void Rmi::acceptConnection(TCPConnectionPtr conn)
{
    // set up the individual connections 
    CmdPacketizer cmdPkter(new Fog::Network::CPacketizer(packetizerSection_));
    RmiConnPtr rmiConn(new RmiConnection(conn, cmdPkter));
    rmiConn->initialize(connSection_);
    (conn->OnData).connect(boost::bind(&Fog::Network::CPacketizer::operator(), &(*cmdPkter), _1, _2));
    (cmdPkter->OnPacket).connect(boost::bind(&Fog::Network::RmiConnection::processCmd, &(*rmiConn), _1, _2));
    connections_.push_back(rmiConn);
}

void Rmi::stop()
{
    if (clientStub_)
        clientStub_->stop();
    if (serverStub_)
        serverStub_->stop(); 

    // clear out all rmi connections
    connections_.clear();
}

} // Network
} // Fog

