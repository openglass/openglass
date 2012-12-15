#ifndef MODEL_INFO
#define MODEL_INFO

struct TestCaseMsgInfo
{
    int testCaseMsgId_;
    std::string msgName_;
    int protocolId_;
    std::string protocolName_;
    std::string protocolIp_;
    int portNum_;
};

struct MsgInfo
{
    int testCaseMsgId_;
};

struct TagInfo
{
    int testCaseMsgTagId_;
    std::string tagName_;
    std::string destDataType_;
    int tagLength_;
    char justifyWith_;
    char justifyLeftRight_;
    int offset_;
    std::string defaultValue_;
    bool required_;
    std::string userValue_;
    char endian;
};

struct ProtocolInfo
{
    int protocolId_;
    std::string protocolName_;
    std::string ip_;
    int portNum_;
};

#endif // MODEL_INFO
