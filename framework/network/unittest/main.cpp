#include <boost/shared_ptr.hpp>

#include "network/TCPConnection.h"
#include "network/IterativeTCPServer.h"
#include "network/Packetizer.h"
#include "common/DataSet.h"
#include "common/TextSerializer.h"

namespace Fog {

class OpenGlass
{
public:
    typedef boost::shared_ptr<Fog::Network::TCPConnection> TCPConnectionPtr;
    typedef boost::shared_ptr<Fog::Network::IterativeTCPServer> TCPServer;
    typedef boost::shared_ptr<Fog::Network::CPacketizer> CmdPacketizer;
    OpenGlass()
    {
    }

    bool initialize(DataSet<TextSerializer>& config_)
    {
        std::string section="openGlass";
        std::string iterServer="tcpIterServer";
        PropertiesPtr* serverDetails = config_.get<PropertiesPtr>(section, iterServer);
        if (!serverDetails)
        {
            LOG_ERROR("Server Details not present in config.");
            throw std::runtime_error("Unable to initailize OpenGlass ");  // Throw an error later
        }

        tcpServer_.reset(new Fog::Network::IterativeTCPServer(*serverDetails));

        std::string packetizer="cpacketizer";
        PropertiesPtr* packetizerDetails= config_.get<PropertiesPtr>(section, packetizer);
        if (!packetizerDetails)
        {
            LOG_ERROR("Packetizer Details not present in config.");
            throw std::runtime_error("Unable to initailize OpenGlass ");  // Throw an error later
        }
        cmdPacketizer_.reset(new Fog::Network::CPacketizer(*packetizerDetails));

        // get the connections correct
      
        (tcpServer_->OnAccept).connect(boost::bind(&Fog::OpenGlass::OnAccept, this, _1));
        return true;
    }

    void run()
    {
        tcpServer_->startAccept();
        std::cin.get(); // TODO - how about a command thread wait!!
        tcpServer_->stop();
        tcpServer_->join();
    }
private:
    void OnAccept(TCPConnectionPtr conn)
    {
        conn_=conn;
        // set up the linkages
        (conn->OnData).connect(boost::bind(&Fog::Network::CPacketizer::operator(), &(*cmdPacketizer_), _1, _2));
        (cmdPacketizer_->OnPacket).connect(boost::bind(&Fog::OpenGlass::onPacket, this, _1, _2));
    }

    void onPacket(char* data, size_t size)
    {
//        DataSet<TextSerializer> ds1;
        ds1.clear();
        ds1.deserialize(data);
        std::string request("request");
        std::string requestId("requestId");
        std::string method("method");
        std::string args("args");

        std::string* met = ds1.get<std::string>("request", "method");
        if (met)
            std::cout << *met << std::endl;

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

        sendRequest(ds1);
    }

    void sendRequest(DataSet<TextSerializer>& ds1)
    {
//        std::string request("request { requestid=1 protocolID=\"101\" method=getTags nor=10  resultset{ result=(\"100\" \"arca\" \"hello\") result2=(\"102\"  \"arca\" \"hello\")}}");
        std::string request=ds1.serialize();
        std::cout << "request - " << request.c_str() << std::endl;

        int reqLength=request.size();
        char length[4+1];
        memset(length, 0, sizeof(length));
        sprintf(length, "%04d", reqLength);
        request.insert(0, length);
        conn_->send(request.c_str(), request.size());
    }

    TCPServer tcpServer_;
    CmdPacketizer cmdPacketizer_;
    TCPConnectionPtr conn_;
    DataSet<TextSerializer> ds1;
};

}// fog

int main(int argc, char* argv[])
{
    try
    {
        Logger::create("openg.log");
        Fog::OpenGlass openg;
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
        if (!loggerDetails)
        {
            throw std::runtime_error("Unable to initailize OpenGlass ");  // Throw an error later
        }

        Properties::iterator ret;
        ret=(*loggerDetails)->find(std::string("logFile"));
        if (ret == (*loggerDetails)->end())
        {
            throw std::runtime_error("Logger filename not found in config file");  // Throw an error later
        }

        std::string* logFile=boost::any_cast<std::string>(&(ret->second));
        if (logFile == NULL)
        {
            throw std::runtime_error("Unable to extract logfile Name");  // Throw an error later
        }
#endif 

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
