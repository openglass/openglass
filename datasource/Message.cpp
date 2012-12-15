#include "Message.h"

namespace Fog {
namespace Datasource{

Message :: Message()
{
    req_        = "REQUEST";
    args_       = "ARGS";
    stat_       = "STATUS";
    statMsg_    = "STATMESSAGE";
    protocolId_ = "PROTOCOLID";
    messageId_  = "MESSAGEID";
    messageName_    = "MESSAGE";
    messageDesc_    = "DESCRIPTION";
    created_        = "CREATED";
    testCaseMId_    = "TESTCASEMSGID";
    tagInfoList_    = "TAGINFOLIST";

}

void Message :: initialize(DSptr)
{
        FC::CommandRouter::getInstance()->registerCmd( "getMessage", 10,
                                                       "Get All Messages",16,
                                   boost::bind(&Message::findAllMessages, this, _1));

        FC::CommandRouter::getInstance()->registerCmd( "addNewMessage", 13,
                                                       "Add a Message",13,
                                   boost::bind(&Message::addMessage, this, _1));

        FC::CommandRouter::getInstance()->registerCmd( "editMessage", 11,
                                                       "Edit Message",12,
                                   boost::bind(&Message::updateMessage, this, _1));

        FC::CommandRouter::getInstance()->registerCmd( "delMessage", 10,
                                                       "delete a message",16,
                                   boost::bind(&Message::delMessage, this, _1));

        FC::CommandRouter::getInstance()->registerCmd( "getTestCaseMsgTags", 18,
                                                       "get the tags for a test case message",36,
                                   boost::bind(&Message::getTestCaseMsgTagInfo, this, _1));

}

bool Message :: getTestCaseMsgTagInfo(DataSet<TextSerializer> &dSet)
{
    //Get the testcase message Id
    int* testMsgID  = dSet.get<int>(req_, testCaseMId_);
    if(! testMsgID)
    {
        dSet.add<std::string>(req_, stat_, "false");
        std::ostringstream oss;
        oss << "TestCaseMsgId not found" <<std::endl;
        dSet.add<std::string>(req_, statMsg_,  oss.str());
        LOG_ERROR(oss.str());
        return false;
    }

    MessageTagInfoListPtr msgTagInfoListPtr( new std::vector< TagInfoPtr> );
    if(! msgTagInfoListPtr)
    {
        dSet.add<std::string>(req_, stat_, "false");
        std::ostringstream oss;
        oss << "TagList not initialized" <<std::endl;
        dSet.add<std::string>(req_, statMsg_,  oss.str());
        LOG_ERROR(oss.str());
        return false;
    }

    Connection* conn=Connection::getInstance();
    ConnPtr dbHandle=conn->getConnection();

    // execute the transaction
    try {
        // giving it a scope
        {
            if(! dbHandle)
            {
                dSet.add<std::string>(req_, stat_, "false");
                std::ostringstream oss;
                oss << "Database connection not successful" <<std::endl;
                dSet.add<std::string>(req_, statMsg_,  oss.str());
                LOG_ERROR(oss.str());
                return false;

            }
            pqxx::work w(*dbHandle);

            std::ostringstream msgId;
            msgId << *testMsgID;

            std::string query = "select test_case_msg_tags_id, tag_name, dest_data_type, tag_length, justify_with, justify_leftright, "
                    "off_set, default_value, required, value_user from dbo.task_details where test_case_msgs_id = " + msgId.str();

            std::cout << "\t getTestCaseMsgTagInfo : QUERY = " << query<< std::endl;

            pqxx::result R = w.exec(query);

            for( pqxx::result::const_iterator row = R.begin();row != R.end(); row++)
            {
                TagInfoPtr tagInfo( new TagInfo);

                //pqxx::from_string(row[4].c_str(), tagInfo->justifyWith_);
                //tagInfo->justifyWith_           = row[4].as<char>();

                pqxx::from_string(row[0].c_str(), tagInfo->testCaseMsgTagId_); //int

                tagInfo->tagName_               = row[1].c_str();

                tagInfo->destDataType_          = row[2].c_str();

                pqxx::from_string(row[3].c_str(), tagInfo->tagLength_);

                tagInfo->justifyWith_           = (row[4].c_str())[0]; //char

                tagInfo->justifyLeftRight_      = (row[5].c_str())[0]; //char

                pqxx::from_string(row[6].c_str(), tagInfo->offset_);

                tagInfo->defaultValue_          = row[7].c_str();

                tagInfo->required_              = row[8].c_str();

                tagInfo->userValue_             = row[9].c_str();

                msgTagInfoListPtr->push_back(tagInfo);
            }

         } //scope

        dSet.add<MessageTagInfoListPtr>(req_, tagInfoList_, msgTagInfoListPtr);
        dSet.add<std::string>(req_, stat_, "true");
        return true;

        } //try
        catch(pqxx::broken_connection& e)
        {
            std::ostringstream oss;
            oss << "Connection to database broken try again" <<std::endl;
            dSet.add<std::string>(req_, stat_, "false");
            dSet.add<std::string>(req_, statMsg_,  oss.str());
            LOG_ERROR(oss.str());
            return false;

        }
        catch(pqxx::sql_error& e)
        {
            std::ostringstream oss;
            oss << e.what() << std::endl;
            dSet.add<std::string>(req_, stat_, "false");
            dSet.add<std::string>(req_, statMsg_,  std::string(e.what()));
            LOG_ERROR(oss.str());
            return false;
        }
        catch(...)
        {
            std::ostringstream oss;
            oss << "Unknown error while fetching data"
                << std::endl;
            dSet.add<std::string>(req_, stat_, "false");
            dSet.add<std::string>(req_, statMsg_,  oss.str());
            LOG_ERROR(oss.str());
            return false;
        }

        return true;
}

bool Message :: findAllMessages(DataSet<TextSerializer> &ds)
{
    //Response should be :
    //LEN REQUEST { REQUESTID=1 METHOD=getMessage ARGS{PROTOCOLID=ALL MESSAGEID=ALL} STATUS= [STATMESSAGE=]
    //                        [RESULTSET { R1=(PROTOCOLID MESSAGEID PROTOCOL MDESCRIPTION ) R2=(PROTOCOLID MESSAGEID PROTOCOL MDESCRIPTION ) ... }] }

    std::cout << "Entered findAllMessages";

    // extract pertinent values from Dataset

    std::string query;

    std::string* protId = NULL;
    protId = ds.get<std::string>(args_, protocolId_);
    if(! protId)
    {
        ds.add<std::string>(req_, stat_, "false");
        std::ostringstream oss;
        oss << "PROTOCOLID not found in the request" <<std::endl;
        ds.add<std::string>(req_, statMsg_,  oss.str());
        LOG_ERROR(oss.str());
        return false;
    }
    else
    {
        /* If ProtocolID is "ALL" then fetch all the messages of ALL the protocols*/
        if( strcmp((*protId).c_str(), "ALL") == 0 )
        {
            query = "SELECT M.protocol_msg_id, M.protocol_id, P.name, M.name, M.descr From dbo.protocol_message M, dbo.protocol P"
                         " WHERE M.protocol_id = P.Protocol_id"
                         " AND M.rec_act = 1 AND M.status = 1"
                         " Order by P.name, M.name";
        }
        /* If ProtocolID is not "ALL" then fetch all the messages for the particular protocol */
        else
        {
            query = "SELECT M.protocol_msg_id, M.protocol_id, P.name, M.name, M.descr From dbo.protocol_message M, dbo.protocol P"
                         " WHERE M.protocol_id = P.Protocol_id"
                    " AND P.Protocol_id = '" + *protId +
                    "' AND M.rec_act = 1 AND M.status = 1"
                         " Order by P.name, M.name";
        }
    }

    std::cout << "query = "<< query << std::endl;

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
                ds.add<std::string>(req_, statMsg_,  oss.str());
                LOG_ERROR(oss.str());
                return false;

            }
            pqxx::work w(*dbHandle);

