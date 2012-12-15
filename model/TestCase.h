#ifndef TESTCASE_H
#define TESTCASE_H

#include <vector>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include <opencommon/ModelInfo.h>
#include "common/DataSet.h"
#include "common/TextSerializer.h"
#include "datasource/DataSourceMgr.h"
#include "opencommon/ModelInfo.h"
#include "Protocol.h"
#include "Message.h"

namespace Fog {
namespace Model {

typedef boost::shared_ptr<TestCaseMsgInfo> MsgInfoPtr;
typedef boost::shared_ptr<std::vector<MsgInfoPtr> > TestMsgsListPtr;
typedef std::vector<MsgInfoPtr>::iterator MsgListIter;
typedef boost::shared_ptr<Message> MsgPtr;
typedef boost::shared_ptr<std::vector<MsgPtr> >MsgsListPtr;
typedef boost::shared_ptr<Protocol>ProtocolPtr;
/**
 * @brief class for test case creation and running. 
 */
class TestCase
{
public:
    TestCase();
    ~TestCase();
    bool initialize(DataSet<TextSerializer>& ds);
    bool run(DataSet<TextSerializer>& ds) {return true; }
    bool save() {return true; } 
    bool stop() {return true; }
    bool refresh() {return true;}
private:
    TestMsgsListPtr msgInfoList_;
    std::vector<int> protocolIdList_; // TODO - protocol class which gets sent to each message
    int32_t testId_;
    std::string req_;
    std::string args_;
    std::string stat_;
    std::string statMsg_;
    std::map<int, ProtocolPtr> protocolList_;
    std::vector<MsgPtr> msgs_;
    Fog::Common::CommandRouter* cmdRouter_;
    std::ofstream logMsgFile_;
};

} // Model
} // Fog

#endif // TESTCASE_H
