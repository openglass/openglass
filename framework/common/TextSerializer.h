#ifndef TEXT_SERIALIZER_H
#define TEXT_SERIALIZER_H 

#include <string.h>

#include <string>
#include <map>
#include <vector>
#include <utility>
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <iomanip>
#include <sstream>

#include <boost/any.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

#include "CommonDefs.h"

// policy class that has serialize and deserialize as its interface
// use ws for removing whitespaces

template <typename T>
class TextSerializer
{
public:
    std::string& serialize( std::string& theKey, T& container, int identOffset=0);
    bool deserialize( const char* text);
private:
    T& containter_;
};

template<>
class TextSerializer<PropertiesPtr>
{
public:
    TextSerializer(std::string& theName, PropertiesPtr& theContainer):
    name_(theName),
    container_(theContainer) { }

    std::string& serialize()
    {
        // TODO - need to change the interface
        serializedStr_= serialize_r( name_, container_, 0);
        return serializedStr_;
    }
    std::string& getName() { return name_; }
    void setName(const char* theName) { name_=theName; }

    bool deserialize(const char* theInputStr)
    {
        if (! theInputStr)
        {
            LOG_ERROR("Input String is Null");
            return false;
        }

        char *inputStr=(char*)theInputStr;
        bool breakLoop=false;
        std::string key;
        while (skipCommentsAndSpace(&inputStr))
        {
            switch (inputStr[0])
            {
                case '{' :
                    {
                        // parserMap
                        if (key.empty())
                        {
                            LOG_ERROR("Cannot deserialize without parent key");
                            return false;
                        }
                        PropertiesPtr p(new Properties());
                        if (!parseMap(p, &inputStr))
                        {
                            std::ostringstream tmp;
                            tmp << "Unable to parse map for key - " << key.c_str() << std::endl;
                            LOG_ERROR(tmp.str());
                            return false;
                        }

                        container_=p;
                        name_=key;
                        breakLoop=true; // expect parent section to include all
                        break;
                    }
                case '}' :
                    {
                        LOG_ERROR ("Not sure what to do with } here. ");
                        return false;
                    }
                case '=' :
                    {
                        LOG_ERROR ("Not sure what to do with = here. ");
                        return false;
                    }
                case '(' :
                    {
                        LOG_ERROR ("What is start of list ( doing here");
                        return false;
                    }
                case ')' :
                    {
                        LOG_ERROR ("What is end of list ) doing here");
                        return false;
                    }
                default : 
                    {
                        TYPE_ valueType=UNKNOWN;
                        if (! getWord(&inputStr, key, valueType))
                            return false;
                        break;
                    }
            }
            if (breakLoop)
            {
                breakLoop=false;
                break;
            }
        }
        return true;
    }
    enum TYPE_{
        INT=0,
        FLOAT,
        STRING,
        UNKNOWN
    };
private:
    template <typename U>
    bool is_type (const boost::any& a)
    {
        return typeid(U) == a.type();
    }

    void addDepth(char* identStr, int depthLevel)
    {
        int identSize=depthLevel; 
        memset(identStr, '\t', identSize);
    }

