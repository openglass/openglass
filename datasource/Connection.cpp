#include "Connection.h"

#include <pqxx/transaction>
#include <stdexcept>
#include <exception>

namespace Fog{
namespace Datasource{

Connection* Connection::conn_= NULL;

Connection* Connection::getInstance()
{
    if (!conn_)
    {
        static boost::mutex guard_;
        // lock 
        //guard_.lock();
        if (!conn_)
            conn_ = new Connection();
       // guard_.unlock();
    }
    return conn_;
}

Connection::Connection()
{
    try
    {
        // TODO - ideally we will be taking in the DataSet here and setting the
        // stuff
        dbConn_.reset(new pqxx::connection("dbname=OpenGlass user=Admin password=admin"));
        std::cout << "Connected to - " << dbConn_->dbname() << std::endl;
    }
	catch (const std::exception &e)
	{
        std::cout << e.what() << std::endl;
	}
    catch (...)
    {
        std::cout << "Unable to connect" << std::endl;
    }
}

} // Datasource
} // Fog
