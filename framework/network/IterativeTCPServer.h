#ifndef ITERATIVE_TCP_SERVER_H
#define ITERATIVE_TCP_SERVER_H

#include <iostream>
#include <vector>
#include <list>
#include <stdexcept>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/signal.hpp>

#include "common/Logger.h"
#include "TCPConnection.h"

namespace ba=boost::asio;
namespace bai=boost::asio::ip;

typedef boost::shared_ptr<ba::io_service> IOServicePtr;
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
class IterativeTCPServer
{
public:
  typedef boost::shared_ptr<TCPConnection> TCPConnectionPtr;

	IterativeTCPServer(PropertiesPtr properties);

    /// @brief signal will be emitted when new connection is accepted
    boost::signal< void (TCPConnectionPtr) > OnAccept;

 	void join();

	void startAccept();
    bool initialize(PropertiesPtr properties);
	
	void stop ();
	void reset();
	WorkPtr		getWork()			{return workPtr_;}
	IOServicePtr	getIOService() 	{return ioServicePtr_;}
	AcceptorPtr	getAcceptor() 		{return acceptorPtr_;}
	
private:

	void onAccept( TCPConnectionPtr 	session,
			       const boost::system::error_code& error);

    // @brief We create a new thread to handle all events happening on the
    // socket
	void serviceThread();
	IOServicePtr 		    ioServicePtr_;
	WorkPtr				    workPtr_;
    EndpointPtr             endpoint_;
	AcceptorPtr			    acceptorPtr_;
	TCPConnectionPtr 	    clientConnPtr_;
	PropertiesPtr           connProperties_;
    int                     port_;

    boost::thread_group     serviceThreads_;
};

} // Network
} // Fog

#endif  // ITERATIVE_TCP_SERVER_H
