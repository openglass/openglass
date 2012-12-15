#include "Protocol.h"

namespace Fog {
namespace Datasource{

    Protocol::Protocol():
    req_("REQUEST"),
    args_("ARGS"),
    stat_("STATUS"),
    msg_("STATMESSAGE"),
    protocol_("PROTOCOLID"),
    protocolName_("PROTOCOL"),
    version_("VERSION"),
    type_("TYPE"),
    created_("CREATED"),
    resultSet_("RESULTSET")
    {
    }

    void Protocol::initialize(DSptr)
    {
            FC::CommandRouter::getInstance()->registerCmd( "getProtocol", 11, 
                                                           "Get All Protocols",17, 
                                       boost::bind(&Protocol::findAllProtocols, this, _1));

            FC::CommandRouter::getInstance()->registerCmd( "addProtocol", 11, 
                                                           "Add a protocols",14, 
                                       boost::bind(&Protocol::addProtocol, this, _1));

            FC::CommandRouter::getInstance()->registerCmd( "editProtocol", 12, 
                                                           "edit protocol",13, 
                                       boost::bind(&Protocol::updateProtocol, this, _1));

            FC::CommandRouter::getInstance()->registerCmd( "delProtocol", 11, 
                                                           "delete a protocol",17, 
                                       boost::bind(&Protocol::delProtocol, this, _1));
    }

/**
* @brief gateway class for protocol table
*/
    bool Protocol::findAllProtocols(DataSet<TextSerializer> & ds)
    {
        //Response should be :
        //LEN REQUEST { REQUESTID=1 METHOD=getProtocol ARGS{PROTOCOLID=ALL} STATUS= [STATMESSAGE=]
        //                        [RESULTSET { R1=(ID NAME TYPE VERSION ) R2=(ID NAME TYPE VERSION ) ... }] }

        std::cout << "Entered findAllProtocols";

        Connection* conn=Connection::getInstance();
        ConnPtr dbHandle=conn->getConnection();

        // execute the transaction
        try {
            // giving it a scope
            {
                if(! dbHandle)
                {
                    ds.add<std::string>(req_, stat_, "false");
                    std::ostringstream oss;
                    oss << "Database connection not successful" <<std::endl;
                    ds.add<std::string>(req_, msg_,  oss.str());
                    LOG_ERROR(oss.str());
                    return false;

                }
                pqxx::work w(*dbHandle);

                pqxx::result R = w.exec("SELECT protocol_id,name,type,version FROM dbo.protocol where status=1 and rec_act=1 order by protocol_id");

                //Check whether result is empty
                if(R.empty())
                {
                    ds.add<std::string>(req_, stat_, "true");
                    return true;
                }

                PropertiesPtr resultSet (new Properties());

                int i=1;
                for (pqxx::result::const_iterator r = R.begin();
                        r != R.end();
                        ++r)
                {
                    //name
                    StringsPtr record(new Strings());

                    record->push_back(r[1].c_str());

                    //type
                    record->push_back(r[2].c_str());

                    //version
                    record->push_back(r[3].c_str());

                    //ID
                    record->push_back(r[0].c_str());
                    std::ostringstream ostr;
                    ostr << "r" << i;
                    resultSet->insert(std::make_pair(ostr.str(), record));
                    i++;
                }

                ds.add<PropertiesPtr>("REQUEST", "RESULTSET", resultSet);
            }

            ds.add<std::string>(req_, stat_, "true");
            return true;
        }
        catch(pqxx::broken_connection& e)
        {
            std::ostringstream oss;
            oss << "Connection to database broken try again" <<std::endl;
            ds.add<std::string>(req_, stat_, "false");
            ds.add<std::string>(req_, msg_,  oss.str());
            LOG_ERROR(oss.str());
            return false;

        }
        catch(pqxx::sql_error& e)
        {
            std::ostringstream oss; 
            oss << e.what() << std::endl;
            ds.add<std::string>(req_, stat_, "false");
            ds.add<std::string>(req_, msg_,  std::string(e.what()));
            LOG_ERROR(oss.str());
            return false;
        }
        catch(...)
        {
            std::ostringstream oss;
            oss << "Unknown error while inserting" 
                << std::endl;
            ds.add<std::string>(req_, stat_, "false");
            ds.add<std::string>(req_, msg_,  oss.str());
            LOG_ERROR(oss.str());
            return false;
        }

        return true;
    }

