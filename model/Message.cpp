#include "Message.h"

#include <iostream>
#include <sstream>
#include <string>

namespace Fog {
namespace Model {

    Message::Message( int testCaseMsgId, std::string& messageName, 
         int testCaseId, ProtocolPtr p) :
         testMsgId_(testCaseMsgId),
         msgName_(messageName),
         testId_(testCaseId), 
         protocol_(p),
         cmdRouter_(NULL),
         msgLen_(0)

{
    cmdRouter_ = Fog::Common::CommandRouter::getInstance();
    tagsList_.reset(new std::vector<TagPtr>);
}

Message :: ~Message()
{
}

bool Message :: initialize()
{
    //create DSPTR add int testMsgId_, OGTagInfo* tagInfo send to getTestCaseMsgTagInfo

    std::string request         = "REQUEST";
    std::string testCaseMId     = "TESTCASEMSGID";
    std::string tagInfoList     = "TAGINFOLIST";

    DataSet <TextSerializer> dataSet(request);

    dataSet.add<int>(request, testCaseMId, testMsgId_);

    //msgTagInfoListPtr_(new std::vector<TagInfoPtr>);

    //dataSet.add<MessageTagInfoListPtr>(request, tagInfoList, msgTagInfoListPtr_);

    //Call the datasource message::getTestCaseMsgTagInfo to get all the tag in a testcase message
    cmdRouter_->invokeMethod("getTestCaseMsgTags", dataSet);

    MessageTagInfoListPtr* temp  = dataSet.get<MessageTagInfoListPtr>(request, tagInfoList);
    msgTagInfoListPtr_           = *temp;

    std::ostringstream str;
    str<< "The no: of tags in the message " << msgTagInfoListPtr_->size() << std::endl;
    LOG_DEBUG(str.str());

    // iterate through all tags and instantiate corresponding tag object
    TagInfoIter tagInfoIter = msgTagInfoListPtr_->begin();
    TagInfoIter endTagsIter = msgTagInfoListPtr_->end();

    while (tagInfoIter != endTagsIter)
    {
        // create Tag
        TagPtr tag (new Tag(*tagInfoIter));

        // initialize Tag
        if (!tag->initialize())
        {
            std::ostringstream logStr;
            logStr << "Unable to initialize tag - "
                   << (*tagInfoIter)->tagName_ 
                   << " in message - " 
                   << msgName_ 
                   << " in test case id - " 
                   << testId_;
            LOG_ERROR(logStr.str());
            return false; // no need to initialise other tags
        }
        // calculate max length 
        msgLen_ += (*tagInfoIter)->tagLength_;
        tagsList_->push_back(tag); // add it to the tags list
        ++tagInfoIter;
    }

    // initializing storage for message

    std::cout << "Max Message Length is " << msgLen_ << std::endl;

    msgBuffer_.reset(new char [msgLen_]);

    // calling load for each tag
    TagsListIter tagsIter = tagsList_->begin();
    TagsListIter tagsEndIter = tagsList_->end();
    while (tagsIter != tagsEndIter)
    {
        (*tagsIter)->load(msgBuffer_);
        ++tagsIter;
    }
    // dump this message somewhere in some format to a logfile
    return true;
}

} //namespace Model
} // namespace Fog
