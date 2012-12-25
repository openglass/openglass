#ifndef DATASET_H
#define DATASET_H

#include <string.h>

#include <string>
#include <map>
#include <vector>
#include <utility>
#include <iostream>
#include <typeinfo>
#include <iomanip>
#include <sstream>

#include <boost/any.hpp>


#include "CommonDefs.h"
#include "Logger.h"

//////////////////////////////////////////////////////////////////////////////
//
//	CLASS: DataSet
//	TODO - crazy implementation.. PropertiesPtr actually need to be DataSet
//	itself to ensure that when a subsection is passed to anyone who needs it the
//	interface remains consistent .. hmph!!!
//
//////////////////////////////////////////////////////////////////////////////

/**
 * Provides inter-object communication among fog applications
 * <br>
 */
template <template <class Container> class SerializePolicy >
class DataSet : public SerializePolicy<PropertiesPtr>
{
public:
    
    DataSet(std::string theName):SerializePolicy<PropertiesPtr>(name_, dataSet_), name_(theName)
    {
        dataSet_.reset(new Properties());
    }

    DataSet():SerializePolicy<PropertiesPtr>(name_, dataSet_)
    {
        dataSet_.reset(new Properties());
    }

    template <typename T>
	bool add(std::string theSection, std::string theKey, const T& theVal)
	{
        PropertiesPtr* value=NULL;
        std::pair<Properties::iterator, bool> retPair;
        // find section in map (has to be a propertyMap)
        if (theSection == name_)
        {
            value=&dataSet_;
        }
        else
        {
            // get the correct section
            Properties::iterator ret;
            // get section from propertyMap
            ret=(*dataSet_).find(theSection);
            if (ret == (*dataSet_).end())
            {
                LOG_ERROR("Section not found");
                return false;
            }

            value=boost::any_cast<PropertiesPtr>(&(ret->second));
            if (value == NULL)
            {
                std::ostringstream tmp;
                tmp << "Section value has to be a property map";
                LOG_ERROR(tmp.str());
                return false;
            }
        }

        retPair=(*value)->insert(std::pair<std::string, T>(theKey, theVal));
        if (retPair.second==false)
        {
            std::ostringstream tmp;
            tmp << " duplicate key " << theKey.c_str();
            LOG_ERROR(tmp.str());
            return false;
        }
        return true;
	}
	
	template<typename T>
	T* get(std::string theSection, std::string theKey)
    {
        Properties::iterator ret;
        PropertiesPtr* value=NULL;
        if (theSection == name_)
        {
            value=&dataSet_;
        }
        else
        {
            ret=(*dataSet_).find(theSection);
            if (ret == (*dataSet_).end())
            {
                std::ostringstream tmp;
                tmp << "Section " << theSection.c_str () << 
                    " not found" ; 
                LOG_ERROR(tmp.str());
                return NULL;
            }
            value=boost::any_cast<PropertiesPtr>(&(ret->second));
            if (value == NULL)
            {
                std::ostringstream tmp;
                tmp << "Section value has to be a property map";
                LOG_ERROR(tmp.str());
                return NULL;
            }
        }

        ret=(*value)->find(theKey);
        if(ret == (*dataSet_).end())
        {
            std::ostringstream tmp;
            tmp << "value for " << theKey.c_str () << 
                " not found" ; 
            LOG_ERROR(tmp.str());
            return NULL;
        }
        // check if value type is as expected by the caller
        T* valuePtr=boost::any_cast<T>(&(ret->second));
        if (valuePtr == NULL)
        {
            LOG_ERROR("Value not as expected");
        }
        return valuePtr;
    }

//renjini - Right now does not clear the first section - "request"	
	void clear()
	{
		dataSet_->clear();
		dataSet_.reset(new Properties());
                name_ = "REQUEST";
	}
private:
	PropertiesPtr dataSet_;
    std::string name_;
};

#endif // DATASET_H
