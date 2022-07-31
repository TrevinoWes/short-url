cmake_minimum_required(VERSION 3.10)

set (CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

#Set project name
project(URL-Server)

add_subdirectory(poco)

include_directories(
   poco/ApacheConnector/include
   poco/CppParser/include
   poco/CppUnit/include
   poco/Crypto/include
   poco/Data/include
   poco/Encodings/include
   poco/Foundation/include
   poco/JSON/include
   poco/MongoDB/include
   poco/Net/include
   poco/NetSSL_OpenSSL/include
   poco/NetSSL_Win/include
   poco/openssl/build/include
   poco/PDF/include
   poco/Redis/include
   poco/SevenZip/include
   poco/Util/include
   poco/XML/include
   poco/Zip/include
)

#add executables
add_executable(${PROJECT_NAME} URL_Server.cpp HomePage.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC Poco::Net Poco::Util)
