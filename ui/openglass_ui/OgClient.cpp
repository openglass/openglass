#include "OgClient.h"

int OgClient :: firstRun = 1;

OgClient::OgClient( /*DataSet <TextSerializer> &dataSet, */QThread *parent) : QThread(parent)/*, dataSet_(dataSet)*/
{
    event = 0;

    request_  = NULL;
    OgClient::firstRun  = 1;
}

void OgClient :: setRequest( DataSet<TextSerializer>* req )
{
    //memcpy(&request_, (void *)&req, sizeof(req));
    request_ =  req;
}

DataSet <TextSerializer> * OgClient :: getRequest()
{
    return request_;
}

bool OgClient :: initialize(DataSet<TextSerializer>& config_)
{

    std::string section="openGlass";
    std::string tcpClient="tcpClient";
    PropertiesPtr* clientDetails= config_.get<PropertiesPtr>(section, tcpClient);
    if (!clientDetails)
    {
        LOG_ERROR("Server Details not present in config.");
        throw std::runtime_error("Unable to initailize OgClient ");  // Throw an error later
    }

    tcpClient_.reset(new Fog::Network::TCPClient(*clientDetails));

    std::string packetizer="cpacketizer";
    PropertiesPtr* packetizerDetails= config_.get<PropertiesPtr>(section, packetizer);
    if (!packetizerDetails)
    {
        LOG_ERROR("Packetizer Details not present in config.");
        throw std::runtime_error("Unable to initailize OgClient ");  // Throw an error later
    }
    cmdPacketizer_.reset(new Fog::Network::CPacketizer(*packetizerDetails));

    // get the connections correct

    (tcpClient_->OnConnect).connect(boost::bind(&OgClient::operator(), this, _1));
    return true;
}


void OgClient :: run()
{

    tcpClient_->startConnect();

    OgClient :: firstRun = 0;
    //closeConnection();
}

/*void OgClient :: handleDataReceived()
{
        closeConnection();
}*/

void OgClient :: closeConnection()
{
    tcpClient_->stop();
    tcpClient_->join();
}

void OgClient :: operator()(TCPConnectionPtr conn)
{

    conn_=conn;

    // set up the linkages
    (conn->OnData).connect(boost::bind(&Fog::Network::CPacketizer::operator(), &(*cmdPacketizer_), _1, _2));
    (cmdPacketizer_->OnPacket).connect(boost::bind(&OgClient::onPacket, this, _1, _2));

    // send initial request
   // sendRequest();
}


bool OgClient :: sendRequest()
{
     //create the reqest

    LOG_DEBUG("******************************Sending the request to server*****************************\n");

    boost::mutex::scoped_lock lock(waitForDataMutex_);

    std::string request;
    request             = request_->serialize();

    LOG_DEBUG(request);

    int32_t reqLength   = request.size();
    char length[5];
    memset(length, 0, sizeof(length));
    sprintf(length,"%04d", reqLength);
    request.insert(0, length);

    //When in debug mode, gives error saying conn_ is NULL, giving time for the second thread
    //to finish the setup of database connection
    if(firstRun == 1)
    {
        sleep(2);
    }

    if( conn_)
        conn_->send(request.c_str(), request.size());
    else
    {
        LOG_DEBUG("Connection unsuccessful");
        return false;
    }

    if( OgClient :: firstRun == 0 )
    {
        std::cout << "///////////////// Waiting ///////////////" <<std::endl;

        dataReceived_.wait(lock);
    }
    return true;
}


void OgClient :: onPacket(char* data, size_t size)
{

    LOG_DEBUG("******************Received the response from the server******************************\n");


    LOG_DEBUG("Calling deserializeeeeeee");

    request_->deserialize(data);

     LOG_DEBUG("after deserializeeeeeee");

     boost::mutex::scoped_lock lock(waitForDataMutex_);
     dataReceived_.notify_one();

     std::cout << "/////////////Wait Over////////////////" << std::endl;

}