    /**
     * @brief Data set will contain all the values required to create a row
     */
    bool Protocol::addProtocol(DataSet<TextSerializer> & ds)
    {
        //Response should be :
        //LEN REQUEST { REQUESTID=1 METHOD=addProtocol ARGS { PROTOCOL=arcabook VERSION=6.0 CREATED= }
        //                  STATUS=TRUE/FALSE [STATMESSAGE=] RESULTSET {   PROTOCOLID=    } }

        std::cout << "Entered addProtocol\n";

        // extract pertinent values from Dataset

        std::string* prot=NULL;
        prot=ds.get<std::string>(args_, protocolName_);
        std::string* ver=NULL;
        ver=ds.get<std::string>(args_, version_);
        std::string* create=NULL;
        create=ds.get<std::string>(args_, created_);
        std::string* type = NULL;
        type = ds.get<std::string>(args_, type_);

        // some of thes fields are mandatory
        if (!prot || !ver || !create )
        {
            // set status as false and return
            ds.add<std::string>(req_, stat_, "false");
            ds.add<std::string>(req_, msg_,  "Incomplete args");
            return false;
        }
        Connection* conn=Connection::getInstance();
        ConnPtr dbHandle=conn->getConnection();


        // execute the transaction
        try {
            // giving it a scope
            {
                pqxx::work w(*dbHandle);

                w.exec("INSERT INTO dbo.protocol (name, version, type, created_by) values( '" +
                        *prot +
                       "' , '" +
                       *ver  +
                       "' , '" +
                       *type +
                       "' , '"+
                        *create +
                        "' )");

                w.commit();

                PropertiesPtr resultSet (new Properties());
                ds.add<PropertiesPtr>(req_, "RESULTSET", resultSet);
/*
                std::cout<<"Getting currval protocol id\n\n";

                //pqxx::result R = w.exec("SELECT currval(dbo.protocol_protocol_id_seq)");

                pqxx::result R = w.exec(pg_get_serial_sequence('dbo.protocol', 'protocol_id');
                pqxx::result::const_iterator row = R.begin();
                if(row != R.end())
                {
                    std::cout<< " The currval = " << row[0].c_str();

                    ds.add<std::string>(resultSet_, protocol_, (row[0]).c_str());
                }
*/
                /////////////////change this
                ds.add<int>("RESULTSET", protocol_, 7);
            }
            ds.add<std::string>(req_, stat_, "true");
            return true;
        }
        catch(pqxx::broken_connection& e)
        {
            std::ostringstream oss;
            oss << "Connection to database broken try again" <<std::endl;
            ds.add<std::string>(req_, stat_, "false");
            ds.add<std::string>(req_, msg_,  oss.str());
            LOG_ERROR(oss.str());
            return false;
        }
        catch(pqxx::sql_error& e)
        {
            std::ostringstream oss; 
            oss << e.what() << std::endl;
            ds.add<std::string>(req_, stat_, "false");
            ds.add<std::string>(req_, msg_,  std::string(e.what()));
            LOG_ERROR(oss.str());
            return false;
        }
        catch(...)
        {
            std::ostringstream oss;
            oss << "Unknown error while inserting" 
                << std::endl;
            ds.add<std::string>(req_, stat_, "false");
            ds.add<std::string>(req_, msg_,  oss.str());
            LOG_ERROR(oss.str());
            return false;
        }
        return true;
    }

