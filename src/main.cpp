#include <string>
#include <boost/program_options/value_semantic.hpp>
#include <iostream>
#include <boost/program_options.hpp>
#include <vector>

// Include the OpenCL headers as our utility code
#include "ocl_utility.hpp"

// Include the Video class
#include "VideoReaderFFMPEG.hpp"
#include "VideoWriterFFMPEG.hpp"

cl_event vectorInit(cl_command_queue q, cl_kernel vecinit_k, cl_int nels,size_t lws_in,
	cl_mem d_v1, cl_mem d_v2)
{
	const size_t gws[] = { ocl::round_mul_up(nels, lws_in) };

	printf("number of elements %d round to %zu GWS %zu\n", nels, lws_in, gws[0]); // vecinit not used since we are not using a local work size

	cl_int err = clSetKernelArg(vecinit_k, 0, sizeof(d_v1), &d_v1);
	ocl::check(err, "setKernelArg vecinit_k 0");

	err = clSetKernelArg(vecinit_k, 1, sizeof(d_v2), &d_v2);
	ocl::check(err, "setKernelArg vecinit_k 1");

	err = clSetKernelArg(vecinit_k, 2, sizeof(nels), &nels);
	ocl::check(err, "setKernelArg vecinit_k 2");

	cl_event vecinit_evt;
	err = clEnqueueNDRangeKernel(q, vecinit_k,
		1, // numero dimensioni
		NULL, // offset
		gws, // global work size
		NULL, // local work size
		0, // numero di elementi nella waiting list
		NULL, // waiting list
		&vecinit_evt); // evento di questo comando
	ocl::check(err, "Enqueue vecinit");

	return vecinit_evt;
}

cl_event vectorAddition(cl_command_queue q, cl_kernel vecadd_k, cl_int nels,size_t lws_in,
    cl_mem d_v1, cl_mem d_v2, cl_mem d_results)
{
    const size_t gws[] = { ocl::round_mul_up(nels, lws_in) };

    printf("number of elements %d round to %zu GWS %zu\n", nels, lws_in, gws[0]); // vecinit not used since we are not using a local work size

    cl_int err = clSetKernelArg(vecadd_k, 0, sizeof(d_v1), &d_v1);
    ocl::check(err, "setKernelArg vecadd_k 0");

    err = clSetKernelArg(vecadd_k, 1, sizeof(d_v2), &d_v2);
    ocl::check(err, "setKernelArg vecadd_k 1");

    err = clSetKernelArg(vecadd_k, 2, sizeof(d_results), &d_results);
    ocl::check(err, "setKernelArg vecadd_k 2");

    err = clSetKernelArg(vecadd_k, 3, sizeof(nels), &nels);
    ocl::check(err, "setKernelArg vecadd_k 3");

    cl_event vecadd_evt;
    err = clEnqueueNDRangeKernel(q, vecadd_k,
        1, // numero dimensioni
        NULL, // offset
        gws, // global work size
        NULL, // local work size
        0, // numero di elementi nella waiting list
        NULL, // waiting list
        &vecadd_evt); // evento di questo comando
    ocl::check(err, "Enqueue vecinit");

    return vecadd_evt;
}

cl_event bgra_to_yuv(cl_command_queue queue, cl_kernel bgra_to_yuv_kernel, cl_int width, cl_int height, size_t lws_in,
    cl_mem input_image_buffer, cl_mem output_image_buffer)
{
    uint nels = width * height;
    const size_t gws[] = { ocl::round_mul_up(nels, lws_in) };

    printf("number of elements %d round to %zu GWS %zu\n", nels, lws_in, gws[0]); // vecinit not used since we are not using a local work size

    cl_int err = clSetKernelArg(bgra_to_yuv_kernel, 0, sizeof(input_image_buffer), &input_image_buffer);
    ocl::check(err, "setKernelArg bgra_to_yuv_kernel 0");

    err = clSetKernelArg(bgra_to_yuv_kernel, 1, sizeof(output_image_buffer), &output_image_buffer);
    ocl::check(err, "setKernelArg bgra_to_yuv_kernel 1");

    err = clSetKernelArg(bgra_to_yuv_kernel, 2, sizeof(width), &width);
    ocl::check(err, "setKernelArg bgra_to_yuv_kernel 2");

    err = clSetKernelArg(bgra_to_yuv_kernel, 3, sizeof(height), &height);
    ocl::check(err, "setKernelArg bgra_to_yuv_kernel 3");

    cl_event bgra_to_yuv_evt;
    err = clEnqueueNDRangeKernel(queue, bgra_to_yuv_kernel,
        1, // numero dimensioni
        NULL, // offset
        gws, // global work size
        NULL, // local work size
        0, // numero di elementi nella waiting list
        NULL, // waiting list
        &bgra_to_yuv_evt); // evento di questo comando
    ocl::check(err, "Enqueue vecinit");

    return bgra_to_yuv_evt;
}

