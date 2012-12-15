#include "TestCase.h"
#include <boost/shared_array.hpp> 



namespace Fog {
namespace Model {

    TestCase::TestCase():testId_(0)
    {
        req_        = "REQUEST";
        args_       = "ARGS";
        stat_       = "STATUS";
        statMsg_    = "STATMESSAGE";
        cmdRouter_ = Fog::Common::CommandRouter::getInstance();
        logMsgFile_.open("rawMsgs.out", std::ios::out|std::ios::binary);
    }

    TestCase::~TestCase()
    {
        if (logMsgFile_.is_open())
            logMsgFile_.close();
    }

    bool TestCase::initialize(DataSet<TextSerializer>& ds)
    {
        std::cout << "Entered Open\n";
        // get all protocol id required in this test case
        // and create protocol class to pass to message creation

        int32_t * testIdPtr=ds.get<int>(args_, "TESTID");
        if (!testIdPtr)
        {
            std::ostringstream oss;
            oss << "TestCase id not present." <<std::endl;
            ds.add<std::string>(req_, stat_, "false");
            ds.add<std::string>(req_, statMsg_,  oss.str());
            LOG_ERROR(oss.str());
            return false;
        }

        testId_=*testIdPtr;

        // Use command router to get msgs from data soure layer
        DataSet <TextSerializer> dataSet(req_);

        //Add the args section
        PropertiesPtr argsProp(new Properties());
        argsProp->insert(std::make_pair<std::string, int>("TESTID", testId_));
        dataSet.add<PropertiesPtr>("REQUEST", args_, argsProp);

        cmdRouter_->invokeMethod("findTestMsgs", dataSet);

        TestMsgsListPtr* temp  = dataSet.get<TestMsgsListPtr>(req_, "RESULTSET");
        if(!temp)
        {
            std::ostringstream oss;
            oss << "did not receive msgs from ds layer." <<std::endl;
            ds.add<std::string>(req_, stat_, "false");
            ds.add<std::string>(req_, statMsg_,  oss.str());
            LOG_ERROR(oss.str());
            return false;
        }
        msgInfoList_= *temp;

        std::ostringstream str;
        str<< "The no: of messages in msglist " << msgInfoList_->size() << std::endl;
        LOG_DEBUG(str.str());

        MsgListIter iter = msgInfoList_->begin();
        MsgListIter end = msgInfoList_->end();
        while (iter != end)
        {
            int protoId= (*iter)->protocolId_;
            ProtocolPtr p;
            // check if protocol of this msg is already created
            std::map<int, ProtocolPtr>::iterator protoIter = protocolList_.find(protoId);;
            if (iter == end) // message of a new protocol
            {
                // Create protocol 
                ProtocolPtr proto (new Protocol(protoId,
                            (*iter)->msgName_,
                            (*iter)->protocolIp_,
                            (*iter)->portNum_));
                protocolList_.insert(std::make_pair(protoId, proto));
                p = proto;
            }
            else
            {
                p = protoIter->second;
            }

            // create message 
            MsgPtr msg(new Message((*iter)->testCaseMsgId_, (*iter)->msgName_, 
                                   testId_, p));
            if (msg->initialize())
            {
                std::cout << "in initialized message" << std::endl;
                msgs_.push_back(msg);
                if (logMsgFile_.is_open())
                    logMsgFile_.write(msg->getMsgBuffer().get(), msg->getMsgLen());
                else
                    std::cout << "unable to write messages" << std::endl;
            }
            else
            {
                std::ostringstream oss;
                oss << "Unable to initialize msg - " 
                    << (*iter)->msgName_
                    << "in test case - " << testId_ << std::endl;
                LOG_ERROR(oss.str());
                return false;
            }

            ++iter;
        }
        logMsgFile_.close();
        return true;
    }
} // Model
} // Fof
