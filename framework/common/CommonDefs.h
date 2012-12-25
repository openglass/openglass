#ifndef COMMON_DEFS_H
#define COMMON_DEFS_H

#include <map>
#include <vector>
#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>

typedef std::map<std::string, boost::any> Properties;
typedef boost::shared_ptr<Properties> PropertiesPtr;
typedef std::vector<int> Ints;
typedef boost::shared_ptr<Ints> IntsPtr;
typedef std::vector<std::string > Strings;
typedef boost::shared_ptr<Strings> StringsPtr;

#endif // COMMOM_DEFS_H

