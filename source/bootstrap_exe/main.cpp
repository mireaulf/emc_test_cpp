#include <cpprest/http_client.h>
#include <cpprest/streams.h>
using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams

#include <string>
using std::string;
#include <xstring>
using std::wstring;
#include <iostream>
using std::cout;
using std::cin;
using std::endl;
using std::cerr;

#include "../../include/EMC/lookup_buffer.h"
using emc::lookup_buffer;

static const int ARGUMENT_COUNT = 3;

struct conversion_error : public std::exception {};

// one of the only time I allow myself to use a #define
#define UNREFERENCED(param) (param)

wstring get_wide_url(const string& url) {
    auto url_wide = std::shared_ptr<wchar_t>(new wchar_t[url.length()]);
    auto str_ptr = url.c_str();
    if(-1 == std::mbstowcs(url_wide.get(), url.c_str(), url.length())) {
        throw conversion_error();
    } 
    return wstring(url_wide.get(), url_wide.get() + url.length());
}

int main(int argc, const char* argv[]) {
    int retval = -1;
    cout << "_____________________________________" << endl
        << "        __          ___    __________" << endl
        << "\\    ___) |        |  /  __)    _    " << endl
        << " |  (__   |  |\\/|  | |  /     _( )_  " << endl
        << " |   __)  |  |  |  | | |     (__ __) " << endl
        << " |  (___  |  |  |  | |  \\__    (_)   " << endl
        << "/       )_|  |__|  |__\\    )_________" << endl
        << "_____________________________________" << endl;
    if(argc < ARGUMENT_COUNT) {
        cout << "Missing arguments:" << endl
            << "Usage: '" << argv[0] << " search_term url'" << endl;
    } else {
        retval = 0;
        string url(argv[1]);
        string search_term(argv[2]);
        try {
            wstring request_to = get_wide_url(url);
            cout << "Looking up for the term '" << search_term << "' @ " << url << endl;
            lookup_buffer<1024, Concurrency::streams::streambuf<uint8_t>> lookup(search_term);
            http_client client(request_to);
            client.request(methods::GET)
            .then([&lookup](http_response resp) {
                cerr << "[LOG] Received response code: " << resp.status_code() << endl;
                if(200 == resp.status_code()) {
                    auto stream = resp.body().streambuf();
                    lookup.search_in(stream, true);
                }
            }).get();
            if(lookup.is_lookup_found()) {
                cout << "The word following the lookup term is: '" << lookup.get_content() << "'";
            } else {
                cout << "The lookup term could not be found. Sorry!";
            }
            cout << endl;
        } catch(std::exception& e) {
            UNREFERENCED(e);
            retval = -2;
            cerr << "[ERROR] Something really bad happened :/" << endl;
        }
    }
    return retval;
}
