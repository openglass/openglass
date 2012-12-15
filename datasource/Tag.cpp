#include "Tag.h"

namespace Fog {

namespace Datasource {

Tag :: Tag()
{
    req_        = "REQUEST";
    args_       = "ARGS";
    generic_    = "GENERIC";
    nonGeneric_ = "NONGENERIC";
    state_      = "STATE";
    both_       = "BOTH";
    stat_       = "STATUS";
    statMsg_    = "STATMESSAGE";
    protocolId_ = "PROTOCOLID";
    messageId_  = "MESSAGEID";
    protMsgTagId_= "PROTOCOLMSGTAGID";
    tagId_      = "TAGID";
    tagName_    = "TAG";
    tagDesc_    = "DESCRIPTION";
    tagType_    = "TAGTYPE";
    tagLen_     = "TAGLENGTH";
    tagValue_   = "DEFVALUE";
    tagDestType_= "DEST_DATATYPE";
    tagJustify_ = "JUSTIFYWITH";
    tagEditable_= "EDITABLE";
    tagReq_     = "REQUIRED";
    tagOffset_  = "OFFSET";
    tagSeq_     = "SEQNO";
    created_    = "CREATED";
}

void Tag :: initialize(DSptr)
{
        FC::CommandRouter::getInstance()->registerCmd( "getTag", 6,
                                                       "Get All Tags",12,
                                   boost::bind(&Tag::findAllTags, this, _1));

        FC::CommandRouter::getInstance()->registerCmd( "addNewTag", 9,
                                                       "Add a Tag",9,
                                   boost::bind(&Tag::addTag, this, _1));

        FC::CommandRouter::getInstance()->registerCmd( "editTag", 7,
                                                       "Edit Tag",8,
                                   boost::bind(&Tag::updateTag, this, _1));

        FC::CommandRouter::getInstance()->registerCmd( "delTag", 6,
                                                       "delete a tag",12,
                                   boost::bind(&Tag::delTag, this, _1));
}


bool Tag :: findAllTags(DataSet<TextSerializer> &ds)
{
    //Response should be :
    //LEN REQUEST { REQUESTID=1 METHOD=getTag ARGS{PROTOCOLID=ALL MESSAGEID=ALL} STATUS= [STATMESSAGE=]
    //                        [RESULTSET { R1=(PROTOCOLID TAGID PROTOCOL TAG DESCRIPTION TAGLEN TAGTYPE DEST_DATATYPE JUSTIFY_WITH EDITABLE)
    //                        R2=(PROTOCOLID TAGID PROTOCOL TAG DESCRIPTION TAGLEN TAGTYPE DEST_DATATYPE JUSTIFY_WITH EDITABLE) ... }] }


    std::cout << "Entered findAllTags";

    // extract pertinent values from Dataset

    std::string query;

    std::string* protId = NULL;
    std::string* messageId = NULL;
    bool nongeneric = false;

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
        /* If ProtocolID is "ALL" then fetch all the tags */

        if( strcmp((*protId).c_str(), "ALL") == 0 )
            query = "Select T.Protocol_ID ,T.Protocol_All_Tags_ID, P.Name, T.Name , T.Descr, T.length, T.tag_type "
                    //",T.dest_data_type, T.justify_with, T.editable "
                    "From dbo.protocol_all_tags T, dbo.protocol P "
                    "Where T.Protocol_ID = P.Protocol_ID AND T.Rec_Act = 1 AND T.Status = 1 AND P.Rec_Act = 1 AND P.Status = 1 "
                    "Order by P.Name, T.Name";


        /* If ProtocolID is not "ALL" then check if the request is for tags for the particular message */
        else
        {
            messageId = ds.get<std::string>(args_, messageId_);
            if(! messageId)
            {
                ds.add<std::string>(req_, stat_, "false");
                std::ostringstream oss;
                oss << "MESSAGEID not found in the request" <<std::endl;
                ds.add<std::string>(req_, statMsg_,  oss.str());
                LOG_ERROR(oss.str());
                return false;
            }

            /* Request for tags in a particular protocol */
            if(strcmp((*messageId).c_str(), "ALL") == 0 )
            {
                query = "Select T.Protocol_ID, T.Protocol_All_Tags_ID, P.Name, T.Name, T.Descr, T.length, T.tag_type "
                        //",T.dest_data_type, T.justify_with, T.editable "
                            "From dbo.protocol_all_tags T, dbo.protocol P "
                            "Where T.Protocol_ID = P.Protocol_ID "
                            "AND T.Protocol_ID = " + *protId +
                            " AND T.Rec_Act = 1 AND T.Status = 1 AND P.Rec_Act = 1 AND P.Status = 1"
                            " Order by P.Name, T.Name";
            }
            /* Request for tags in a particular message */
            else
            {
                //LEN REQUEST { REQUESTID=1 METHOD=getTag ARGS{PROTOCOLID=ALL TAGID=ALL} STATUS= [STATMESSAGE=]
                //                        [RESULTSET { R1=(PROTOCOLID TAGID PROTOCOL TAG DESCRIPTION TAGLEN TAGTYPE DEST_DATATYPE JUSTIFY_WITH EDITABLE
                //                                          MESSAGEID, MESSAGE, OFFSET REQUIRED TAGVALUE SEQUENCE)
                //                        R2=(PROTOCOLID TAGID PROTOCOL TAG DESCRIPTION TAGLEN TAGTYPE DEST_DATATYPE JUSTIFY_WITH EDITABLE OFFSET REQUIRED TAGVALUE SEQUENCE) ... }] }

                query = "Select T.Protocol_ID, T.Protocol_All_Tags_ID, P.Name, T.Name, T.Descr, T.length, T.tag_type, "
                        //"T.dest_data_type, T.justify_with, T.editable, "
                        "M.Protocol_Msg_ID, M.Name, MT.Protocol_msg_tag_id, MT.off_set, MT.required, MT.default_value, MT.sequence "
                        "From dbo.protocol_all_tags T, dbo.protocol P, dbo.protocol_msg_tags MT, dbo.protocol_message M "
                        "Where MT.Protocol_Msg_ID = M.Protocol_Msg_ID "
                        "AND M.Protocol_ID = P.Protocol_ID "
                        "AND MT.Protocol_All_Tags_ID = T.Protocol_All_Tags_ID "
                        "AND MT.Protocol_Msg_ID = '" + *messageId +
                        "' AND T.Status = 1 AND T.Rec_Act = 1 AND P.Status = 1 AND P.Rec_Act = 1"
                        " AND MT.Status = 1 AND MT.Rec_Act = 1 AND M.Status = 1 AND M.Rec_Act = 1"
                        " Order by P.Name, T.Name";

                nongeneric = true;

            }
        }
    }

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