    /**
     * @brief Data set will contain the new values
     */
    bool Protocol::updateProtocol(DataSet<TextSerializer> & ds)
    {
        //Response should be:
        //LEN REQUEST { REQUESTID=1 METHOD=editProtocol ARGS { PROTOCOLID= PROTOCOL=arcabook VERSION=6.0 TYPE= CREATED= }
        //              STATUS= [STATMESSAGE=]  }

        std::cout << "Entered updateProtocol\n";

        // extract pertinent values from Dataset

        std::string* protocolId=ds.get<std::string>(args_, protocol_);
        if(!protocolId)
        {
            std::ostringstream oss;
            oss << "Protocol id not present" <<std::endl;
            ds.add<std::string>(req_, stat_, "false");
            ds.add<std::string>(req_, msg_,  oss.str());
            LOG_ERROR(oss.str());
            return false;
        }

        std::string* protName = NULL;
        protName    = ds.get<std::string>(args_, protocolName_);

        std::string* ver=NULL;
        ver         = ds.get<std::string>(args_, version_);

        std::string* create=NULL;
        create      = ds.get<std::string>(args_, created_);

        std::string* type = NULL;
        type        = ds.get<std::string>(args_, type_);

        // some of thes fields are mandatory
        if (!protName || !ver || !type )
        {
            // set status as false and return
            ds.add<std::string>(req_, stat_, "false");
            ds.add<std::string>(req_, msg_,  "Incomplete args");
            return false;
        }

        Connection* conn=Connection::getInstance();
        ConnPtr dbHandle=conn->getConnection();

        // execute the transaction
        try {
            // giving it a scope
            {
                pqxx::work w(*dbHandle);

                //std::ostringstream oId;
                //oId << *protocolId;

                //std::ostringstream oVer;
                //oVer << *ver;

               /* w.exec("Insert into dbo.protocol(name, version, type, status, rec_act, created_by, created_on)"
                       "Select name, version, type, status, 0, created_by, created_on"
                       "Where Protocol_ID = " + os.str());*/

                w.exec("UPDATE dbo.protocol SET name = '" + *protName + "' ," +
                       "version = '" + *ver + "' ," +
                       "type = '" + *type + "'  " +
                       "WHERE protocol_id = " +
                       *protocolId /*oId.str()*/ );

                w.commit();

            }
            ds.add<std::string>(req_, stat_, "true");
            return true;
        }
        catch(pqxx::broken_connection& e)
        {
            std::ostringstream oss;
            oss << "Connection to database broken try again" <<std::endl;
            ds.add<std::string>(req_, stat_, "false");
            ds.add<std::string>(req_, msg_,  oss.str());
            LOG_ERROR(oss.str());
            return false;

        }
        catch(pqxx::sql_error& e)
        {
            std::ostringstream oss;
            oss << e.what() << std::endl;
            ds.add<std::string>(req_, stat_, "false");
            ds.add<std::string>(req_, msg_,  std::string(e.what()));
            LOG_ERROR(oss.str());
            return false;
        }
        catch(...)
        {
            std::ostringstream oss;
            oss << "Unknown error while deleting"
                << std::endl;
            ds.add<std::string>(req_, stat_, "false");
            ds.add<std::string>(req_, msg_,  oss.str());
            LOG_ERROR(oss.str());
            return false;
        }
        return true;
    }

    /**
     * @brief Data set will contain all the values required to create a row
     * ret 
     */
    bool Protocol::delProtocol(DataSet<TextSerializer> & ds) {

        //Response should be:
        //LEN REQUEST { REQUESTID=1 METHOD=delProtocol ARGS { PROTOCOLID= } STATUS=true/false [STATMESSAGE=] }

        std::cout << "Entered delProtocol\n";

        // extract pertinent values from Dataset

        std::string* protocolId=ds.get<std::string>(args_, protocol_);
        if(!protocolId)
        {
            std::ostringstream oss;
            oss << "Protocol id not present" <<std::endl;
            ds.add<std::string>(req_, stat_, "false");
            ds.add<std::string>(req_, msg_,  oss.str());
            LOG_ERROR(oss.str());
            return false;
        }

        Connection* conn=Connection::getInstance();
        ConnPtr dbHandle=conn->getConnection();

        // execute the transaction
        try {
            // giving it a scope
            {
                pqxx::work w(*dbHandle);

                //std::ostringstream opId;
                //opId << *protocolId;

               /* w.exec("Insert into dbo.protocol(name, version, type, status, rec_act, created_by, created_on)"
                       "Select name, version, type, status, 0, created_by, created_on"
                       "Where Protocol_ID = " + os.str()); */

                w.exec("UPDATE dbo.protocol SET status=0, rec_act = 0 WHERE protocol_id = '" +
                       *protocolId + "'");

                w.commit();

            }
            ds.add<std::string>(req_, stat_, "true");
            return true;
        }
        catch(pqxx::broken_connection& e)
        {
            std::ostringstream oss;
            oss << "Connection to database broken try again" <<std::endl;
            ds.add<std::string>(req_, stat_, "false");
            ds.add<std::string>(req_, msg_,  oss.str());
            LOG_ERROR(oss.str());
            return false;

        }
        catch(pqxx::sql_error& e)
        {
            std::ostringstream oss; 
            oss << e.what() << std::endl;
            ds.add<std::string>(req_, stat_, "false");
            ds.add<std::string>(req_, msg_,  std::string(e.what()));
            LOG_ERROR(oss.str());
            return false;
        }
        catch(...)
        {
            std::ostringstream oss;
            oss << "Unknown error while deleting"
                << std::endl;
            ds.add<std::string>(req_, stat_, "false");
            ds.add<std::string>(req_, msg_,  oss.str());
            LOG_ERROR(oss.str());
            return false;
        }
        return true;}
} // DataSource
} 
