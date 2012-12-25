#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include <iostream>
#include <vector>
#include <list>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/thread.hpp>
#include <boost/signal.hpp>

#include "common/Logger.h"
#include "common/CommonDefs.h"

// --------------------------------------------------------------------------
/// @brief  This class encapsulates a tcp connection's state
//TODO - needs to be abstracted away to simply become a connection, whether udp
//or tcp should be based on the configuration it receives 
// ----------------------------------------------------------------------------

namespace ba=boost::asio;
namespace bai=boost::asio::ip;

namespace Fog {
namespace Network {


typedef std::deque< std::vector< boost::uint8_t > > SendBuffers;
typedef boost::shared_ptr<ba::io_service> IOServicePtr;

class TCPConnection : public  boost::enable_shared_from_this< TCPConnection >
{
public:
	TCPConnection( IOServicePtr io_service, PropertiesPtr properties);

	~TCPConnection();

    bool initialize(PropertiesPtr properties);
	void Close();
    void onError(boost::system::error_code & ec);
	void recv();
	void send( const void * buffer, size_t length );
	boost::asio::ip::tcp::socket& getSocket() {return socket_; }

    // this signal raised when client gets disconnected from server
    boost::signal< void () > onDisconnect;

    // this signal raised when data is received
    boost::signal< void (char*, size_t ) > OnData;

private:
	virtual void onRecv( const boost::system::error_code & ec, 
                         std::size_t bytesTx );

	virtual void onSend( const boost::system::error_code & ec, 
                         SendBuffers::iterator itr );

    size_t recvBufLen_;
    boost::shared_array<char>  recvBuffer_;
	SendBuffers sendBuffer_;
	boost::asio::ip::tcp::socket socket_; 
};

} // Network
} // Fog

#endif // TPCONNECTION_H