            std::cout << "QUERY = " << query;

            pqxx::result R = w.exec(query);

            //Check whether result is empty
            if(R.empty())
            {
                ds.add<std::string>(req_, stat_, "true");
                return true;
            }

            PropertiesPtr resultSet (new Properties());

            if(nongeneric)
            {
                int i=1;
                for (pqxx::result::const_iterator r = R.begin();
                        r != R.end();
                        ++r)
                {
                    StringsPtr record(new Strings());

    //                record->clear();

                    //protocolid
                    record->push_back(r[0].c_str());

                    //tagid
                    record->push_back(r[1].c_str());

                    //protocol name
                    record->push_back(r[2].c_str());

                    //tag name
                    record->push_back(r[3].c_str());

                    //Description
                   record->push_back(r[4].c_str());

                    //length
                    record->push_back(r[5].c_str());

                    //type
                    record->push_back(r[6].c_str());

                    /*Destination datatype
                    std::ostringstream ddvalue;
                    ddvalue << "\"" <<r[7].c_str() << "\"";
                    record->push_back(ddvalue.str());

                    //Justify with
                    std::ostringstream jwValue;
                    jwValue << "\"" <<r[8].c_str() << "\"";
                    record->push_back(jwValue.str());

                    //Editable
                    std::ostringstream eValue;
                    eValue << "\"" <<r[9].c_str() << "\"";
                    record->push_back(eValue.str());
    */
                    //Message specific parameters

                    //Message id
                    record->push_back(r[7].c_str());

                    //Message mname
                    record->push_back(r[8].c_str());

                    //Protocol_Msg_tag ID
                    record->push_back(r[9].c_str());

                    //Offset
                    record->push_back(r[10].c_str());

                    //Required
                    record->push_back(r[11].c_str());

                    //Tagvalue
                    record->push_back(r[12].c_str());

                    //Sequence
                    record->push_back(r[13].c_str());


                    std::ostringstream ostr;
                    ostr << "r" << i;
                    resultSet->insert(std::make_pair(ostr.str(), record));
                    i++;
                }
                ds.add<PropertiesPtr>("REQUEST", "RESULTSET", resultSet);

            } //non generic
            else
            {
                int i=1;
                for (pqxx::result::const_iterator r = R.begin();
                        r != R.end();
                        ++r)
                {
                    StringsPtr record(new Strings());

    //                record->clear();

                    //protocolid
                    record->push_back(r[0].c_str());

                    //tagid
                    record->push_back(r[1].c_str());

                    //protocol name
                    record->push_back(r[2].c_str());

                    //tag name
                    record->push_back(r[3].c_str());

                    //Description
                   record->push_back(r[4].c_str());

                    //length
                    record->push_back(r[5].c_str());

                    //type
                    record->push_back(r[6].c_str());

                    /*Destination datatype
                    std::ostringstream ddvalue;
                    ddvalue << "\"" <<r[7].c_str() << "\"";
                    record->push_back(ddvalue.str());

                    //Justify with
                    std::ostringstream jwValue;
                    jwValue << "\"" <<r[8].c_str() << "\"";
                    record->push_back(jwValue.str());

                    //Editable
                    std::ostringstream eValue;
                    eValue << "\"" <<r[9].c_str() << "\"";
                    record->push_back(eValue.str());
    */
                    std::ostringstream ostr;
                    ostr << "r" << i;
                    resultSet->insert(std::make_pair(ostr.str(), record));
                    i++;
                }
                ds.add<PropertiesPtr>("REQUEST", "RESULTSET", resultSet);

            } //generic

        } //scope

