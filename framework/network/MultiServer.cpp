#include "MultiServer.h"

namespace Fog {
namespace Network {

// --------------------------------------------------------------------------
/// @brief Class provides serial tcp server function. Only accepts a single
//connection and will close the acceptor. If client disconnected will again go
//back to listenig on the same port for next connection
//
// ----------------------------------------------------------------------------

    MultiServer::MultiServer(PropertiesPtr properties)
	:   ioServicePtr_(new ba::io_service),
	    workPtr_(new ba::io_service::work(*ioServicePtr_)),
		acceptorPtr_(new bai::tcp::acceptor(*ioServicePtr_)),
        isStopped_(false)
	{
        if (!initialize(properties))
            throw std::runtime_error("Unable to initailize TCPServer");  // Throw an error later

	}

    MultiServer::~MultiServer()
    {
        if (! isStopped_)
            stop();
    }

 	void MultiServer::join() { serviceThreads_.join_all(); }

	void MultiServer::startAccept()
	{
		if (ioServicePtr_.get())
		{
			acceptorPtr_->open( endpoint_->protocol() );
			acceptorPtr_->set_option( boost::asio::ip::tcp::acceptor::
						  reuse_address(true) );
			acceptorPtr_->bind( *endpoint_ );
			acceptorPtr_->listen(1);
			clientConnPtr_.reset(new TCPConnection(ioServicePtr_, connProperties_));
			acceptorPtr_->async_accept( clientConnPtr_->getSocket(), 
				          boost::bind(&MultiServer::onAccept, 
				              this, clientConnPtr_, _1) );
	
			std::cout << "Listening on: " << *endpoint_ << std::endl;
		}
		else
		{
			std::cout << "Cannot start accepting without io_service" << std::endl;
		}
	}

	void MultiServer::stop ()
	{
		std::cout << "Stopping io_service " << std::endl;
		workPtr_.reset();
		ioServicePtr_->stop();

        // waiting for all service threads to join
        join();
        isStopped_=true;
	}
	
	void MultiServer::reset()
	{
		(*ioServicePtr_).reset();
		workPtr_.reset(new ba::io_service::work(*ioServicePtr_));
	}

    bool MultiServer::initialize(PropertiesPtr properties)
    {
        Properties::iterator ret;
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
                LOG_ERROR("hdrLength not an integer value");
                return false;
            }

            port_=*port;
        }

        endpoint_.reset(new bai::tcp::endpoint(bai::tcp::v4(), port_) );
        std::ostringstream tmp;
        tmp << "Iterative Server initialized with port " << port_;
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

        ret= properties->find(std::string("noOfThreads"));
        if (ret == properties->end())
        {
            LOG_ERROR("noOfThreads not present in config file");
            return false;
        }
        else
        {
            int* noOfThreads=boost::any_cast<int>(&(ret->second));
            if (noOfThreads== NULL)
            {
                LOG_ERROR("noOfThreads is not an integer value");
                return false;
            }

            for( int x = 0; x < *noOfThreads; ++x )
            {
                serviceThreads_.create_thread( boost::bind (&MultiServer::serviceThread, this));
            }
        }

        return true;
    }

	void MultiServer::onAccept( TCPConnectionPtr 	session,
			       const boost::system::error_code& error)
	{
		if (! error)
		{
            OnAccept(session);
		    std::cout << "Starting to read from socket" << std::endl;
		    session->recv();
            // TODO - this needs to be encapsulated somewhere
            // For going back to listening for the next connection
			clientConnPtr_.reset(new TCPConnection(ioServicePtr_, connProperties_));
			acceptorPtr_->async_accept( clientConnPtr_->getSocket(), 
				          boost::bind(&MultiServer::onAccept, 
				              this, clientConnPtr_, _1) );
		}
        else 
        {
            static uint16_t num_of_retrials=0;
            std::cout << "Error while accepting. Error - " << error << std::endl;
            if (num_of_retrials < 10 )
            {
                std::cout << "Trial # " << num_of_retrials << std::endl;
                startAccept();
                num_of_retrials++;
            }
        }
	}

    // @brief We create a new thread to handle all events happening on the
    // socket
	void MultiServer::serviceThread()
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
