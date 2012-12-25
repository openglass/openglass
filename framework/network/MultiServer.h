#ifndef MULTI_TCP_SERVER_H
#define MULTI_TCP_SERVER_H

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
class MultiServer
{
public:
  typedef boost::shared_ptr<TCPConnection> TCPConnectionPtr;

	MultiServer(PropertiesPtr properties);
    ~MultiServer();

    /**
     * @brief this singal is emitted when a new connection is established.
     * Please note that server doesn't keep any connections stored. Its the
     * responsibility of the client code to maintain these connections
     */
    boost::signal< void (TCPConnectionPtr) > OnAccept;

 	void join();

	void startAccept();
	
	void stop ();
	void reset();
	WorkPtr		getWork()			{return workPtr_;}
	IOServicePtr	getIOService() 	{return ioServicePtr_;}
	AcceptorPtr	getAcceptor() 		{return acceptorPtr_;}
	
private:
    bool initialize(PropertiesPtr properties);
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
    uint32_t                connectionId_;
    bool                    isStopped_;
};

} // Network
} // Fog

#endif  // MULTI_TCP_SERVER
