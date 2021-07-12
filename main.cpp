#include "bulk.h"
#include <boost/program_options.hpp>

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

    std::unique_ptr<ResponseHandler> response_handler = GetMainResponseHandler();
    CommandHandler command_handler{vm["block-size"].as<size_t>()};
    std::string command;
    while (std::cin >> command) {
        response_handler->HandleResponse(command_handler.HandleCommand(command));
    }
    response_handler->HandleResponse(command_handler.Stop());

    return 0;
}