        ds.add<std::string>(req_, stat_, "true");
        return true;

    } //try
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

} //findAllTags



bool Tag :: addTag(DataSet<TextSerializer> &ds)
{
    //Response should be :
    //LEN RESPONSE { REQUESTID=1 METHOD=addNewTag STATE= [GENERIC { PROTOCOLID= TAG= DESCRIPTION= TYPE= LENGTH= CREATED=””
    //               }] [NONGENERIC { MESSAGEID= REQUIRED= OFFSET= SEQUENCE= VALUE= }]
    //                  [BOTH { } ] STATUS=TRUE/FALSE [STATMESSAGE=] RESULTSET {   TAGID=    } }

    int state = 0;
    PropertiesPtr* generic      = NULL;
    PropertiesPtr* nongeneric   = NULL;
    std::string* protId         = NULL;
    std::string* messageId      = NULL;
    std::string* tagId          = NULL;

    std::string* tagName;
    std::string* desc;
    std::string* type;
    std::string* length;
    std::string* created;
    //std::string* justify;
    //std::string* edit;
    //std::string* destDatatype;
    std::string* req;
    std::string* offset;
    std::string* sequence;
    std::string* value;

    std::cout << "Entered addTag";

    // extract pertinent values from Dataset
    // Check if, GENERIC or NONGENERIC or BOTH NONGENERIC and GENERIC properties are present in the request.
    //GENERIC => If the user adds the tag to a protocol
    //NONGENERIC => If the user adds the tag to a message

    std::string* strState = ds.get<std::string>(req_, state_);

    if(strState)
    {
        if (strcmp((*strState).c_str(), "GENERIC") == 0)
            state = GENERIC;
        else if (strcmp((*strState).c_str(), "NONGENERIC") == 0)
            state = NONGENERIC;
        else if (strcmp((*strState).c_str(), "BOTH") == 0)
            state = BOTH;
    }
    if( state == GENERIC || state == BOTH )
    {
        generic = ds.get<PropertiesPtr>(req_, generic_);
        if( ! generic )
        {
            std::ostringstream oss;
            oss << "generic args not present" <<std::endl;
            ds.add<std::string>(req_, stat_, "false");
            ds.add<std::string>(req_, statMsg_,  oss.str());
            LOG_ERROR(oss.str());
            return false;
        }
        std::cout<< " Generic State" << std::endl;

        protId=ds.get<std::string>(generic_, protocolId_);
        std::cout<< "Protocol ID " << *protId << std::endl;

        if(!protId)
        {
            std::cout<< " Protocol id not found" << std::endl;

            std::ostringstream oss;
            oss << "Protocol id not present" <<std::endl;
            ds.add<std::string>(req_, stat_, "false");
            ds.add<std::string>(req_, statMsg_,  oss.str());
            LOG_ERROR(oss.str());
            return false;
        }

        //get rest of the values
        tagName    = ds.get<std::string>(generic_, tagName_);
        desc       = ds.get<std::string>(generic_, tagDesc_);
        //destDatatype = ds.get<std::string>(generic_, tagDestType_);
        type       = ds.get<std::string>(generic_, tagType_);
        length     = ds.get<std::string>(generic_, tagLen_);
        //justify    = ds.get<std::string>(generic_, tagJustify_);
        //edit       = ds.get<std::string>(generic_, tagEditable_);
        created    = ds.get<std::string>(generic_, created_);

        if( !tagName || !type )
        {
            std::ostringstream oss;
            oss << "Incomplete arguments" <<std::endl;
            ds.add<std::string>(req_, stat_, "false");
            ds.add<std::string>(req_, statMsg_,  oss.str());
            LOG_ERROR(oss.str());
            return false;
        }

    }

    if( state == NONGENERIC || state == BOTH )
    {
        nongeneric = ds.get<PropertiesPtr>(req_, nonGeneric_);
        if(!nongeneric)
        {
            std::ostringstream oss;
            oss << "non-generic args not present" <<std::endl;
            ds.add<std::string>(req_, stat_, "false");
            ds.add<std::string>(req_, statMsg_,  oss.str());
            LOG_ERROR(oss.str());
            return false;
        }

        //Tag Id
        tagId = ds.get<std::string>(nonGeneric_, tagId_);
        if(!tagId)
        {
            std::cout<< " Tag id not found" << std::endl;

            std::ostringstream oss;
            oss << "Tag id not present" <<std::endl;
            ds.add<std::string>(req_, stat_, "false");
            ds.add<std::string>(req_, statMsg_,  oss.str());
            LOG_ERROR(oss.str());
            return false;
        }

        //MessageId
        messageId = ds.get<std::string>(nonGeneric_, messageId_);
        if(!messageId)
        {
            std::cout<< " Message id not found" << std::endl;

            std::ostringstream oss;
            oss << "Message id not present" <<std::endl;
            ds.add<std::string>(req_, stat_, "false");
            ds.add<std::string>(req_, statMsg_,  oss.str());
            LOG_ERROR(oss.str());
            return false;
        }

        req        = ds.get<std::string>(nonGeneric_, tagReq_);
        offset     = ds.get<std::string>(nonGeneric_, tagOffset_);
        sequence   = ds.get<std::string>(nonGeneric_, tagSeq_);
        value      = ds.get<std::string>(nonGeneric_, tagValue_);
        created    = ds.get<std::string>(nonGeneric_, created_);

        if( !sequence || !offset || !req )
        {
            std::ostringstream oss;
            oss << "Incomplete arguments" <<std::endl;
            ds.add<std::string>(req_, stat_, "false");
            ds.add<std::string>(req_, statMsg_,  oss.str());
            LOG_ERROR(oss.str());
            return false;
        }
    }


    Connection* conn=Connection::getInstance();
    ConnPtr dbHandle=conn->getConnection();


    // execute the transaction
    try {
        // giving it a scope
        {
            pqxx::work w(*dbHandle);

            //Update protocol_all_tags table
            if( state == GENERIC )
            {
                w.exec("insert into dbo.protocol_all_tags (protocol_id, name, descr, tag_type, length, created_by) values( "
                       + *protId + ",'" + *tagName + "','" + *desc + "','" + *type + "',"
                       + *length + ",'" + *created + "' )");

                w.commit();
                PropertiesPtr resultSet (new Properties());
                ds.add<PropertiesPtr>(req_, "RESULTSET", resultSet);

                ds.add<std::string>("RESULTSET", tagId_, "9");

            }
            //Update protocol_msg_tags
            else if( state == NONGENERIC )
            {
                w.exec("insert into dbo.protocol_msg_tags (protocol_all_tags_id, protocol_msg_id, required, off_set, sequence, default_value, created_by) values( "
                       + *tagId + "," + *messageId + ",'" + *req + "'," + *offset + "," + *sequence + ","
                       + *value + ",'" + *created + "' )");
                w.commit();

                PropertiesPtr resultSet (new Properties());
                ds.add<PropertiesPtr>(req_, "RESULTSET", resultSet);

                ds.add<std::string>("RESULTSET", protMsgTagId_, "9");

            }
            //Update protocol_all_tags and protocol_msg_tags table
            else if( state == BOTH )
            {
                w.exec("insert into dbo.protocol_all_tags (protocol_id, name, descr, tag_type, length, created_by) values( "
                       + *protId + ",'" + *tagName + "','" + *desc + "','" + *type + "',"
                       + *length + ",'" + *created + "' )");

                w.commit();

                //get the new tag ID from the table using currval/**tagId*/s

                w.exec("insert into dbo.protocol_msg_tags (protocol_all_tags_id, protocol_msg_id, required, off_set, sequence, default_value, created_by) values( 101,"
                       + *messageId + ",'" + *req + "'," + *offset + "," + *sequence + ","
                       + *value + ",'" + *created + "' )");

                w.commit();

                //get the new protocol_msg_tag ID from tha table currval

                PropertiesPtr resultSet (new Properties());
                ds.add<PropertiesPtr>(req_, "RESULTSET", resultSet);

                ds.add<std::string>("RESULTSET", tagId_, "9");
                ds.add<std::string>("RESULTSET", protMsgTagId_, "9");

            }

            std::cout<<"Before inserting message id\n\n";


            //pqxx::result R = w.exec("SELECT FROM currval(dbo.protocol_protocol_id_seq)");
            //int ret = 0;
            //pqxx::result::const_iterator row = R.begin();
            //if(row != R.end())
            //{
                //ds.add<std::string>(resultSet, protocol_, (row[0][0]).c_str());
            //}


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


bool Tag :: delTag(DataSet<TextSerializer> &ds)
{
    //LEN REQUEST { REQUESTID=1 METHOD=delTag ARGS { MESSAGEID= TAGID= } STATUS= [STATMESSAGE=] }

    std::cout << "Entered delTag";

    // extract pertinent values from Dataset

    std::string* protMsgTagId = NULL;
    bool removeAllReferences = false;

    std::string* tagId = ds.get<std::string>(args_, tagId_);
    if(!tagId)
    {
        std::ostringstream oss;
        oss << "Tag id not present" <<std::endl;
        ds.add<std::string>(req_, stat_, "false");
        ds.add<std::string>(req_, statMsg_,  oss.str());
        LOG_ERROR(oss.str());
        return false;
    }
    else
    {
        protMsgTagId = ds.get<std::string>(args_, protMsgTagId_);
        if(protMsgTagId)
        {

            //Delete tag from the particular protocol and all messages that adds it
            if(strcmp((*protMsgTagId).c_str(), "ALL") == 0)

                removeAllReferences = true;

            //Delete tag from the particular message only
            else

                removeAllReferences = false;
        }
     }

    Connection* conn=Connection::getInstance();
    ConnPtr dbHandle=conn->getConnection();

    // execute the transaction
    try {
        // giving it a scope
        {
            pqxx::work w(*dbHandle);

            //Delete tag from the particular protocol and all messages that adds it
            if (removeAllReferences)
            {
                w.exec("Update dbo.protocol_all_tags Set rec_act = 0 "
                       "Where Protocol_All_Tags_ID = "+ *tagId);

                w.commit();

            }
            //Delete tag from the particular message only
            else
            {
                w.exec("update dbo.protocol_msg_tags set rec_act = '0' where protocol_msg_tag_id = " + *protMsgTagId);

                w.commit();
            }

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

bool Tag :: updateTag(DataSet<TextSerializer> &ds)
{
    std::cout << "Entered updateTag";

    // extract pertinent values from Dataset

    std::string query;
    bool genericity             = false;
    PropertiesPtr* generic      = NULL;
    PropertiesPtr* nongeneric   = NULL;


    generic = ds.get<PropertiesPtr>(req_, generic_);
    if( ! generic )
    {
        nongeneric = ds.get<PropertiesPtr>(req_, nonGeneric_);
        if(!nongeneric)
        {
            std::ostringstream oss;
            oss << "Neither generic nor non-generic args present" <<std::endl;
            ds.add<std::string>(req_, stat_, "false");
            ds.add<std::string>(req_, statMsg_,  oss.str());
            LOG_ERROR(oss.str());
            return false;
        }
        else
            genericity = false; //Updating nongeneric features of a tag
    }
    else
        genericity = true;  //Updating generic features of a tag

    if(genericity)
    {
        //Extract values from GENERIC properties ptr
        std::string* tagId = ds.get<std::string>(generic_, tagId_);
        if(!tagId)
        {
            std::ostringstream oss;
            oss << "Tag id not present" <<std::endl;
            ds.add<std::string>(req_, stat_, "false");
            ds.add<std::string>(req_, statMsg_,  oss.str());
            LOG_ERROR(oss.str());
            return false;
        }
        std::string* tagName    = ds.get<std::string>(generic_, tagName_);
        std::string* desc       = ds.get<std::string>(generic_, tagDesc_);
        //std::string* destDatatype = ds.get<std::string>(generic_, tagDestType_);
        std::string* type       = ds.get<std::string>(generic_, tagType_);
        std::string* length     = ds.get<std::string>(generic_, tagLen_);
        //std::string* justify    = ds.get<std::string>(generic_, tagJustify_);
        //std::string* edit       = ds.get<std::string>(generic_, tagEditable_);

        if( !tagName || !type )
        {
            std::ostringstream oss;
            oss << "Incomplete arguments" <<std::endl;
            ds.add<std::string>(req_, stat_, "false");
            ds.add<std::string>(req_, statMsg_,  oss.str());
            LOG_ERROR(oss.str());
            return false;
        }

        query = "update dbo.protocol_all_tags set name = '" + *tagName + "', descr = '" + *desc + "', tag_type = '" + *type
                + "', length = '" + *length + "' where protocol_all_tags_id = '" + *tagId + "'";
    }
    else
    {
        //Extract values from NONGENERIC properties ptr
        std::string* protMsgTagId = ds.get<std::string>(nonGeneric_, protMsgTagId_);
        if(!protMsgTagId)
        {
            std::ostringstream oss;
            oss << "Protocol_Msg_Tag id not present" <<std::endl;
            ds.add<std::string>(req_, stat_, "false");
            ds.add<std::string>(req_, statMsg_,  oss.str());
            LOG_ERROR(oss.str());
            return false;
        }

        std::string* tagReq    = ds.get<std::string>(nonGeneric_, tagReq_);
        std::string* offset    = ds.get<std::string>(nonGeneric_, tagOffset_);
        //std::string* seq       = ds.get<std::string>(nonGeneric_, tagSeq_);
        std::string* value     = ds.get<std::string>(nonGeneric_, tagValue_);

        if( !tagReq || !offset || !value )
        {
            std::ostringstream oss;
            oss << "Incomplete arguments" <<std::endl;
            ds.add<std::string>(req_, stat_, "false");
            ds.add<std::string>(req_, statMsg_,  oss.str());
            LOG_ERROR(oss.str());
            return false;
        }

        /*  "', sequence = " + *seq
                + "', */
        query = "update dbo.protocol_msg_tags set required = '" + *tagReq + "', off_set = '" + *offset + "', default_value = '" + *value + "' where protocol_msg_tag_id = '" + *protMsgTagId + "'" ;
    }

    Connection* conn=Connection::getInstance();
    ConnPtr dbHandle=conn->getConnection();

    // execute the transaction
    try {
        // giving it a scope
        {
            pqxx::work w(*dbHandle);

            w.exec(query);

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

}
}

