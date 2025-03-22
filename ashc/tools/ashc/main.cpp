#include <cstdint>
#include <filesystem>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <unistd.h>
#include <ash/lex/lexer.hpp>
#include <ash/rep/tokens.hpp>

constexpr const char* ASH_VERSION = "0.1.0";
namespace fs = std::filesystem;

std::string token_type_str(ash::TokenType type) 
{
    size_t index = static_cast<size_t>(type);
    if (index < ash::token_names.size() && !ash::token_names[index].empty())
        return std::string(ash::token_names[index]);

    return "TOKEN(" + std::to_string(index) + ")";
}

void print_usage(const std::string& program)
{
    std::cout << "Ash Language Compiler v" << ASH_VERSION << "\n\n";
    std::cout << "Usage: " << program << " [option] <input>\n";
    std::cout << "Options:\n";
    std::cout << "  -o, --output FILE     Specify output file\n";
    std::cout << "  -O0, -O1, -O2, -O3    Set optimization level\n";
    std::cout << "  -v, --verbose         Enable verbose output\n";
    std::cout << "  -d, --debug           Include debug information\n";
    std::cout << "  -t, --test-lexer      Run lexer tests\n";
    std::cout << "  -h, --help            Display this help message\n";
    std::cout << "  --version             Display compiler version\n";
}

std::string read_file(const std::string& filename) 
{
    std::ifstream file(filename);
    if (!file)
        throw std::runtime_error("Could not open file: " + filename);
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
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
    bool test_lexer = false;

    static struct option long_options[] = {
        { "output", required_argument, nullptr, 'o' },
        { "verbose", no_argument, nullptr, 'v' },
        { "debug", no_argument, nullptr, 'd' },
        { "test-lexer", no_argument, nullptr, 't' },
        { "help", no_argument, nullptr, 'h' },
        { "version", no_argument, nullptr, 'V' },
        { nullptr, 0, nullptr, 0 }
    };

    int opt_index = 0;
    int c;

    while ((c = getopt_long(argc, argv, "o:O:vdth", long_options, &opt_index)) != -1) 
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
            case 't':
                test_lexer = true;
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
    if (output_file.empty() && !test_lexer) /* generate default; remove ext & add .o */
    {
        fs::path input_path(input_file);
        fs::path output_path = input_path.stem();
        output_path += ".o";
        output_file = output_path.string();
    }

    /* if we can write to the output file */
    if (!test_lexer && !output_file.empty()) 
    {
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
    }

    if (verbose && !test_lexer)
    {
        std::cout << "Input file: " << input_file << "\n";
        std::cout << "Output file: " << output_file << "\n";
        std::cout << "Optimization level: O" << static_cast<int>(opt_level) << "\n";
        std::cout << "Debug info: " << (debug ? "enabled" : "disabled") << "\n";
    }

    try 
    {
        std::string source_code = read_file(input_file);
        std::string_view source_view = source_code;
        std::string_view filename_view = input_file;
        
        ash::Lexer lexer(source_view, filename_view);
        ash::TokenList tokens = lexer.tokenize();
        
        if (test_lexer) 
        {
            std::cout << "===== Lexer Test Results for " << input_file << " =====\n";
            std::cout << "Total tokens: " << tokens.size() << "\n\n";
            
            std::cout << std::setw(5) << "Index" << " | " 
                      << std::setw(20) << "Type" << " | " 
                      << std::setw(10) << "Position" << " | "
                      << "Text\n";
            std::cout << std::string(50, '-') << "\n";
            
            for (size_t i = 0; i < tokens.size(); ++i) 
            {
                ash::Token token = tokens[i];
                std::string token_text;
                
                if (token.len > 0)
                {
                    token_text = std::string(source_view.substr(token.start, token.len));
                } 
                else 
                {
                    token_text = "<EOF>";
                }
                
                std::cout << std::setw(5) << i << " | " 
                          << std::setw(20) << token_type_str(token.type) << " | "
                          << std::setw(10) << token.start << " | "
                          << "\"" << token_text << "\"\n";
            }
            
            std::cout << "\n===== End of Lexer Test =====\n";
        } 
        else if (verbose) 
        {
            std::cout << "Lexing successful. Found " << tokens.size() << " tokens.\n";
            
            /* optionally print all tokens for debugging */
            if (debug) 
            {
                std::cout << "Tokens:\n";
                for (size_t i = 0; i < tokens.size(); ++i) 
                {
                    ash::Token token = tokens[i];
                    std::string token_text;
                    
                    if (token.len > 0) 
                    {
                        token_text = std::string(source_view.substr(token.start, token.len));
                    } 
                    else 
                    {
                        token_text = "<EOF>";
                    }
                    
                    std::cout << i << ": " 
                              << token_type_str(token.type) << " '"
                              << token_text << "'\n";
                }
            }
            
            /* TODO: Pass tokens to parser */
            std::cout << "Lexing completed. Parsing not yet implemented.\n";
        }
        
    } 
    catch (const std::exception& e) 
    {
        std::cerr << "Compilation failed: " << e.what() << "\n";
        return 1;
    }

    return 0;
}