            pqxx::result R = w.exec(query);

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
                StringsPtr record(new Strings());

                record->push_back(r[2].c_str());

                record->push_back(r[0].c_str());
                record->push_back(r[1].c_str());

                record->push_back(r[3].c_str());

                record->push_back(r[4].c_str());

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
        oss << "Unknown error while fetching data"
            << std::endl;
        ds.add<std::string>(req_, stat_, "false");
        ds.add<std::string>(req_, statMsg_,  oss.str());
        LOG_ERROR(oss.str());
        return false;
    }

    return true;
}


bool Message :: addMessage(DataSet<TextSerializer> &ds)
{
    //Response should be :
    //LEN RESPONSE { REQUESTID=1 METHOD=addNewMessage ARGS { PROTOCOLID= MESSAGE=message1 DESCRIPTION=”” CREATED=””
    //              /*DETAILED_DESC=*/ } STATUS=TRUE/FALSE [STATMESSAGE=] RESULTSET {   MESSAGEID=    } }

    std::cout << "Entered addMessage\n\n";

    // extract pertinent values from Dataset

    std::string* protId=NULL;
    protId=ds.get<std::string>(args_, protocolId_);
    if(!protId)
    {
        std::ostringstream oss;
        oss << "Protocol id not present" <<std::endl;
        ds.add<std::string>(req_, stat_, "false");
        ds.add<std::string>(req_, statMsg_,  oss.str());
        LOG_ERROR(oss.str());
        return false;
    }


    std::string* name=NULL;
    name=ds.get<std::string>(args_, messageName_);
    if(!name)
    {
        std::ostringstream oss;
        oss << "Message name not present" <<std::endl;
        ds.add<std::string>(req_, stat_, "false");
        ds.add<std::string>(req_, statMsg_,  oss.str());
        LOG_ERROR(oss.str());
        return false;
    }

    std::string* desc=NULL;
    desc=ds.get<std::string>(args_, messageDesc_);
    std::string* creat=NULL;
    creat=ds.get<std::string>(args_, created_);

    Connection* conn=Connection::getInstance();
    ConnPtr dbHandle=conn->getConnection();


    // execute the transaction
    try {
        // giving it a scope
        {
            pqxx::work w(*dbHandle);

            w.exec("INSERT INTO dbo.protocol_message (protocol_id, name, descr, created_by) values( " +
                    *protId +
                    ",'" +
                    *name  +
                    "','" +
                    *desc +
                   "','" +
                    *creat +
                    "')");

            w.commit();

            PropertiesPtr resultSet (new Properties());
            ds.add<PropertiesPtr>(req_, "RESULTSET", resultSet);

            std::cout<<"Before inserting message id\n\n";


            pqxx::result R = w.exec("SELECT nextval('dbo.protocol_message_protocol_msg_id_seq')");

            pqxx::result::const_iterator row = R.begin();
            if(row != R.end())
            {
                pqxx::tuple::const_iterator col = row->begin();
                if(col != row->end())
                    ds.add<int>("RESULTSET", messageId_, col->as<int>());
            }

            //pqxx::tuple row = R[0];

            ds.add<std::string>("RESULTSET", messageId_, "9");
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

bool Message :: updateMessage(DataSet<TextSerializer> &ds)
{
    //LEN REQUEST { REQUESTID=1 METHOD=editMessage ARGS { PROTOCOLID= MESSAGEID= MESSAGE=message1 DESCRIPTION=”” CREATED= }
    //              STATUS= [STATMESSAGE=]  }

    std::cout << "Entered updateMessage";

    // extract pertinent values from Dataset

    std::string* protocolId=ds.get<std::string>(args_, protocolId_);
    if(!protocolId)
    {
        std::ostringstream oss;
        oss << "Protocol id not present" <<std::endl;
        ds.add<std::string>(req_, stat_, "false");
        ds.add<std::string>(req_, statMsg_,  oss.str());
        LOG_ERROR(oss.str());
        return false;
    }

    std::string* messageId = NULL;
    messageId    = ds.get<std::string>(args_, messageId_);
    if(!messageId)
    {
        std::ostringstream oss;
        oss << "Message id not present" <<std::endl;
        ds.add<std::string>(req_, stat_, "false");
        ds.add<std::string>(req_, statMsg_,  oss.str());
        LOG_ERROR(oss.str());
        return false;
    }

    std::string* message=NULL;
    message         = ds.get<std::string>(args_, messageName_);

    std::string* create=NULL;
    create          = ds.get<std::string>(args_, created_);

    std::string* description = NULL;
    description     = ds.get<std::string>(args_, messageDesc_);

    // some of these fields are mandatory
    if (!message )
    {
        // set status as false and return
        ds.add<std::string>(req_, stat_, "false");
        ds.add<std::string>(req_, statMsg_,  "Message name not present");
        return false;
    }

    Connection* conn=Connection::getInstance();
    ConnPtr dbHandle=conn->getConnection();

    // execute the transaction
    try {
        // giving it a scope
        {
            pqxx::work w(*dbHandle);

           /* w.exec("Insert into dbo.protocol(name, version, type, status, rec_act, created_by, created_on)"
                   "Select name, version, type, status, 0, created_by, created_on"
                   "Where Protocol_ID = " + os.str());*/

            w.exec("UPDATE dbo.protocol_message SET protocol_id = " + *protocolId + " ,name = '" + *message + "' ,descr = '" + *description +
                   "' WHERE protocol_msg_id = " +
                   *messageId );

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
        oss << "Unknown error while deleting"
            << std::endl;
        ds.add<std::string>(req_, stat_, "false");
        ds.add<std::string>(req_, statMsg_,  oss.str());
        LOG_ERROR(oss.str());
        return false;
    }

    return true;
}

bool Message :: delMessage(DataSet<TextSerializer> &ds)
{

    //LEN REQUEST { REQUESTID=1 METHOD=delMessage ARGS { MESSAGEID= } STATUS= [STATMESSAGE=] }

    std::cout << "Entered delMessage";

    // extract pertinent values from Dataset

    std::string* messageId=ds.get<std::string>(args_, messageId_);
    if(!messageId)
    {
        std::ostringstream oss;
        oss << "Message id not present" <<std::endl;
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

            w.exec("UPDATE dbo.protocol_message SET rec_act=0 WHERE protocol_msg_id=" +
                    *messageId );

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
        oss << "Unknown error while deleting"
            << std::endl;
        ds.add<std::string>(req_, stat_, "false");
        ds.add<std::string>(req_, statMsg_,  oss.str());
        LOG_ERROR(oss.str());
        return false;
    }
    return true;
}

} //Datasource
} //Fog