cl_event brga_to_rgba(cl_command_queue queue, cl_kernel bgra_to_rgba_kernel, cl_int width, cl_int height, size_t lws_in,
    cl_mem input_image_buffer, cl_mem output_image_buffer)
{
    uint nels = width * height;
    const size_t gws[] = { ocl::round_mul_up(nels, lws_in) };

    printf("number of elements %d round to %zu GWS %zu\n", nels, lws_in, gws[0]); // vecinit not used since we are not using a local work size

    cl_int err = clSetKernelArg(bgra_to_rgba_kernel, 0, sizeof(input_image_buffer), &input_image_buffer);
    ocl::check(err, "setKernelArg bgra_to_rgba_kernel 0");

    err = clSetKernelArg(bgra_to_rgba_kernel, 1, sizeof(output_image_buffer), &output_image_buffer);
    ocl::check(err, "setKernelArg bgra_to_rgba_kernel 1");

    err = clSetKernelArg(bgra_to_rgba_kernel, 2, sizeof(width), &width);
    ocl::check(err, "setKernelArg bgra_to_rgba_kernel 2");

    err = clSetKernelArg(bgra_to_rgba_kernel, 3, sizeof(height), &height);
    ocl::check(err, "setKernelArg bgra_to_rgba_kernel 3");

    cl_event bgra_to_rgba_evt;
    err = clEnqueueNDRangeKernel(queue, bgra_to_rgba_kernel,
        1, // numero dimensioni
        NULL, // offset
        gws, // global work size
        NULL, // local work size
        0, // numero di elementi nella waiting list
        NULL, // waiting list
        &bgra_to_rgba_evt); // evento di questo comando
    ocl::check(err, "Enqueue vecinit");

    return bgra_to_rgba_evt;
}

cl_event rgba_to_grayscale(cl_command_queue queue, cl_kernel rgba_to_grayscale_kernel, cl_int width, cl_int height, size_t lws_in,
    cl_mem input_image_buffer, cl_mem output_image_buffer)
{
    const size_t gws[] = { ocl::round_mul_up(width, lws_in), ocl::round_mul_up(height, lws_in) };
    printf("number of elements %d round to %zu GWS %zu\n", width * height, lws_in, gws[0]); 
    cl_int err = clSetKernelArg(rgba_to_grayscale_kernel, 0, sizeof(input_image_buffer), &input_image_buffer);
    ocl::check(err, "setKernelArg rgba_to_grayscale_kernel 0");
    err = clSetKernelArg(rgba_to_grayscale_kernel, 1, sizeof(output_image_buffer), &output_image_buffer);
    ocl::check(err, "setKernelArg rgba_to_grayscale_kernel 1");
    err = clSetKernelArg(rgba_to_grayscale_kernel, 2, sizeof(width), &width);
    ocl::check(err, "setKernelArg rgba_to_grayscale_kernel 2");
    err = clSetKernelArg(rgba_to_grayscale_kernel, 3, sizeof(height), &height);
    ocl::check(err, "setKernelArg rgba_to_grayscale_kernel 3");
    cl_event rgba_to_grayscale_evt;
    err = clEnqueueNDRangeKernel(queue, rgba_to_grayscale_kernel,
        2, // numero dimensioni
        NULL, // offset
        gws, // global work size
        NULL, // local work size
        0, // numero di elementi nella waiting list
        NULL, // waiting list
        &rgba_to_grayscale_evt); // evento di questo comando
    ocl::check(err, "Enqueue rgba_to_grayscale");
    return rgba_to_grayscale_evt;
}

