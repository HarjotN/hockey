// HockeyStats.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <string>

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams

int main(int argc, char* argv[])
{
    auto fileStream = std::make_shared<ostream>();

    // Open stream to output file.
    pplx::task<void> requestTask = fstream::open_ostream(U("results.html")).then([=](ostream outFile)
        {
            *fileStream = outFile;

            // Create http_client to send the request.
            http_client client(U("https://statsapi.web.nhl.com/api/v1/people/8475166"));

            // Build request URI and start the request.
            //uri_builder builder(U("/stats/"));
            //builder.append_query(U("stats"), U("careerRegularSeason"));
            //return client.request(methods::GET, builder.to_string());

            return client.request(methods::GET);
        })

    // Handle response headers arriving.
        .then([=](http_response response)
        {
            printf("Received response status code:%u\n", response.status_code());

            // Write response body into the file.
            if (response.status_code() == status_codes::OK) {
                std::cout << "status_ok";
                    return response.extract_json();
            }
            //Error cases. For now just return empty json

            std::cout << "\n get json data";
            return pplx::task_from_result(json::value());
        })
        .then([=](pplx::task<json::value> previousTask)
            {
                try
                {
                    std::cout << "\nparse json data here!";
                    const json::value& v = previousTask.get();

                    utility::string_t jsonval = v.serialize();

                    std::wcout << jsonval;

                    auto array = v.at(U("row")).as_array();
                    for (int i = 0; i < array.size(); i++) {
                        auto id = array[i].at(U("id")).as_string();
                        std::wcout << "\n" << id;
                        auto key = array[i].at(U("key")).as_string();
                        std::wcout << "\n" << key;
                        auto array2 = array[i].at(U("value")).as_array();

                        std::wcout << array2[0];
                        std::wcout << array2[1];
                    }
                }
                catch (const http_exception& e) {
                    std::cout << "\nError";
                }

                fileStream->close();
            });

            // Wait for all the outstanding I/O to complete and handle any exceptions
            try
            {
                requestTask.wait();
            }
            catch (const std::exception& e)
            {
                printf("Error exception:%s\n", e.what());
            }

            return 0;
}
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
