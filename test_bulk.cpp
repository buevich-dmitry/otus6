#define BOOST_TEST_MODULE test_bulk

#include "bulk.h"
#include <set>
#include <boost/filesystem.hpp>

#include <boost/test/unit_test.hpp>

namespace fs = boost::filesystem;

BOOST_AUTO_TEST_SUITE(test_response_handler)

void TestResponseHandler(ResponseHandler* handler, std::function<std::string()> get_handler_output) {
    std::string expected_output;

    BOOST_CHECK_EQUAL(expected_output, get_handler_output());

    handler->HandleResponse({});
    BOOST_CHECK_EQUAL(expected_output, get_handler_output());

    handler->HandleResponse({"cmd1"});
    expected_output += "bulk: cmd1\n";
    BOOST_CHECK_EQUAL(expected_output, get_handler_output());

    handler->HandleResponse({"cmd2", "cmd3", "cmd4"});
    expected_output += "bulk: cmd2, cmd3, cmd4\n";
    BOOST_CHECK_EQUAL(expected_output, get_handler_output());

    handler->HandleResponse({});
    BOOST_CHECK_EQUAL(expected_output, get_handler_output());

    handler->HandleResponse({"cmd5", "cmd6"});
    expected_output += "bulk: cmd5, cmd6\n";
    BOOST_CHECK_EQUAL(expected_output, get_handler_output());
}

BOOST_AUTO_TEST_CASE(test_OstreamResponseHandler) {
    std::stringstream ss;
    std::unique_ptr<ResponseHandler> handler = std::make_unique<OstreamResponseHandler>(ss);

    TestResponseHandler(handler.get(), [&ss]() { return ss.str(); });
}

BOOST_AUTO_TEST_CASE(test_FileResponseHandler) {
    std::string file_name = "test_file.log";
    if (fs::exists(file_name)) {
        fs::remove(file_name);
    }
    std::unique_ptr<ResponseHandler> handler = std::make_unique<FileResponseHandler>(file_name);

    TestResponseHandler(handler.get(), [&file_name]() {
        BOOST_CHECK(fs::exists(file_name));
        std::ifstream file{file_name};
        BOOST_CHECK(file.good());
        return std::string{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
    });
}

}


BOOST_AUTO_TEST_SUITE(test_response_handler)

void TestCommand(CommandHandler& handler, const std::string& command, const std::vector<std::string>& expected_response) {
    BOOST_CHECK(expected_response == handler.HandleCommand(command));
}

void TestStopCommand(CommandHandler& handler, const std::vector<std::string>& expected_response) {
    BOOST_CHECK(expected_response == handler.Stop());
}

BOOST_AUTO_TEST_CASE(test_CommandHandler_1) {
    CommandHandler handler{3};
    TestCommand(handler, "cmd1", {});
    TestCommand(handler, "cmd2", {});
    TestCommand(handler, "cmd3", {"cmd1", "cmd2", "cmd3"});
    TestCommand(handler, "cmd4", {});
    TestCommand(handler, "cmd5", {});
    TestStopCommand(handler, {"cmd4", "cmd5"});
}

BOOST_AUTO_TEST_CASE(test_CommandHandler_2) {
    CommandHandler handler{3};
    TestCommand(handler, "cmd1", {});
    TestCommand(handler, "cmd2", {});
    TestCommand(handler, "{", {"cmd1", "cmd2"});
    TestCommand(handler, "cmd3", {});
    TestCommand(handler, "cmd4", {});
    TestCommand(handler, "}", {"cmd3", "cmd4"});
    TestCommand(handler, "{", {});
    TestCommand(handler, "cmd5", {});
    TestCommand(handler, "cmd6", {});
    TestCommand(handler, "{", {});
    TestCommand(handler, "cmd7", {});
    TestCommand(handler, "cmd8", {});
    TestCommand(handler, "}", {});
    TestCommand(handler, "cmd9", {});
    TestCommand(handler, "}", {"cmd5", "cmd6", "cmd7", "cmd8", "cmd9"});
    TestStopCommand(handler, {});
}

}