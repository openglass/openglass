#include <boost/shared_ptr.hpp>

#include "network/TCPConnection.h"
#include "network/TCPClient.h"
#include "network/Packetizer.h"
#include "common/DataSet.h"
#include "common/TextSerializer.h"

namespace Fog {

class OpenGlass
{
public:
    typedef boost::shared_ptr<Fog::Network::TCPConnection> TCPConnectionPtr;
    typedef boost::shared_ptr<Fog::Network::TCPClient> TCPClient;
    typedef boost::shared_ptr<Fog::Network::CPacketizer> CmdPacketizer;
    OpenGlass()
    {
    }

    bool initialize(DataSet<TextSerializer>& config_)
    {

        std::string section="openGlass";
        std::string tcpClient="tcpClient";
        PropertiesPtr* clientDetails= config_.get<PropertiesPtr>(section, tcpClient);
        if (!clientDetails)
        {
            LOG_ERROR("Server Details not present in config.");
            throw std::runtime_error("Unable to initailize OpenGlass ");  // Throw an error later
        }

        tcpClient_.reset(new Fog::Network::TCPClient(*clientDetails));

        std::string packetizer="cpacketizer";
        PropertiesPtr* packetizerDetails= config_.get<PropertiesPtr>(section, packetizer);
        if (!packetizerDetails)
        {
            LOG_ERROR("Packetizer Details not present in config.");
            throw std::runtime_error("Unable to initailize OpenGlass ");  // Throw an error later
        }
        cmdPacketizer_.reset(new Fog::Network::CPacketizer(*packetizerDetails));

        // get the connections correct
      
        (tcpClient_->OnConnect).connect(boost::bind(&Fog::OpenGlass::operator(), this, _1));
        return true;
    }

    void run()
    {
        tcpClient_->startConnect();
        std::cin.get(); // TODO - how about a command thread wait!!
        tcpClient_->stop();
        tcpClient_->join();
    }
private:
    void operator()(TCPConnectionPtr conn)
    {
        // store connection for future send
        conn_=conn;
        // set up the linkages
        (conn->OnData).connect(boost::bind(&Fog::Network::CPacketizer::operator(), &(*cmdPacketizer_), _1, _2));
        (cmdPacketizer_->OnPacket).connect(boost::bind(&Fog::OpenGlass::onPacket, this, _1, _2));

        // send initial request
        sendRequest();
    }

    void sendRequest()
    {
        // create the reqest
//        std::string request("request { requestId=1 method=getTags args { protocol=arca }}");
        std::string request("request { requestid=1 protocolID=\"101\" method=getTags nor=10  resultset{ result=(\"100\" \"arca\" \"hello\") result2=(\"102\"  \"arca\" \"hello\")}}");


        int32_t reqLength=request.size();
        char length[4+1];
        memset(length, 0, sizeof(length));
        sprintf(length, "%04d", reqLength);
        request.insert(0, length);
        conn_->send(request.c_str(), request.size());
    }

    void onPacket(char* data, size_t size)
    {
        // read data and deserialize into a dataset
        // and then reply with some 

//        DataSet<TextSerializer> ds1;
        ds1.clear();
        std::cout  <<  "input data is - " << data << std::endl;
        ds1.deserialize(data);
        std::string arbit = ds1.serialize();

        std::cout  <<  "arbit is - " << arbit.c_str() << std::endl;

        std::string* proto= ds1.get<std::string>("request", "protocolID");
        if (proto)
            std::cout << *proto<< std::endl;
        else
        {
            std::cout << "protocolId not found"<< std::endl;
            return;
        }

        PropertiesPtr* resultSet = ds1.get<PropertiesPtr>("request", "resultset");
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
#if 0
        std::string response="request";
        std::string requestId="requestId";
        std::string tags="tags";
        int* requestIdPtr=input.get<int>(response, requestId);
        std::cout << " the request id is " << *requestIdPtr<< std::endl;
        StringsPtr* responseList=input.get<StringsPtr>(response, tags);
        if (responseList)
        {
            std::vector<std::string>::iterator tagIter=(*responseList)->begin();
            int i=0;
            while(tagIter != (*responseList)->end())
            {
                std::cout << " tag ["<< i << "] = " << *tagIter << std::endl;
                i++;
                ++tagIter;
            }
        }
#endif 
    }

    TCPClient tcpClient_;
    CmdPacketizer cmdPacketizer_;
//    DataSet<TextSerializer> config_;
    TCPConnectionPtr conn_;
    DataSet<TextSerializer> ds1;
};

}// fog

int main(int argc, char* argv[])
{
    try
    {
        Logger::create("sopeng.log");
        std::ifstream is;
        is.open("test_config");
        is.seekg(0, std::ios::end);
        long length=is.tellg();
        is.seekg(0, std::ios::beg);

        char* buffer=new char[length];
        is.read(buffer, length);

        std::string testConfig(buffer);

        DataSet<TextSerializer> config_;
        config_.deserialize(buffer); // store config datasset

        std::string section="openGlass";
#if 0
        std::string logger="logger";

        PropertiesPtr* loggerDetails= config_.get<PropertiesPtr>(section, logger);
        if (!properties)
        {
            LOG_ERROR("logger Details not present in config.");
            throw std::runtime_error("Unable to initailize OpenGlass ");  // Throw an error later
        }

        Properties::iterator ret;
        ret=(*properties)->find(std::string("logFile"));
        if (ret == properties->end())
        {
            LOG_ERROR("Logger filename not present in config file");
            throw std::runtime_error("Logger filename not found in config file");  // Throw an error later
        }
        else
        {
            std::string* logFile=boost::any_cast<std::string>(&(ret->second));
            if (logFile == NULL)
            {
                LOG_ERROR("recvLength not an integer value");
                return false;
            }

        }
#endif 
        Fog::OpenGlass openg;
        openg.initialize(config_);
        openg.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    catch (...)
    {
        std::cerr << "Unknown Exception: in main" <<"\n";
    }
}
