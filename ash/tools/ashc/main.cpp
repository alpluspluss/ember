#include <cstdint>
#include <filesystem>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <string>
#include <unistd.h>

constexpr const char* ASH_VERSION = "0.1.0";
namespace fs = std::filesystem;

void print_usage(const std::string& program)
{
    std::cout << "Ash Language Compiler v" << ASH_VERSION << "\n\n";
    std::cout << "Usage: " << program << " [option] <input>\n";
    std::cout << "Options:\n";
    std::cout << "  -o, --output FILE     Specify output file\n";
    std::cout << "  -O0, -O1, -O2, -O3    Set optimization level\n";
    std::cout << "  -v, --verbose         Enable verbose output\n";
    std::cout << "  -d, --debug           Include debug information\n";
    std::cout << "  -h, --help            Display this help message\n";
    std::cout << "  --version             Display compiler version\n";
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        print_usage(argv[0]); /* argv[0] is the program name (path included) */
        return 1;
    }

    std::string output_file;
    std::uint8_t opt_level = 1;
    bool verbose = false;
    bool debug = false;

    static struct option long_options[] = {
        { "output", required_argument, nullptr, 'o' },
        { "verbose", no_argument, nullptr, 'v' },
        { "debug", no_argument, nullptr, 'd' },
        { "help", no_argument, nullptr, 'h' },
        { "version", no_argument, nullptr, 'V' },
        { nullptr, 0, nullptr, 0 }
    };

    int opt_index = 0;
    int c;

    while ((c = getopt_long(argc, argv, "o:O:vdh", long_options, &opt_index)) != -1) 
    {
        switch (c) 
        {
            case 'o':
                output_file = optarg;
                break;
            case 'O':
                opt_level = std::stoi(&optarg[0]);
                if (opt_level < 0 || opt_level > 3) 
                {
                    std::cerr << "Invalid optimization level: " << static_cast<int>(opt_level) << "\n";
                    return 1;
                }
                break;
            case 'v':
                verbose = true;
                break;
            case 'd':
                debug = true;
                break;
            case 'h':
                print_usage(argv[0]);
                return 0;
            case 'V':
                std::cout << "Ash Language Compiler v" << ASH_VERSION << "\n";
                return 0;
            case '?': /* getopt_long already printed an error message */
                return 1;
            default:
                std::abort();
        }
    }

    if (optind >= argc)
    {
        std::cerr << "error: no input file specified" << "\n";
        return 1;
    }

    std::string input_file = argv[optind]; /* input file */
    if (output_file.empty()) /* generate default; remove ext & add .o */
    {
        fs::path input_path(input_file);
        fs::path output_path = input_path.stem();
        output_path += ".o";
        output_file = output_path.string();
    }

    /* if we can write to the output file */
    try 
    {
        std::ofstream test_file(output_file, std::ios::out);
        if (!test_file) 
        {
            std::cerr << "error: cannot write to output file: " << output_file << "\n";
            return 1;
        }
        test_file.close();
        fs::remove(output_file); /* remove the test file */
    } 
    catch (const std::exception& e) 
    {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }

    if (verbose)
    {
        std::cout << "Input file: " << input_file << "\n";
        std::cout << "Output file: " << output_file << "\n";
        std::cout << "Optimization level: O" << static_cast<int>(opt_level) << "\n";
        std::cout << "Debug info: " << (debug ? "enabled" : "disabled") << "\n";
    }

    /* TODO: call into the actual compiler */
    std::cout << "Compilation not yet implemented" << "\n";

    return 0;
}