#include "TCPConnection.h"

// --------------------------------------------------------------------------
/// @brief  This class encapsulates a tcp connection's state
//TODO - needs to be abstracted away to simply become a connection, whether udp
//or tcp should be based on the configuration it receives 
// ----------------------------------------------------------------------------

namespace Fog {
namespace Network {

boost::mutex global_stream_lock;

TCPConnection::TCPConnection( IOServicePtr io_service, PropertiesPtr properties)
		:socket_( *io_service )
	{
        if (!initialize(properties))
            throw std::runtime_error("Unable to initailize TCPConnection");  // Throw an error later
	}

	TCPConnection::~TCPConnection()
	{
        Close();
	}

    bool TCPConnection::initialize(PropertiesPtr properties) 
    {
        Properties::iterator ret;
        ret=properties->find(std::string("recvLength"));
        if (ret == properties->end())
        {
            LOG_ERROR("recvLength not present in config file");
            return false;
        }
        else
        {
            int* recvLength=boost::any_cast<int>(&(ret->second));
            if (recvLength== NULL)
            {
                LOG_ERROR("recvLength not an integer value");
                return false;
            }

            recvBufLen_=*recvLength;
        }

        recvBuffer_.reset(new char [recvBufLen_]);
        std::ostringstream tmp;
        tmp << "TCPConnection initialized with default recv size" << recvBufLen_;
        LOG_DEBUG(tmp.str());
        return true;
    }

	void TCPConnection::Close()
	{
		boost::system::error_code ec;
		socket_.shutdown( boost::asio::ip::tcp::socket::shutdown_both, ec );
		socket_.close( ec ); // ignored ec? !!!
	}

    void TCPConnection::onError(boost::system::error_code & ec)
    {
        // TODO - is every error worth closing the socket and going forward or
        std::ostringstream errorStr;
        errorStr << "[" << boost::this_thread::get_id()
            << "] Error: " << ec << std::endl;

        LOG_ERROR(errorStr.str());
        Close();
    }

	void TCPConnection::recv()
	{
        socket_.async_read_some( boost::asio::buffer(recvBuffer_.get(), 
                    recvBufLen_), 
			boost::bind( &TCPConnection::onRecv, shared_from_this(), _1, _2 ));
	}

	void TCPConnection::send( const void * buffer, size_t length )
	{
		bool can_send_now = false;

		std::vector< boost::uint8_t > output;
		std::copy( (const boost::uint8_t *)buffer, 
                (const boost::uint8_t *)buffer + length, 
                std::back_inserter( output ) );

		// Store if this is the only current send or not
		can_send_now = sendBuffer_.empty();

		// Save the buffer to be sent
		sendBuffer_.push_back( output );

		// Only send if there are no more pending buffers waiting!
		if( can_send_now )
		{
			// Start the next pending send
			boost::asio::async_write( socket_,
				boost::asio::buffer(sendBuffer_.front() ), 
				boost::bind( 
					&TCPConnection::onSend, 
					shared_from_this(), 
					boost::asio::placeholders::error, 
					sendBuffer_.begin()
				)
			);
		}
	}

	void TCPConnection::onRecv( const boost::system::error_code & ec,
                                        std::size_t bytesTx )
	{
		if( ec )
		{
            std::ostringstream errorStr;
            errorStr << "[" << boost::this_thread::get_id()
                << "] Error in receiving data. Error: " << ec << std::endl;

            LOG_ERROR(errorStr.str());
            return;
		}
		else
		{
            // raise onData for clients of TCPConnection to use this data
            OnData(recvBuffer_.get(), bytesTx);

            recv(); // go back to reading more
		}
	}

	void TCPConnection::onSend( const boost::system::error_code & ec,
            SendBuffers::iterator itr )
	{
		if( ec )
		{
            std::ostringstream errorStr;
            errorStr << "[" << boost::this_thread::get_id()
                << "] Error: " << ec << std::endl;

            LOG_ERROR(errorStr.str());
            return;
		}
		else
		{
            std::ostringstream debugStr;
            debugStr << "[" << boost::this_thread::get_id()
				<< "] Sent " << (*itr).size() 
                << " bytes." << std::endl;
            LOG_DEBUG(debugStr.str());
		}
		sendBuffer_.erase( itr );

		// Start the next pending send
		if( !sendBuffer_.empty() )
		{
			boost::asio::async_write( 
			    socket_,
				boost::asio::buffer( sendBuffer_.front() ), 
				boost::bind( &TCPConnection::onSend, 
					this, boost::asio::placeholders::error, 
					sendBuffer_.begin())
			);
		}
	}

} // Network
} // Fog
