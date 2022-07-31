#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Util/ServerApplication.h>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "HomePage.h"

using namespace Poco::Net;
using namespace Poco::Util;

std::shared_ptr<URLHash> urlHash = std::make_shared<URLHash>();


class RequestHandlerFactory : public HTTPRequestHandlerFactory
{
public:
  virtual HTTPRequestHandler* createRequestHandler(const HTTPServerRequest &)
  {
    return new HomePage(urlHash);
  }
};

class ServerApp : public ServerApplication
{
protected:
  int main(const std::vector<std::string> &)
  {
    HTTPServer s(new RequestHandlerFactory, ServerSocket(9090), new HTTPServerParams);

    s.start();
    std::cout << std::endl << "Server started" << std::endl;

    waitForTerminationRequest();  // wait for CTRL-C or kill

    std::cout << std::endl << "Shutting down..." << std::endl;
    s.stop();

    return Application::EXIT_OK;
  }
};

int main(int argc, char** argv)
{
  ServerApp app;
  return app.run(argc, argv);
}