    std::string serialize_r( std::string& theKey, PropertiesPtr& container_, 
            int depth)
    {
        // Maps would start with '{'
        // The list value can be specified in "(" 

        char ident[64];
        memset(ident, 0, sizeof(ident));
        addDepth(ident, depth);

        std::ostringstream oss;
        oss << ident << theKey.c_str() << "\n";
        oss << ident << "{" << "\n";

        Properties::iterator iter=container_->begin();

        for(; iter != container_->end(); ++iter)
        {
            // cast it to find the type and based on the known types we can then
            // serialize
            if (is_type<int>((*iter).second) )
            {
                int* value=boost::any_cast<int>(&((*iter).second));
                oss << ident << (*iter).first.c_str() << "=" << *value << "\n";
                continue;
            }

            if (is_type<float>((*iter).second))
            {
                float* value=boost::any_cast<float>(&((*iter).second));
                oss << ident <<  (*iter).first.c_str()  << "=" << std::setprecision(7) << *value << "\n";
                continue;
            }

            if (is_type<std::string>((*iter).second))
            {
                std::string* value=boost::any_cast<std::string>(&((*iter).second));
                oss << ident << (*iter).first.c_str()  << "=" << "\"" << value->c_str() << "\""<< "\n";
                continue;
            }

            if (is_type<StringsPtr>((*iter).second))
            {
                StringsPtr* value=boost::any_cast<StringsPtr>(&((*iter).second));
                oss << ident << (*iter).first.c_str()  << "=";
                oss << "( " << " ";
                Strings::iterator listIter=(*value)->begin();
                Strings::iterator endIter=(*value)->end();

                for(; listIter != endIter; ++listIter)
                {
                    // TODO - need to add check for ehitespace within strings
                    // and how to get around it
//                    oss << " " << (*listIter).c_str() ;
                    oss << " " << "\"" << (*listIter).c_str() << "\"";
                }
                oss << " )" << "\n";
                continue;
            }

            if (is_type<IntsPtr>((*iter).second))
            {
                IntsPtr* value=boost::any_cast<IntsPtr>(&((*iter).second));
                oss << ident << (*iter).first.c_str()  << "=";
                oss << "( " << " ";
                Ints::iterator listIter=(*value)->begin();
                Ints::iterator endIter=(*value)->end();

                for(; listIter != endIter; ++listIter)
                {
                    oss << " " << *listIter;
                }
                oss << " )" << "\n";
                continue;
            }

            if (is_type<PropertiesPtr>((*iter).second))
            {
                PropertiesPtr *value=boost::any_cast<PropertiesPtr>(&((*iter).second));
                std::string property;
                std::string theKey=(*iter).first;
                property=serialize_r( theKey, (*value), ++depth );
                oss << ident << property.c_str() << "\n";
                continue;
            }

            std::ostringstream tmp;
             tmp << "unable to handle value type - " << 
                         typeid((*iter).second).name() << "for key" << (*iter).first 
                         << std::endl;
            LOG_ERROR(tmp.str());

            // TODO - throw exception here 
            break;
        }
        oss << ident << "}" << "\n";
        return oss.str();
    }

    template <typename T>
    bool addProperty (Properties& prop, std::string& theKey, T& theVal)
    {
        std::pair<Properties::iterator, bool> ret;
        ret=prop.insert(std::pair<std::string, T>(theKey, theVal));
        if (ret.second==false)
        {
            std::cout << " duplicate key " << theKey.c_str() << std::endl;
            return false;
        }
        return true;
    }

    TYPE_ getType(char* word)
    {
        int wordLen = strlen(word);

        if ( !wordLen)
            return UNKNOWN;

        int i=0;

        // LEADING + OR -
        if ( wordLen > 1 && (word[0] == '-' || word[0] == '+') )
            i++;

        int noDots=0;
        // TEST FOR ALL DIGITS
        for (; i < wordLen; i++)
        {
            if ( !isdigit(word[i]) )
            {
                if (word[i] == '.' )
                {
                    noDots++;
                    if (noDots > 1)
                        return STRING;
                    continue;
                }
                return STRING;
            }
        }
        if (noDots ==1 )
            return FLOAT;
        else 
            return INT;
    }

    bool skipWhiteSpace(char** inputStr)
    {
        while((*inputStr)[0] != '\0' && isspace((*inputStr)[0]))
            (*inputStr)++;
        if ((*inputStr)[0] == '\0')
        {
            LOG_DEBUG("reached end of string");
            return false;
        }
        return true;
    }

    bool skipCommentsAndSpace(char** inputStr)
    {
        if (! skipWhiteSpace(inputStr))
            return false;
        while((*inputStr)[0] == '#')
        {
            // get to end of line 
            char* el= strchr(*inputStr, '\n');
            if(!el)
                std::cout << " last line comments not appreciated" << std::endl;
            ptrdiff_t tillEndLine= el - (*inputStr);
            *inputStr=*inputStr+tillEndLine;
            if (! skipWhiteSpace(inputStr))
                return false;
        }
        return true;
    }

