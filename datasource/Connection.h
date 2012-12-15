#ifndef CONNECTION_H
#define CONNECTION_H

#include <boost/shared_ptr.hpp>
#include <pqxx/connection>
#include <boost/thread/mutex.hpp>

typedef boost::shared_ptr<pqxx::connection>  ConnPtr;
namespace Fog{
namespace Datasource{

class Connection
{
    public:
    static Connection* getInstance();
    ConnPtr getConnection() {return dbConn_;}
private:
    Connection();
    Connection(const Connection&);
    Connection& operator=(Connection& );

    static Connection* conn_;
    ConnPtr dbConn_;
};

} // datasource
} // fog

#endif 
