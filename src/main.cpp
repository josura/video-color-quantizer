#include <string>
#include <boost/program_options/value_semantic.hpp>
#include <iostream>
#include <boost/program_options.hpp>
#include <vector>

// Include the OpenCL headers as our utility code
#include "ocl_utility.hpp"


int main(int argc, char** argv) {
    // Initialize the program options
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    std::string input_file, output_file;
    // Add options
    desc.add_options()
        ("help,h", "produce help message")
        ("input,i", po::value<std::vector<std::string>>(), "input video file name")
        ("output,o", po::value<std::string>(), "output video file name");
    
    // Parse the command line arguments
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    // Check if help is requested
    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 0;
    }
    // Check if input file is provided
    if (vm.count("input")) {
        std::vector<std::string> input_files = vm["input"].as<std::vector<std::string>>();
        for (const auto& file : input_files) {
            std::cout << "Input file: " << file << "\n";
        }
    } else {
        std::cerr << "No input file provided.\n";
        return 1;
    }
    // Check if output file is provided
    if (vm.count("output")) {
        output_file = vm["output"].as<std::string>();
        std::cout << "Output file: " << output_file << "\n";
    } else {
        std::cerr << "No output file provided.\n";
        return 1;
    }
    // Select the OpenCL platform
    cl_platform_id platform = ocl::select_platform();
    // Select the OpenCL device
    cl_device_id device = ocl::select_device(platform);
    // Create the OpenCL context
    cl_context context = ocl::create_context(platform, device);
    // Create the command queue
    cl_command_queue queue = ocl::create_queue(context, device);
    // Create the OpenCL program
    cl_program program = ocl::create_program("kernels/uniformQuantization.cl", context, device);
    // Program for testing vector addition
    cl_program program2 = ocl::create_program("kernels/operations.cl", context, device);
    
    

    return 0;
}