    /**
     * @brief extracts a word from a char buffer and also provides the type 
     *
     * @param inputStr 
     * @param word
     * @param valueType - Only for explicity quoted strings to advertise
     * themselvees as strings as there is no other way to do this.
     *
     * @return  bool - whether sucessfully extracted or not
     */
    bool getWord(char** inputStr, std::string& word, TYPE_& valueType)
    {
        // todo add for " and for 
        if (! skipCommentsAndSpace(inputStr))
            return false;

        char* begin=(*inputStr);
        bool isQuoted=false;
        if (*begin == '"')
        {
            (*inputStr)++;
            begin=strchr(*inputStr, '"');
            if (!begin)
            {
                std::cout << "no enclosing quotes" << std::endl;
                return false;
            }
            isQuoted=true;
        }
        else
        {
            while ( true )
            { 
                bool breakLoop=false;
                switch(*begin)
                {
                    case '=' :
                    case '{' :
                    case '}' :
                    case '(' :
                    case ')' :
                        breakLoop=true;
                        break;
                }

                if (isspace(*begin) || breakLoop )
                    break;
                begin++;
            }
        }

        ptrdiff_t size=begin - (*inputStr);
        if (size==0)
            return false;

        char tmpWord[size +1];
        strncpy(tmpWord, *inputStr, size);
        tmpWord[size]='\0';
        word=tmpWord;
        if (isQuoted)
        {
           *inputStr += (size+1);
           valueType=STRING;
        }
        else
        {
            *inputStr += size;
            valueType=UNKNOWN;
        }
           
        return true;
    }

    bool parseList(PropertiesPtr propPtr, std::string& key, char** inputStr)
    {
        if ((*inputStr)[0] != '(')
        {
            LOG_ERROR("did not receive map opening");
            return false;
        }

        (*inputStr)++;
        if (!skipCommentsAndSpace(inputStr))
            return false;
        std::string value;
        bool firstWord=true;
        bool isInts=false;
        IntsPtr ip(new Ints());
        StringsPtr sp(new Strings());
        TYPE_ valueType=UNKNOWN;
        while (getWord(inputStr, value, valueType))
        {
            if (firstWord)
            {
                // determine type and create vectors accordingly
                TYPE_ valType;
                if (valueType != UNKNOWN)
                    valType=valueType;
                else
                    valType=getType((char*)value.c_str());

                switch(valType)
                {
                    // only supportin list of ints , no floats yet!!!
                    case INT:
                        isInts=true;
                        break;
                    case STRING:
                        isInts=false;
                        break;
                    default:
                        std::cout << "unknown type of list - not supported" << std::endl;
                        return false;
                }
                firstWord=false;
            }
            if(!isInts)
            {
                sp->push_back(value);
            }
            else
            {
                try 
                {
                    int intVal=boost::lexical_cast<int> (value.c_str());
                    ip->push_back(intVal);
                }
                catch(boost::bad_lexical_cast& e)
                {
                    std::cout << "unable to convert - not supported" << std::endl;
                    return false;
                }
            }
        }
        if ((*inputStr)[0] == ')')
        {
            if (isInts)
            {
                std::pair<Properties::iterator, bool> ret;
                ret=propPtr->insert(std::pair<std::string, IntsPtr>(key, ip));
                if (ret.second==false)
                {
                    std::ostringstream tmp;
                    tmp <<  "Insertion of key " << key.c_str() << "in map failed";
                    LOG_ERROR(tmp.str());
                    return false;
                }
//                addProperty<IntsPtr>(*propPtr, key, ip);
            }
            else
            {
//                addProperty<StringsPtr>(*propPtr, key, sp);
                std::pair<Properties::iterator, bool> ret;
                ret=propPtr->insert(std::pair<std::string, StringsPtr>(key, sp));
                if (ret.second==false)
                {
                    std::ostringstream tmp;
                    tmp <<  "Insertion of key " << key.c_str() << "in map failed";
                    LOG_ERROR(tmp.str());
                    return false;
                }
            }
            (*inputStr)++;
            return true;
        }
        return false;
    }