cl_event uniform_quantize(cl_command_queue queue, cl_kernel uniform_quantize_kernel, cl_int width, cl_int height, size_t lws_in,
    cl_mem input_image_buffer, cl_mem output_image_buffer, int levels)
{
    const size_t gws[] = { ocl::round_mul_up(width, lws_in), ocl::round_mul_up(height, lws_in) };
    printf("number of elements %d round to %zu GWS %zu\n", width * height, lws_in, gws[0]); 
    cl_int err = clSetKernelArg(uniform_quantize_kernel, 0, sizeof(input_image_buffer), &input_image_buffer);
    ocl::check(err, "setKernelArg uniform_quantize_kernel 0");
    err = clSetKernelArg(uniform_quantize_kernel, 1, sizeof(output_image_buffer), &output_image_buffer);
    ocl::check(err, "setKernelArg uniform_quantize_kernel 1");
    err = clSetKernelArg(uniform_quantize_kernel, 2, sizeof(width), &width);
    ocl::check(err, "setKernelArg uniform_quantize_kernel 2");
    err = clSetKernelArg(uniform_quantize_kernel, 3, sizeof(height), &height);
    ocl::check(err, "setKernelArg uniform_quantize_kernel 3");
    err = clSetKernelArg(uniform_quantize_kernel, 4, sizeof(levels), &levels);
    ocl::check(err, "setKernelArg uniform_quantize_kernel 4");
    cl_event uniform_quantize_evt;
    err = clEnqueueNDRangeKernel(queue, uniform_quantize_kernel,
        2, // numero dimensioni
        NULL, // offset
        gws, // global work size
        NULL, // local work size
        0, // numero di elementi nella waiting list
        NULL, // waiting list
        &uniform_quantize_evt); // evento di questo comando
    ocl::check(err, "Enqueue uniform_quantize");
    return uniform_quantize_evt;
}

cl_event quantize_binarize(cl_command_queue queue, cl_kernel uniform_quantize_kernel, cl_int width, cl_int height, size_t lws_in,
    cl_mem input_image_buffer, cl_mem output_image_buffer)
{
    const size_t gws[] = { ocl::round_mul_up(width, lws_in), ocl::round_mul_up(height, lws_in) };
    printf("number of elements %d round to %zu GWS %zu\n", width * height, lws_in, gws[0]); 
    cl_int err = clSetKernelArg(uniform_quantize_kernel, 0, sizeof(input_image_buffer), &input_image_buffer);
    ocl::check(err, "setKernelArg binarize 0");
    err = clSetKernelArg(uniform_quantize_kernel, 1, sizeof(output_image_buffer), &output_image_buffer);
    ocl::check(err, "setKernelArg binarize 1");
    err = clSetKernelArg(uniform_quantize_kernel, 2, sizeof(width), &width);
    ocl::check(err, "setKernelArg binarize 2");
    err = clSetKernelArg(uniform_quantize_kernel, 3, sizeof(height), &height);
    ocl::check(err, "setKernelArg binarize 3");
    cl_event uniform_quantize_evt;
    err = clEnqueueNDRangeKernel(queue, uniform_quantize_kernel,
        2, // numero dimensioni
        NULL, // offset
        gws, // global work size
        NULL, // local work size
        0, // numero di elementi nella waiting list
        NULL, // waiting list
        &uniform_quantize_evt); // evento di questo comando
    ocl::check(err, "Enqueue uniform_quantize");
    return uniform_quantize_evt;
}



