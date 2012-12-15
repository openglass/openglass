#include "Tag.h"

#include <endian.h>
#include <stdint.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>


namespace Fog {
namespace Model {

Tag::Tag(TagInfoPtr tagInfo):tagInfo_(tagInfo), bigEndian_(true)
{
    if (tagInfo_->endian == 'l')
        bigEndian_=false;
}

bool Tag::initialize()
{
    bool result=false;
    if ((tagInfo_->userValue_).empty())
        result=initializeDest(tagInfo_->defaultValue_);
    else   
        result=initializeDest(tagInfo_->userValue_);
    return result;
}

// msgBuffer need to have sufficient storage else memory corruption in progress
// :)
//
void Tag::load (MsgBufferPtr msgBuffer)
{
    // memcpy at offset for this tag
    std::cout << " msg Buffer - " << std::hex << msgBuffer.get() 
	      <<  " tag Offset - " << tagInfo_->offset_ 
	      << " tag Length - " << tagInfo_->tagLength_ << std::endl;
    memcpy( msgBuffer.get() +(tagInfo_->offset_), tagValue_.get(), 
            tagInfo_->tagLength_);
}

bool Tag::initializeDest(std::string& value)
{
        switch (tagInfo_->destDataType_.at(0)) {
            case 'T':
                {
                    // for Byte pair.. no idea what to do with this currently
                    tagValue_.reset(new char[tagInfo_->tagLength_]);
                    // memcpied at the destination offset
                    memset(tagValue_.get(), 0, tagInfo_->tagLength_);
                    return true; 
                }
            case 'h':
                {
                    tagValue_.reset(new char[tagInfo_->tagLength_]);
                    // memcpied at the destination offset
                    int16_t tmp;
                    sscanf(value.c_str(), "%hd", &tmp); // TODO ideally check for scanf return
                    bigEndian_ ?  tmp=htobe16((uint16_t)tmp) : 
                                  tmp=htole16((uint16_t)tmp);
                    memcpy(tagValue_.get(), &tmp, tagInfo_->tagLength_);
                    return true; 
                }
            case 'H':
                {
                    tagValue_.reset(new char[tagInfo_->tagLength_]);
                    // memcpied at the destination offset
                    uint16_t tmp;
                    sscanf(value.c_str(), "%hu", &tmp); // TODO ideally check for scanf return
                    bigEndian_ ?  tmp=htobe16(tmp) : 
                                  tmp=htole16(tmp);
                    memcpy(tagValue_.get(), &tmp, tagInfo_->tagLength_);
                    return true;
                }
            case 'i':
                {
                    tagValue_.reset(new char[tagInfo_->tagLength_]);
                    // memcpied at the destination offset
                    uint32_t tmp;
                    sscanf(value.c_str(), "%d", (int32_t*) &tmp); // TODO ideally check for scanf return
                    bigEndian_ ?  tmp=htobe32(tmp) : 
                                  tmp=htole32(tmp);
                    memcpy(tagValue_.get(), &tmp, tagInfo_->tagLength_);
                    return true; // ideally check for scanf return
                }
            case 'u':
                {
                    tagValue_.reset( new char[tagInfo_->tagLength_]);
                    // memcpied at the destination offset
                    uint32_t tmp;
                    sscanf(value.c_str(), "%u", &tmp); // TODO ideally check for scanf return
                    bigEndian_ ?  tmp=htobe32(tmp) : 
                                  tmp=htole32(tmp);
                    memcpy(tagValue_.get(), &tmp, tagInfo_->tagLength_);
                    return true;
                }
            case 'I':
                {
                    tagValue_.reset( new char[tagInfo_->tagLength_]);
                    // memcpied at the destination offset
                    uint64_t tmp;
                    sscanf(value.c_str(), "%lld", (long long int*)&tmp); // TODO ideally check for scanf return
                    bigEndian_ ?  tmp=htobe64(tmp) : 
                                  tmp=htole64(tmp);
                    memcpy(tagValue_.get(), &tmp, tagInfo_->tagLength_);
                    return true;
                }
            case 'U':
                {
                    tagValue_.reset( new char[tagInfo_->tagLength_]);
                    // memcpied at the destination offset
                    sscanf(value.c_str(), "%llu", (long long unsigned int*)tagValue_.get()); 
                    uint64_t tmp;
                    sscanf(value.c_str(), "%llu", (long long unsigned int*)&tmp); // TODO ideally check for scanf return
                    bigEndian_ ?  tmp=htobe64(tmp) : 
                                  tmp=htole64(tmp);
                    memcpy(tagValue_.get(), &tmp, tagInfo_->tagLength_);
                    return true;
                }
            case 'b':
            case 'B': 
                    {
                        // since dest data type is binary will attempt to change
                        // char values 1 to 9 to its binary form 1 to 9
                        char val = value.at(0);
                        if ( (val >= 48)  && (val <=57) )
                            val -= 48; // thereby making it binary 
                        else
                        {
                            std::ostringstream log;
                            log << "char value specified not really a number" 
                                << "Not sure why data type specified as binary -" 
                                << "for tag - " << tagInfo_->tagName_ 
                                << " Will memcpy blindly and continue" ;
                            LOG_WARNING(log.str());
                        }

                        tagValue_.reset( new char[1]);
                        tagValue_[0]=val;
                        return true; 
                    }
            case 'C':
            case 'c':
                    tagValue_.reset( new char[1]);
                    // memcpied at the destination offset
                    tagValue_[0]=tagInfo_->userValue_.at(0);
                    return true; // ideally check for scanf return
            case 's':
                    {
                    // all asciii formats will come here
                    tagValue_.reset(new char [tagInfo_->tagLength_]);
                    std::ostringstream targetStr;
                    if (tagInfo_->justifyLeftRight_== 'L')
                        targetStr << std::left;
                    else
                        targetStr << std::right;
                    targetStr << std::setw(tagInfo_->tagLength_) 
                              << std::setfill(tagInfo_->justifyWith_) 
                              << value;
                    memcpy(tagValue_.get(), (targetStr.str()).c_str(), 
                           tagInfo_->tagLength_);
                    return true; // ideally check for scanf return
                    }
            case 'f':
                 {
                    tagValue_.reset( new char[tagInfo_->tagLength_]);
                    // memcpied at the destination offset
                    uint32_t tmp;
                    sscanf(value.c_str(), "%f", (float *)&tmp); // TODO ideally check for scanf return
                    bigEndian_ ?  tmp=htobe32(tmp) : 
                                  tmp=htole32(tmp);
                    memcpy(tagValue_.get(), &tmp, tagInfo_->tagLength_);
                    return true;
                 }
            case 'd':
                 {
                    tagValue_.reset( new char[tagInfo_->tagLength_]);
                    // memcpied at the destination offset
                    uint64_t tmp;
                    sscanf(value.c_str(), "%lf", (double *)&tmp); // TODO ideally check for scanf return
                    bigEndian_ ?  tmp=htobe64(tmp) : 
                                  tmp=htole64(tmp);
                    memcpy(tagValue_.get(), &tmp, tagInfo_->tagLength_);
                    return true;
                 }
            default:
                    {
                        std::ostringstream log;
                        log << "Unknown destination datatype - " << 
                            tagInfo_->destDataType_ 
                            << "for tag - " << tagInfo_->tagName_
                            << std::endl;
                        LOG_ERROR(log.str());
                        return false; 
                    }
        }
}


} // Model
} // Fog
