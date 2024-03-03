// DraftWebsiteServer.cpp : Defines the entry point for the application.
//


#include "DraftWebsiteServer.h"
#include "httplib.h"

using namespace httplib;


int main()
{
    httplib::Server svr;


    bool mountPointed = svr.set_mount_point("", "D:\\DraftWebsite\\DraftWebsite\\DraftWebsiteHTML");
    

    std::cout << "are mount pointed " << mountPointed  << std::endl;
    svr.Get("/", [](const httplib::Request& req, Response& res) {
        std::cout << "hej getta html" << std::endl;
        
        std::ifstream ifStream = std::ifstream("D:\\DraftWebsite\\DraftWebsite\\DraftWebsiteHTML\\placeholder.html");
        
        ifStream.seekg(0, ifStream.end);
        std::string html = std::string(ifStream.tellg(),0);
        ifStream.seekg(0, ifStream.beg);
        ifStream.read(html.data(), html.size());
        res.set_content(html , "text/html");
        std::cout << html.size() << std::endl;
        });

    svr.Get("/hi", [](const httplib::Request& req, Response& res) {
        std::cout << "hej " << std::endl;
        res.status = StatusCode::Accepted_202;
        res.set_content("har var det data","text/plain");
        });

    // Match the request path against a regular expression
    // and extract its captures
    svr.Get(R"(/numbers/(\d+))", [&](const Request& req, Response& res) {
        auto numbers = req.matches[1];
        res.set_content(numbers, "text/plain");
        
        });

    // Capture the second segment of the request path as "id" path param
    svr.Get("/users/:id", [&](const Request& req, Response& res) {
        auto user_id = req.path_params.at("id");
        res.set_content(user_id, "text/plain");
        });



    svr.Post("/body-header-param", [](const Request& req, Response& res) {

            std::cout << "hej" << std::endl;
            if (req.body._Equal("/hi"))
            {
                res.set_content("hej fran servern","text/plain");
            }
            else
            {
                res.set_content("knas med parsandet", "text/plain");
            }

        });
    
    // Extract values from HTTP headers and URL query params
    svr.Get("/body-header-param", [](const Request& req, Response& res) {
        
        std::cout << "kommer hit" << std::endl;
        if (req.has_header("Content-Length")) {
            auto val = req.get_header_value("Content-Length");
        }
        if (req.has_param("key")) {
            auto val = req.get_param_value("key");
        }
        res.set_content(req.body, "text/plain");
        if (req.body._Equal("hej"))
        {
            res.set_content("laste korrekt", "text/plain");
        }
        });

    svr.Get("/stop", [&](const Request& req, Response& res) {
        svr.stop();
        });

    std::cout << "hej varlden" << std::endl;

    svr.listen("localhost", 1234);
	return 0;
}
