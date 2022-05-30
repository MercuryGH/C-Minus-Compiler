#include <iostream>
#include <boost/program_options.hpp>
#include "backend/AssemblyFileIO.h"
#include "backend/VM.h"
#include "Runtime.h"

const string Runtime::WELCOME_PROMPT = "VM for C-Minus Programming Language. \nOptions";

bool Runtime::readArgs(int argc, char **argv) {
    namespace bpo = boost::program_options;

    bpo::options_description desc(WELCOME_PROMPT);
    desc.add_options()
        ("help,h", "Show help message.")
        ("run,r", bpo::value<string>(&asmFilePath), "Run assembly file with VM from <arg> path.");

    bpo::variables_map var_map;
    try {
        bpo::store(bpo::parse_command_line(argc, argv, desc), var_map);

        if (var_map.find("help") != var_map.end()) {
            std::cout << desc << "\n";
            return false;
        }

        bpo::notify(var_map);
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return false;
    } catch (...) {
        std::cerr << "Unknown error during readArgs! \n";
        return false;
    }

    if (asmFilePath.empty()) {
        std::cout << desc << "\n";
        return false;
    }

    return true;
}

void Runtime::execCode() const {
    if (asmFilePath.empty() == false) {
        auto codes = AssemblyFileIO::readAsmFile(asmFilePath);

        VM vm(codes);
        vm.run();
    } else {
        std::cerr << "Fatal error: no input files.\n";
    }
}