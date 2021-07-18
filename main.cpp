#include "bulk.h"
#include "response_handler.h"
#include <boost/program_options.hpp>

namespace po = boost::program_options;

std::string MakeBulkFileName() {
    return "bulk" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".log";
}

int main(int ac, char** av) {
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("block-size", po::value<size_t>())
    ;
    po::positional_options_description pos_desc;
    pos_desc.add("block-size", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(ac, av).
            options(desc).positional(pos_desc).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }

    if (!vm.count("block-size")) {
        std::cout << "You must specify block size" << std::endl;
        std::terminate();
    }

    CommandHandler command_handler{vm["block-size"].as<size_t>()};
    command_handler.AddResponseHandler(MakeOstreamResponseHandler(std::cout));
    command_handler.AddResponseHandler(MakeFileResponseHandler(MakeBulkFileName()));
    std::string command;
    while (std::cin >> command) {
        command_handler.HandleCommand(command);
    }
    command_handler.Stop();

    return 0;
}
