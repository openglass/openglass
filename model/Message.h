#ifndef MODEL_MESSAGE_H
#define MODEL_MESSAGE_H

#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include "opencommon/ModelInfo.h"
#include "common/Command.h"
#include "common/DataSet.h"
#include "common/TextSerializer.h"
#include <common/CommonDefs.h>
#include <common/Logger.h>
#include "Protocol.h"
#include "Tag.h"

namespace Fog{
namespace Model{

typedef boost::shared_ptr<TagInfo> TagInfoPtr;
typedef boost::shared_ptr < std::vector< TagInfoPtr > > MessageTagInfoListPtr;
typedef std::vector< TagInfoPtr >::iterator TagInfoIter;
typedef boost::shared_ptr<Protocol> ProtocolPtr;
typedef boost::shared_ptr<Tag> TagPtr;
typedef boost::shared_ptr < std::vector<TagPtr> > TagsList;
typedef std::vector<TagPtr>::iterator TagsListIter;

class Message
{

private:
    boost::shared_array<char> msgBuffer_;
    int testMsgId_;
    std::string msgName_;
    //container for the tags
    MessageTagInfoListPtr msgTagInfoListPtr_;
    int testId_;
    ProtocolPtr protocol_;
    Fog::Common::CommandRouter* cmdRouter_;
    TagsList tagsList_;
    uint32_t msgLen_;
public:
    Message( int testCaseMsgId, 
             std::string& messageName, 
             int testCaseId, 
             ProtocolPtr p);
    ~Message();
    //Get the tagInfo for a particular testcase message and store in the vector
    bool initialize();

    //To send the message if required
    void send();

    //Dump the message to a file in readable form
    void dump(char*);
    boost::shared_array<char> getMsgBuffer() { return msgBuffer_; }
    int32_t getMsgLen() { return msgLen_; }


};

} //namespace Model
} //namespace Fog


#endif // MODEL_MESSAGE_H