int main(int argc, char** argv) {
    // Initialize the program options
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    std::string input_file, output_file;
    bool binarize = false, grayscale = false;
    // Add options
    desc.add_options()
        ("help,h", "produce help message")
        ("input,i", po::value<std::string>(), "input video file name")
        ("levels,l", po::value<int>(), "number of levels for quantization")
        ("binarize", po::bool_switch(&binarize)->default_value(false), "binarize the image, making the levels of the quantization 0 and 1 for every channel, meaning that the value will be either 0 or 255")
        ("grayscale", po::bool_switch(&grayscale)->default_value(false), "convert to grayscale using the luminosity method")
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
        input_file = vm["input"].as<std::string>();
        std::cout << "Input file: " << input_file << std::endl;
        // Check if the input file exists
        std::ifstream file(input_file);
        if (!file) {
            std::cerr << "Input file does not exist: " << input_file << "\n";
            return 1;
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

    // Check if the levels for quantization are provided
    int levels = 0;
    if (vm.count("levels")) {
        levels = vm["levels"].as<int>();
        // control if the levels are less than 256 and more or equal than 2
        if (levels < 2 || levels > 256) {
            std::cerr << "The number of levels for quantization must be between 2 and 256.\n";
            return 1;
        }
        std::cout << "Levels for quantization: " << levels << "\n";
    } else {
        std::cerr << "No levels for quantization provided.\n";
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
    cl_program program = ocl::create_program("src/kernels/uniformQuantization.cl", context, device);
    // Program for testing vector addition
    cl_program program2 = ocl::create_program("src/kernels/operations.cl", context, device);

    // test the vector addition on a small vector
    const int N = 512*512;
    std::vector<float> a(N, 1.0f);
    std::vector<float> b(N, 2.0f);
    std::vector<float> results(N, 0.0f);
    cl_int err;
    cl_mem a_buf = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, N * sizeof(float), a.data(), &err);
    ocl::check(err, "Creating buffer for a");
    cl_mem b_buf = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, N * sizeof(float), b.data(), &err);
    ocl::check(err, "Creating buffer for b");
    cl_mem result_buf = clCreateBuffer(context, CL_MEM_WRITE_ONLY, N * sizeof(float), nullptr, &err);
    ocl::check(err, "Creating buffer for results");
    // Create the kernel for vector initialization
    cl_kernel vecinit_k = clCreateKernel(program2, "vector_initialization_twice", &err);
    ocl::check(err, "Creating kernel vecinit");

    // get information on the preferred work group size
    size_t lws_in = 0;
    err = clGetKernelWorkGroupInfo(vecinit_k, device, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE,
        sizeof(lws_in), &lws_in, nullptr);  // TODO also change from parameters in the future
    ocl::check(err, "Getting preferred work group size");
    
    // Get the event for the kernel
    cl_event vecinit_evt = vectorInit(queue, vecinit_k, N, lws_in, a_buf, b_buf);
    // Wait for the event to complete
    clWaitForEvents(1, &vecinit_evt);
    // Create the kernel for vector addition
    cl_kernel vecadd_k = clCreateKernel(program2, "vector_addition4", &err);
    ocl::check(err, "Creating kernel vecadd");
    // get information on the preferred work group size
    err = clGetKernelWorkGroupInfo(vecadd_k, device, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE,
        sizeof(lws_in), &lws_in, nullptr);  // TODO also change from parameters in the future
    ocl::check(err, "Getting preferred work group size");
    // Get the event for the kernel
    cl_event vecadd_evt = vectorAddition(queue, vecadd_k, N, lws_in, a_buf, b_buf, result_buf);
    // Wait for the event to complete
    clWaitForEvents(1, &vecadd_evt);
    // Read the vector initialization results
    err = clEnqueueReadBuffer(queue, a_buf, CL_TRUE, 0, N * sizeof(float), a.data(), 0, nullptr, nullptr);
    ocl::check(err, "Reading buffer a");
    err = clEnqueueReadBuffer(queue, b_buf, CL_TRUE, 0, N * sizeof(float), b.data(), 0, nullptr, nullptr);
    ocl::check(err, "Reading buffer b");
    err = clEnqueueReadBuffer(queue, result_buf, CL_TRUE, 0, N * sizeof(float), results.data(), 0, nullptr, nullptr);
    ocl::check(err, "Reading buffer results");
    // Control if the results are correct, meaning vector a have increasing values from 0 to N, and vector b has decreasing values from N to 0
    for (int i = 0; i < N; ++i) {
        if (results[i] != a[i] + b[i]) {
            std::cout << "Results are not correct at index " << i << ": " << results[i] << "\n";
            break;
        }
    }
    // free the buffers
    clReleaseMemObject(a_buf);
    clReleaseMemObject(b_buf);
    clReleaseMemObject(result_buf);
    // free the kernels
    clReleaseKernel(vecinit_k);
    clReleaseKernel(vecadd_k);
    // free the program
    clReleaseProgram(program2);

    // testing the reading of the video
    VideoReaderFFMPEG video(input_file);
    std::vector<uint8_t> frame_data(video.get_width() * video.get_height() * 4); // BGRA RGB32
    // std::vector<uint8_t> frame_data(video.get_width() * video.get_height() * 3); // YUV444P
    // get first frame for testing
    // if(video.read_next_frame(frame_data)) {
    //     std::cout << "Read first frame of size: " << frame_data.size() << "\n";
    // } else {
    //     std::cerr << "Failed to read first frame.\n";
    //     return 1;
    // }
    // start working on the video frames
    // create the kernel for the conversion
    // cl_kernel bgra_to_yuv_kernel = clCreateKernel(program2, "bgra_to_yuv", &err);
    // ocl::check(err, "Creating kernel bgra_to_yuv");
    // // get information on the preferred work group size
    // size_t lws_in = 0;
    // err = clGetKernelWorkGroupInfo(bgra_to_yuv_kernel, device, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE,
    //     sizeof(lws_in), &lws_in, nullptr);  // TODO also change from parameters in the future
    // ocl::check(err, "Getting preferred work group size");
    // // create the kernel for the conversion
    cl_kernel bgra_to_rgba_kernel = clCreateKernel(program, "brga_to_rgba", &err);
    ocl::check(err, "Creating kernel bgra_to_rgba");
    std::vector<uint8_t> frame_data_output(video.get_width() * video.get_height() * 4); // RGBA
    cl_kernel quantization_kernel = clCreateKernel(program, "uniform_quantize_nearest", &err);
    ocl::check(err, "Creating kernel quantization");
    // get information on the preferred work group size
    err = clGetKernelWorkGroupInfo(quantization_kernel, device, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE,
        sizeof(lws_in), &lws_in, nullptr);  // TODO also change from parameters in the future
    ocl::check(err, "Getting preferred work group size");

    VideoWriterFFMPEG videoOutput(output_file, video.get_width(), video.get_height(), video.get_fps());
    while(video.read_next_frame(frame_data)) {
        // process the frame data
        // convert the BRGA to RGBA, since the conversion in FFMPEG has some problems
        cl_mem input_image_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            frame_data.size(), frame_data.data(), &err);
        ocl::check(err, "Creating buffer for input image");
        // create the buffer for the output image
        cl_mem output_image_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
            frame_data.size(), nullptr, &err);
        ocl::check(err, "Creating buffer for output image");
        // create the event
        cl_event bgra_to_rgba_evt = brga_to_rgba(queue, bgra_to_rgba_kernel,
            video.get_width(), video.get_height(), lws_in, input_image_buffer, output_image_buffer);
        // wait for the event to complete
        clWaitForEvents(1, &bgra_to_rgba_evt);
        // quantize the image depending on input parameters
        // the input parameters will establish which kernel to use and the number of levels in case of quantization with more than 2 levels

        // read the output image
        err = clEnqueueReadBuffer(queue, output_image_buffer, CL_TRUE, 0,
            frame_data_output.size(), frame_data_output.data(), 0, nullptr, nullptr);
        ocl::check(err, "Reading output image");
        // free the buffers
        clReleaseMemObject(input_image_buffer);
        clReleaseMemObject(output_image_buffer);
        // free the event
        clReleaseEvent(bgra_to_rgba_evt);
        // write the frame to the output file
        videoOutput.write_frame(frame_data_output.data());
    }
    

    return 0;
}