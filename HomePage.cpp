#include <iostream>
#include "HomePage.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTMLForm.h"

using namespace std::string_literals;
using Poco::Net::NameValueCollection;

std::mutex HomePage::hashMutex;

HomePage::HomePage(std::shared_ptr<URLHash> hash) : urlHash(hash) {
	// Build conversion table for encoding
	int count = 0;
	for (char c = 'a'; c <= 'z'; ++c) {
		conversionTable[count++] = c;
	}

	for (char c = 'A'; c <= 'Z'; ++c) {
		conversionTable[count++] = c;
	}

	for (char c = '0'; c <= '9'; ++c) {
		conversionTable[count++] = c;
	}
}

void HomePage::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
	// Determine if we need to redirect to mapped URL
	{
		std::lock_guard<std::mutex> lk(hashMutex);
		auto url = (*urlHash).find(request.getURI());
		if(url != urlHash->end()) {
			++url->second.first;
			response.redirect(url->second.second);
			return;
		}
	}

	
	response.setChunkedTransferEncoding(true);
	response.setContentType("text/html"s);

	Poco::Net::HTMLForm form(request, request.stream());
	std::vector<bool> errorList(Error::SIZE, false);
	// Track whether we can use value calculated from form
	std::vector<bool> useResponse(2, false);
	std::string resp;

	// Process forms
	if (!form.empty())
	{
		auto opt = atoi(form["operation"].c_str());
		if(opt == Operation::INSERT) {
			auto url = form["url"];

			if (url.size() <= 0) {
				errorList[Error::MISSINGINPUT0] = true;
			} else if (!regex_match(url, URLPATTERN)) {
				errorList[Error::INVALIDURL] = true;
			} else {
				auto shortUrl = encodeURL(url);
				{
					std::lock_guard<std::mutex> lk(hashMutex);
					(*urlHash)[shortUrl] = std::make_pair(0, url);
				}
				useResponse[opt] = true;
				auto link = request.getHost() + shortUrl;
				resp = "short URL: <a href=\"" + link  + "\">" + link + "</a>"; 
			}
		} else if (opt == Operation::RETRIEVE) {
			auto shortUrl = form["short_url"];
			if(shortUrl.size() <= 0) {
				errorList[Error::MISSINGINPUT1] = true;
			} else {
			       	if(shortUrl[0] != '/')
					shortUrl = "/" + shortUrl;

				std::lock_guard<std::mutex> lk(hashMutex);
				if(urlHash->find(shortUrl) == urlHash->end()) {
					errorList[Error::INVALIDSHORTURL] = true;
				} else {
					useResponse[opt] = true;
					auto origURL = (*urlHash)[shortUrl].second;
					auto usageCount = std::to_string((*urlHash)[shortUrl].first);
					resp = "original URL: " + origURL + ", usage count: " + usageCount;
				}
			}
		}
	}

	// Dynamically set html for page
	std::ostream& responseStream = response.send();
	responseStream << "\n";
	responseStream << "<html>\n";
	responseStream << "\t<head>\n";
	responseStream << "\t\t<title>Short URL</title>\n";
	responseStream << "\t</head>\n";
	responseStream << "\t<body style=\"margin: 0; padding:0;\">\n";
	responseStream << "\t\t<h1 style=\"display: flex; justify-content: center; margin-top: 10%;\" >Short URL</h1>\n";
	responseStream << "\t\t<div style=\"display: flex;\">\t\n";
	responseStream << "\t\t\t<div style=\"width: 50%; height: 15em; background-color: green;\" >\n";
	responseStream << "\t\t\t\t<h3 style=\"display: flex; justify-content: center;\">Create New Link</h3>\n";
	responseStream << "\t\t\t\t<form action=\"/form\" method=\"POST\" style=\"display: flex; justify-content: center;\">\n";
	responseStream << "\t\t\t\t\t<input type=\"hidden\" name=\"operation\" id=\"operation\" value=\"0\" />\n";
	responseStream << "\t\t\t\t\t<input type=\"text\" name=\"url\" id=\"url\" placeholder=\"http://google.com\" />\n";
	responseStream << "\t\t\t\t\t<input type=\"submit\" />\n";
	responseStream << "\t\t\t\t</form>\n";

	if(errorList[Error::MISSINGINPUT0])
		responseStream << "<p style=\" color: red; display: flex; justify-content: center;\">A URL must be passed</p>\n";
	if(errorList[Error::INVALIDURL])
		responseStream << "<p style=\" color: red; display: flex; justify-content: center;\">Must be a valid URL</p>\n";
	if(useResponse[0])
		responseStream << "<p style=\" color: white; display: flex; justify-content: center;\">" + resp + "</p>\n";

	responseStream << "\t\t\t</div>\n";
	responseStream << "\t\t\t<div style=\"width: 50%; height: 15em; background-color: purple;\" >\n";
	responseStream << "\t\t\t\t<h3 style=\"display: flex; justify-content: center;\">Retrieve Link and Use Count</h3>\n";
	responseStream << "\t\t\t\t<form action=\"/form\" method=\"GET\" style=\"display: flex; justify-content: center;\">\n";
	responseStream << "\t\t\t\t\t<input type=\"hidden\" name=\"operation\" id=\"operation\" value=\"1\" />\n";
        responseStream << "\t\t\t\t\t<input type=\"text\" name=\"short_url\" id=\"short_url\" placeholder=\"Pe\" />\n";
        responseStream << "\t\t\t\t\t<input type=\"submit\" />\n";
        responseStream << "\t\t\t\t</form>\n";

	if(errorList[Error::MISSINGINPUT1])
		responseStream << "<p style=\" color: red; display: flex; justify-content: center;\">Code cannot be empty</p>\n";
	if(errorList[Error::INVALIDSHORTURL])
		responseStream << "<p style=\" color: red; display: flex; justify-content: center;\">A valid code must be passed</p>\n";
	if(useResponse[1]) {
		responseStream << "<p style=\" color: white; display: flex; justify-content: center;\">" + resp + "</p>\n";
	}

	responseStream << "\t\t\t</div>\n";
	responseStream << "\t\t</div>\n";
	responseStream << "\t</body>\n";
	responseStream << "</html>\n";
	responseStream << "\n";
}

// Bijective function to ensure 1-to-1 relationship
std::string HomePage::encodeURL(std::string url) {
	int sum  = 0;
	for(char& ele: url) {
		sum += ele;
	}	

	std::vector<int> encoding;
	while(sum > 0) {
		encoding.push_back(sum % conversionTable.size());
		sum /= conversionTable.size();
	}

	// All URIs are returned with a starting /
	std::string result = "/"; 
	for(auto it = encoding.rbegin(); it != encoding.rend(); ++it) {
		result.push_back(conversionTable[*it]);
	}
	return result;
}

