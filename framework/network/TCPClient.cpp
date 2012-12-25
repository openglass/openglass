#include "TCPClient.h"

namespace Fog {
namespace Network {

// --------------------------------------------------------------------------
/// @brief Class provides serial tcp server function. Only accepts a single
//connection and will close the acceptor. If client disconnected will again go
//back to listenig on the same port for next connection
//
// ----------------------------------------------------------------------------
    TCPClient::TCPClient(PropertiesPtr properties)
      : ioServicePtr_(new ba::io_service),
        workPtr_(new ba::io_service::work(*ioServicePtr_)),
        noOfRetrials_(10),
        waitSeconds_(1)

    {
        if (!initialize(properties))
            throw std::runtime_error("Unable to initailize TCPServer");  // Throw an error later

    }

  void TCPClient::join() { serviceThread_.join(); }

  void TCPClient::startConnect()
  {
      if (ioServicePtr_.get())
      {
        clientConnPtr_.reset(new TCPConnection(ioServicePtr_, connProperties_));
        (clientConnPtr_->getSocket()).async_connect( *endpoint_, 
                       boost::bind(&TCPClient::onConnect, this, clientConnPtr_, _1) );

          std::cout << "Trying to connect to: " << *endpoint_ << std::endl;
      }
      else
      {
          std::cout << "Cannot start connecting without io_service" << std::endl;
      }
  }

  void TCPClient::stop ()
  {
      std::cout << "Stopping io_service " << std::endl;
      workPtr_.reset();
      ioServicePtr_->stop();
      clientConnPtr_->Close();
      join();
  }

  void TCPClient::reset()
  {
      (*ioServicePtr_).reset();
      workPtr_.reset(new ba::io_service::work(*ioServicePtr_));
  }

  bool TCPClient::initialize(PropertiesPtr properties)
  {
      Properties::iterator ret;
      ret=properties->find(std::string("ip"));
      if (ret == properties->end())
      {
          LOG_ERROR("port not present in config file");
          return false;
      }
      else
      {
          std::string* ip=boost::any_cast<std::string>(&(ret->second));
          if (ip == NULL)
          {
              LOG_ERROR("unable to extract ip address from confing file");
              return false;
          }

          ip_=*ip;
      }

      ret=properties->find(std::string("port"));
      if (ret == properties->end())
      {
          LOG_ERROR("port not present in config file");
          return false;
      }
      else
      {
          int* port=boost::any_cast<int>(&(ret->second));
          if (port == NULL)
          {
              LOG_ERROR("port not an integer value");
              return false;
          }

          port_=*port;
      }

      endpoint_.reset(new bai::tcp::endpoint(boost::asio::ip::address::from_string(ip_), port_) );
      std::ostringstream tmp;
      tmp << "Initialized endpoint " << *endpoint_;
      LOG_DEBUG(tmp.str());

      ret= properties->find(std::string("connDetails"));
      if (ret == properties->end())
      {
          LOG_ERROR("port not present in config file");
          return false;
      }
      else
      {
          PropertiesPtr* connDetails=boost::any_cast<PropertiesPtr>(&(ret->second));
          if (connDetails== NULL)
          {
              LOG_ERROR("hdrLength not an integer value");
              return false;
          }
          connProperties_=*connDetails;
      }

      serviceThread_=boost::thread(&TCPClient::serviceThread, this);
      return true;
  }

  void TCPClient::onConnect( TCPConnectionPtr 	session,
                  const boost::system::error_code& error)
  {
      if (! error)
      {
          std::ostringstream tmp;
          tmp << "Connected to " << *endpoint_;
          LOG_ERROR(tmp.str());
          session->recv(); // this starts the recv loop
          OnConnect(session);
      }
      else 
      {
          static int fails=0;
          std::ostringstream tmp;
          tmp << "Error -" << error << " while connecting to " << *endpoint_;
          tmp << "retry -" << fails << " out of " << noOfRetrials_;
          LOG_ERROR(tmp.str());

          if (fails > noOfRetrials_)
          {
              fails=0;
//              stop();
          }
          else
          {
              fails++;
              sleep(waitSeconds_);
              startConnect();
          } 
      }
  }

  void TCPClient::serviceThread()
  {
      while( true )
      {
          std::cout << "Starting connection thread - [" << boost::this_thread::get_id()
              << "]" << std::endl;
          boost::system::error_code ec;
          (*ioServicePtr_).run(ec);
          if( ec )
          {
              std::ostringstream errorStr;
              errorStr << "[" << boost::this_thread::get_id()
                  << "] Error: " << ec << std::endl;
              LOG_ERROR(errorStr.str());
          }
          break;
      }
      std::cout << "[" << boost::this_thread::get_id()
          << "] Connection Thread Finish" << std::endl;
  }

} // Network
} // Fog
