#include <iostream>

//Boost header files
#include <boost/shared_ptr.hpp>

//Database pgsql header files
#include <pqxx/transaction>
#include <pqxx/result>

//OpenGlass framework header files
#include <common/DataSet.h>
#include <common/TextSerializer.h>
#include <common/CommonDefs.h>
#include "network/TCPConnection.h"
#include "network/IterativeTCPServer.h"
#include "network/Packetizer.h"

//Openglass Gateway header files
#include "Protocol.h"
#include "Message.h"
#include "Tag.h"
#include "Connection.h"

namespace fgd=Fog::Datasource;


namespace Fog {

class Command_Router
{

private:
    fgd::Protocol protocol_;
    fgd::Message message_;
    fgd::Tag tag_;

public:
    Command_Router(){};
    ~Command_Router(){};

    void routerFunc(DataSet <TextSerializer> &);

}; //Command_Router

void Command_Router :: routerFunc(DataSet<TextSerializer> &dataSet)
{
    bool result = false;

    std::string request("REQUEST");
    std::string requestId("REQUESTID");
    std::string method("METHOD");


    int* id = dataSet.get<int>(request, requestId);
    std::cout << " The request id is " << *id << std::endl;

    std::string* methodStr  = dataSet.get<std::string>(request, method);
    std::cout << " The method id is " << *methodStr << std::endl;

    if (strcmp((*methodStr).c_str(), "getProtocol") == 0)
    {
        //Check protocolID to determine all protocols or selected protocols
        //For now just calling all protocols

        result = protocol_.findAllProtocols(dataSet);

        std::cout << "The result from p.findAllProtocols is " << ((result)? "true": "false") << std::endl;
    }
    else if (strcmp((*methodStr).c_str(), "addProtocol") == 0)
    {
        result = protocol_.addProtocol(dataSet);

        std::cout << "The result from p.addProtocol is " << ((result)? "true": "false") << std::endl;
    }
    else if (strcmp((*methodStr).c_str(), "delProtocol") == 0)
    {
        result = protocol_.delProtocol(dataSet);

        std::cout << "The result from p.delProtocol is " << ((result)? "true": "false") << std::endl;
    }
    else if (strcmp((*methodStr).c_str(), "editProtocol") == 0)
    {
        result = protocol_.updateProtocol(dataSet);

        std::cout << "The result from p.updateProtocol is " << ((result)? "true": "false") << std::endl;
    }
    else if (strcmp((*methodStr).c_str(), "getMessage") == 0)
    {
        result = message_.findAllMessages(dataSet);

        std::cout << "The result from p.findAllMessages is " << ((result)? "true": "false") << std::endl;
    }
    else if (strcmp((*methodStr).c_str(), "addNewMessage") == 0)
    {
        result = message_.addMessage(dataSet);

        std::cout << "The result from addMessage is " << ((result)? "true": "false") << std::endl;
    }
    else if (strcmp((*methodStr).c_str(), "editMessage") == 0)
    {
        result = message_.updateMessage(dataSet);

        std::cout << "The result from updateMessage is " << ((result)? "true": "false") << std::endl;
    }
    else if (strcmp((*methodStr).c_str(), "delMessage") == 0)
    {
        result = message_.delMessage(dataSet);

        std::cout << "The result from delMessage is " << ((result)? "true": "false") << std::endl;
    }
    else if (strcmp((*methodStr).c_str(), "getTag") == 0)
    {
        result = tag_.findAllTags(dataSet);

        std::cout << "The result from findAllTags is " << ((result)? "true": "false") << std::endl;
    }
    else if (strcmp((*methodStr).c_str(), "editTag") == 0)
    {
        result = tag_.updateTag(dataSet);

        std::cout << "The result from updateTag is " << ((result)? "true": "false") << std::endl;
    }
    else if (strcmp((*methodStr).c_str(), "addNewTag") == 0)
    {
        //result = tag_.addTag(dataSet);

        std::cout << "The result from addTag is " << ((result)? "true": "false") << std::endl;
    }
    else if (strcmp((*methodStr).c_str(), "delTag") == 0)
    {
        result = tag_.delTag(dataSet);

        std::cout << "The result from delTag is " << ((result)? "true": "false") << std::endl;
    }
    else
    {
        LOG_ERROR("Method name - not found in the request");
    }

}


class Framework_Server
{

public:
    typedef boost::shared_ptr<Fog::Network::TCPConnection> TCPConnectionPtr;
    typedef boost::shared_ptr<Fog::Network::IterativeTCPServer> TCPServer;
    typedef boost::shared_ptr<Fog::Network::CPacketizer> CmdPacketizer;
    Framework_Server()
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

        (tcpServer_->OnAccept).connect(boost::bind(&Fog::Framework_Server::OnAccept, this, _1));
        return true;
    }

    void closeConnection()
    {
        tcpServer_->stop();
        tcpServer_->join();
    }

    void run()
    {
        std::cout << "Entered Run" << std::endl << std::endl;
        tcpServer_->startAccept();
        std::cin.get(); // TODO - how about a command thread wait!!
        closeConnection();
    }

private:

    Fog::Command_Router cmd_Router_;

    void OnAccept(TCPConnectionPtr conn)
    {
        conn_=conn;
        // set up the linkages
        (conn->OnData).connect(boost::bind(&Fog::Network::CPacketizer::operator(), &(*cmdPacketizer_), _1, _2));
        (cmdPacketizer_->OnPacket).connect(boost::bind(&Fog::Framework_Server::onPacket, this, _1, _2));

        //Start accepting another connection
        //tcpServer_->reset();
        //run();
    }

    void onPacket(char* data, size_t size)
    {

        LOG_DEBUG("****************************Received request from the client*******************************8");
        DataSet<TextSerializer> dataSet;

        LOG_DEBUG(data);

        dataSet.deserialize(data);

        cmd_Router_.routerFunc(dataSet);



        std::cout << "Sending response";
        sendRequest(dataSet);
    }

    void sendRequest(DataSet <TextSerializer> &dataSet)
    {
        /*std::string request("response{ requestid=1 method=getTags nor=10 resultset{ result=(\"arca\" \"100\" \"hello\") result2=( \"arca\" \"102\" \"hello\")}}");
        int reqLength=request.size();
        char length[4+1];
        memset(length, 0, sizeof(length));
        sprintf(length, "%04d", reqLength);
        request.insert(0, length);
        conn_->send(request.c_str(), request.size());
        */

        LOG_DEBUG("****************************Sending response to the client*******************************8");

        std::string request;
        char length[4+1];

        request             = dataSet.serialize();
        int reqLength       = request.size();

        LOG_DEBUG(request);

        memset(length, 0, sizeof(length));
        sprintf(length, "%04d", reqLength);
        request.insert(0, length);
        conn_->send(request.c_str(), request.size());

    }

    TCPServer tcpServer_;
    CmdPacketizer cmdPacketizer_;
    TCPConnectionPtr conn_;

}; //Framework_Server



}// fog

int main(int argc, char* argv[])
{
    try
    {

        Logger::create("openg.log");
        Fog::Framework_Server openg;
        std::ifstream is;
        is.open("test_config");
        is.seekg(0, std::ios::end);
        long length=is.tellg();
        is.seekg(0, std::ios::beg);

         std::cout<< "Going for initialize";

        char* buffer=new char[length];
        is.read(buffer, length);



        DataSet<TextSerializer> config_;
        config_.deserialize(buffer); // store config datasset

        std::cout<< "Going for initialize";

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



 
