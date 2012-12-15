#ifndef MODEL_TAG_H
#define MODEL_TAG_H
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

#include <common/Logger.h>
#include <opencommon/ModelInfo.h>

namespace Fog {
namespace Model {

typedef boost::shared_ptr<TagInfo> TagInfoPtr;
typedef boost::shared_array<char> MsgBufferPtr;

/**
 * @brief class for test case creation and running. 
 */
class Tag
{
public:
    Tag(TagInfoPtr tagInfo);
    void load (MsgBufferPtr msgBuffer);
    bool initialize();
private:
    bool initializeDest(std::string& value);
    TagInfoPtr tagInfo_;
    boost::shared_array<char> tagValue_;
    bool bigEndian_;
};

} // Model
} // Fog

#endif // MODEL_TAG_H