    bool parseMap(PropertiesPtr propPtr, char** inputStr)
    {

        if ((*inputStr)[0] != '{')
        {
            LOG_ERROR("did not receive map opening");
            return false;
        }

        (*inputStr)++;

        if (!skipCommentsAndSpace(inputStr))
        {
            LOG_ERROR("got end of string when not expected");
            return false;
        }


        std::string key;
        // understand value
        while (skipCommentsAndSpace(inputStr))
        {
            switch((*inputStr)[0])
            {
                case '}':
                    {
                        // end of map, break and return
                        (*inputStr)++;
                        if (propPtr->empty())
                        {
                            LOG_ERROR("Property map is empty");
                            return false;
                        }
                        return true;
                    }
                case '{':
                    {
                        // start of another Property Map within
                        PropertiesPtr newPtr(new Properties());

                        if(!parseMap(newPtr, inputStr))
                        {
                            LOG_ERROR("Unable to parser embedded map ");
                            return false;
                        }

                        addProperty<PropertiesPtr>(*propPtr, key, newPtr);
                        break;
                    }
                case '=':
                    // go and extract the value
                    {
                        if (key.empty())
                        {
                            LOG_ERROR("Cannot create value for an empty key");
                            return false;
                        }

                        (*inputStr)++;
                        std::string value;
                        TYPE_ valueType=UNKNOWN;
                        if (! getWord(inputStr, value, valueType))
                            break; // value could be a list or map or whatever
                        try 
                        {
                            TYPE_ valType;
                            if (valueType != UNKNOWN)
                                valType=valueType;
                            else
                                valType=getType((char*)value.c_str());
                            switch(valType)
                            {
                                case FLOAT:
                                    {
                                        float flValue = boost::lexical_cast<float> (value.c_str());
                                        addProperty<float>(*propPtr, key, flValue);
                                        break;
                                    }
                                case INT:
                                    {
                                        int intValue = boost::lexical_cast<int> (value.c_str());
                                        addProperty<int>(*propPtr, key, intValue );
                                        break;
                                    }
                                case STRING:
                                {
         //                           addProperty<std::string>(*propPtr, key, value);
                                    std::pair<Properties::iterator, bool> ret;
                                    ret=propPtr->insert(std::pair<std::string, std::string>(key, value));
                                    if (ret.second==false)
                                    {
                                        std::ostringstream tmp;
                                        tmp << " duplicate key " << key.c_str() << std::endl;
                                        LOG_ERROR(tmp.str());
                                        return false;
                                    }

                                    break;
                                }
                                case UNKNOWN:
                                    break;
                                default:
                                    break;
                            }
                            //reset the key
                            key="";
                        }
                        catch(boost::bad_lexical_cast& e)
                        {
                            std::cout << "Unable to cast - " << e.what() << std::endl;
                            break;
                        }
                    }
                    break;
                case '(':
                    {
                        if (key.empty())
                        {
                            LOG_ERROR("Cannot create value for an empty key");
                            return false;
                        }

                        if (! parseList(propPtr, key, inputStr))
                        {
                            LOG_ERROR("Cannot create list for an empty key");
                            return false;
                        }
                    }
                    break;
                case ')':
                    {
                        LOG_ERROR("Cannot parse end of list token here ");
                        return false;
                    }
                default :
                    // only get and set the key here
                    //
                    TYPE_ valueType=UNKNOWN;
                    getWord(inputStr, key, valueType);
                    break;
            }
        }
        return true;
    }


    std::string& name_;
    PropertiesPtr& container_;
    std::string serializedStr_;
};



#endif // TEST_SERIALIZER_H
