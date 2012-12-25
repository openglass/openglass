#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <iostream>
#include <stdexcept>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/signal.hpp>

#include "common/Logger.h"
#include "TCPConnection.h"

namespace ba=boost::asio;
namespace bai=boost::asio::ip;

typedef boost::shared_ptr<ba::io_service>     IOServicePtr;
typedef boost::shared_ptr<bai::tcp::acceptor> AcceptorPtr;
typedef boost::shared_ptr<ba::io_service::work> WorkPtr;
typedef boost::shared_ptr<bai::tcp::endpoint> EndpointPtr;

namespace Fog {
namespace Network {

// --------------------------------------------------------------------------
/// @brief Class provides serial tcp server function. Only accepts a single
//connection and will close the acceptor. If client disconnected will again go
//back to listenig on the same port for next connection
//
// ----------------------------------------------------------------------------
class TCPClient
{

public:
  typedef boost::shared_ptr<TCPConnection> TCPConnectionPtr;

  TCPClient(PropertiesPtr properties);
  // --------------------------------------------------------------------------
  /// @brief signal will be emitted when new connection is accepted
  // ----------------------------------------------------------------------------
  boost::signal< void (TCPConnectionPtr) > OnConnect;

  void join();

  void startConnect();

  void stop ();
  void reset();
  WorkPtr		getWork()		{return workPtr_;}
  IOServicePtr	getIOService() 	{return ioServicePtr_;}
	
private:
  bool initialize(PropertiesPtr properties);
  void onConnect( TCPConnectionPtr 	session,
                  const boost::system::error_code& error);

  /*
  * @brief We create a new thread to handle all events happening on the
  * socket
  */
  void serviceThread();

  IOServicePtr 		    ioServicePtr_;
  WorkPtr				workPtr_;
  EndpointPtr           endpoint_;
  TCPConnectionPtr 	    clientConnPtr_;
  PropertiesPtr         connProperties_;
  int                   port_;
  std::string           ip_;
  int                   noOfRetrials_;
  int                   waitSeconds_;
  boost::thread         serviceThread_;
};

} // Network
} // Fog

#endif  // TCPCLIENT_H
