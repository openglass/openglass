#include "TestCaseMapper.h"

#include <boost/lexical_cast.hpp>

namespace Fog {
namespace Datasource{
    TestCaseMapper::TestCaseMapper()
    {
        req_        = "REQUEST";
        args_       = "ARGS";
        stat_       = "STATUS";
        statMsg_    = "STATMESSAGE";
    }

    void TestCaseMapper::initialize(DSptr)
    {
        FC::CommandRouter::getInstance()->registerCmd( "findTestMsgs", 12,
                                                       "Get All Msgs in a test case",27,
                                   boost::bind(&TestCaseMapper::findTestMsgs, this, _1));
#if 0
        FC::CommandRouter::getInstance()->registerCmd( "addNewTag", 9,
                                                       "Add a Tag",9,
                                   boost::bind(&Tag::addTag, this, _1));

        FC::CommandRouter::getInstance()->registerCmd( "editTag", 7,
                                                       "Edit Tag",8,
                                   boost::bind(&Tag::updateTag, this, _1));

        FC::CommandRouter::getInstance()->registerCmd( "delTag", 6,
                                                       "delete a tag",12,
                                   boost::bind(&Tag::delTag, this, _1));
#endif 
    }

    /**
     * @brief Data set will contain all the values required to create a row
     */
    bool TestCaseMapper::findTestMsgs(DataSet<TextSerializer> & ds)
    {
        //Response should be :
        //LEN REQUEST { REQUESTID=1 METHOD=findTestCaseMapper ARGS { testCaseId=id }
        //                  STATUS=TRUE/FALSE [STATMESSAGE=] RESULTSET=list<TestCaseInfo> }
        //

        std::cout << "Entered findTestMsgs\n";

        int* testCaseId=ds.get<int>(args_, "TESTID");
        if (!testCaseId)
        {
            std::ostringstream oss;
            oss << "TestCase id not present" <<std::endl;
            ds.add<std::string>(req_, stat_, "false");
            ds.add<std::string>(req_, statMsg_,  oss.str());
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
                char testIdStr [10+1]; 
                memset(testIdStr, 0, sizeof(testIdStr));
                sprintf(testIdStr, "%d", *testCaseId);


                std::ostringstream queryStream;
                queryStream << "SELECT DISTINCT test_case_msgs_id, message," << 
                               " message_seq, protocol_id, protocol_name, " <<
                               "ip, port" <<
                               " FROM dbo.task_details where test_case_id=" <<
                               *testCaseId << " ORDER BY message_seq";
                pqxx::result R = w.exec(queryStream.str());

                //Check whether result is empty
                if(R.empty())
                {
                    ds.add<std::string>(req_, stat_, "true");
                    return true;
                }

                TestMsgsListPtr testMsgsList (new std::vector<MsgInfoPtr> );

                for (pqxx::result::const_iterator r = R.begin(); r != R.end();
                        ++r)
                {
                    MsgInfoPtr testCaseInfo(new TestCaseMsgInfo);

                    //test_case_msgs_id
                    testCaseInfo->testCaseMsgId_=r[0].as<int>();

                    //msg name
                    testCaseInfo->msgName_=r[1].c_str();

                    // message_seq is not required except for ordering
                    // hence not extracting

                    //protocol id
                    testCaseInfo->protocolId_=r[3].as<int>();

                    //msg name
                    testCaseInfo->protocolName_=r[4].c_str();
                    
                    // protocol ip
                    testCaseInfo->protocolIp_=r[5].c_str();

                    // port num
                    testCaseInfo->portNum_=r[6].as<int>();

                    testMsgsList->push_back(testCaseInfo);
                }

                ds.add<TestMsgsListPtr>(req_, "RESULTSET", testMsgsList);
            }
            ds.add<std::string>(req_, stat_, "true");
            return true;
        }
        catch(pqxx::broken_connection& e)
        {
            std::ostringstream oss;
            oss << "Connection to database broken try again" <<std::endl;
            ds.add<std::string>(req_, stat_, "false");
            ds.add<std::string>(req_, statMsg_,  oss.str());
            LOG_ERROR(oss.str());
            return false;
        }
        catch(pqxx::sql_error& e)
        {
            std::ostringstream oss; 
            oss << e.what() << std::endl;
            ds.add<std::string>(req_, stat_, "false");
            ds.add<std::string>(req_, statMsg_,  std::string(e.what()));
            LOG_ERROR(oss.str());
            return false;
        }
        catch(...)
        {
            std::ostringstream oss;
            oss << "Unknown error while inserting" 
                << std::endl;
            ds.add<std::string>(req_, stat_, "false");
            ds.add<std::string>(req_, statMsg_,  oss.str());
            LOG_ERROR(oss.str());
            return false;
        }
        return true;
    }
#if 0

/**
* @brief gateway class for protocol table
*/
    bool TestCaseMapper::findAllTestCases()
    {
        //Response should be :
        //LEN REQUEST { REQUESTID=1 METHOD=getTestCaseMapper ARGS{PROTOCOLID=ALL} STATUS= [STATMESSAGE=]
        //                        [RESULTSET { R1=(ID NAME TYPE VERSION ) R2=(ID NAME TYPE VERSION ) ... }] }

        std::cout << "Entered findAllTestCaseMappers";

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
     * @brief Data set will contain the new values
     */
    bool TestCaseMapper::updateTestCaseMapper(DataSet<TextSerializer> & ds)
    {
        //Response should be:
        //LEN REQUEST { REQUESTID=1 METHOD=editTestCaseMapper ARGS { PROTOCOLID= PROTOCOL=arcabook VERSION=6.0 TYPE= CREATED= }
        //              STATUS= [STATMESSAGE=]  }

        std::cout << "Entered updateTestCaseMapper\n";

        // extract pertinent values from Dataset

        std::string* protocolId=ds.get<std::string>(args_, protocol_);
        if(!protocolId)
        {
            std::ostringstream oss;
            oss << "TestCaseMapper id not present" <<std::endl;
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
                       "Where TestCaseMapper_ID = " + os.str());*/

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
    bool TestCaseMapper::delTestCaseMapper(DataSet<TextSerializer> & ds) {

        //Response should be:
        //LEN REQUEST { REQUESTID=1 METHOD=delTestCaseMapper ARGS { PROTOCOLID= } STATUS=true/false [STATMESSAGE=] }

        std::cout << "Entered delTestCaseMapper\n";

        // extract pertinent values from Dataset

        std::string* protocolId=ds.get<std::string>(args_, protocol_);
        if(!protocolId)
        {
            std::ostringstream oss;
            oss << "TestCaseMapper id not present" <<std::endl;
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
                       "Where TestCaseMapper_ID = " + os.str()); */

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
#endif 
} // DataSource
} 
