#ifndef HomePage_INCLUDED
#define HomePage_INCLUDED

#include <map>
#include <string>
#include <memory>
#include <utility>
#include <regex>
#include <mutex>
#include "Poco/Net/HTTPRequestHandler.h"

//Hash of stored urls scheme:
// key: shortened uri
// value->first: count of usage
// value->second: original url
typedef std::map<const std::string, std::pair<int, std::string>> URLHash;

class HomePage: public Poco::Net::HTTPRequestHandler
{
public:
	HomePage(std::shared_ptr<URLHash> hash);
	void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
private:
	static std::mutex hashMutex;
	std::map<int, char> conversionTable;
	std::shared_ptr<URLHash> urlHash;
	const std::regex URLPATTERN{"((http|https)://)(www.)?[a-zA-Z0-9@:%._\\+~#?&//=]{2,256}\\.[a-z]{2,6}\\b([-a-zA-Z0-9@:%._\\+~#?&//=]*)"};
	
	enum Error
	{
		MISSINGINPUT0,
		MISSINGINPUT1,
		INVALIDURL,
		INVALIDSHORTURL,
		SIZE = INVALIDSHORTURL
	};

	enum Operation
	{
	       	INSERT = 0,
	  	RETRIEVE = 1	
        };
	std::string encodeURL(std::string url);
};


#endif // HomePage_INCLUDED
