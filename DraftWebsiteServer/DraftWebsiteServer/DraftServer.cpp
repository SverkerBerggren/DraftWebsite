#include "DraftServer.h"

using namespace httplib;
void DraftServer::Start()
{
    svr.set_mount_point("", "D:\\DraftWebsite\\DraftWebsite\\DraftWebsiteHTML");


    svr.Get("/", [](const httplib::Request& req, Response& res) {
        std::cout << "hej getta html" << std::endl;
        std::cout << " Cookie " << req.get_header_value("Cookie") << std::endl;

        std::ifstream ifStream = std::ifstream("D:\\DraftWebsite\\DraftWebsite\\DraftWebsiteHTML\\placeholder.html");

        ifStream.seekg(0, ifStream.end);
        std::string html = std::string(ifStream.tellg(), 0);
        ifStream.seekg(0, ifStream.beg);
        ifStream.read(html.data(), html.size());
        res.set_content(html, "text/html");
        res.set_header("Set-Cookie", "PlayerId=forstaSpelaren; Expires=Thu, 21 Apr 2024 07:28:00 GMT;");

        std::cout << html.size() << std::endl;
        });

    svr.Get("/DraftableCards", [](const httplib::Request& req, Response& res) {

        std::string playerId = req.get_header_value("Cookie");

        std::ifstream ifStream = std::ifstream("D:\\DraftWebsite\\DraftWebsite\\DraftWebsiteHTML\\placeholder.html");


        });


    svr.Get("/hi", [](const httplib::Request& req, Response& res) {
        std::cout << "hej " << std::endl;
        res.status = StatusCode::Accepted_202;
        res.set_content("har var det data", "text/plain");
        });

    svr.listen("localhost", 1234);
}

