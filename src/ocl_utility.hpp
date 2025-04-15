/**
 * @file ocl_utility.hpp
 * @brief A modern C++ utility header for managing common OpenCL boilerplate code.
 *
 * This header provides helper functions for selecting platforms and devices,
 * creating OpenCL contexts and queues, compiling programs, and profiling events.
 * The goal is to minimize redundant code when initializing and managing OpenCL.
 * 
 * Adapted and extended from a C version for use with C++.
 */

#ifndef OCL_UTILITY_HPP
#define OCL_UTILITY_HPP

#define CL_TARGET_OPENCL_VERSION 120
#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdarg>
#include <ctime>

namespace ocl {
    constexpr size_t BUFSIZE = 4096;

    /**
     * @brief Checks an OpenCL error and throws an exception if an error occurred.
     * @param err The OpenCL error code.
     * @param msg A printf-style error message.
     */
    inline void check(cl_int err, const char* msg, ...) {
        if (err != CL_SUCCESS) {
            char buffer[BUFSIZE + 1];
            va_list args;
            va_start(args, msg);
            vsnprintf(buffer, BUFSIZE, msg, args);
            va_end(args);
            buffer[BUFSIZE] = '\0';
            std::cerr << buffer << " - error " << err << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    /**
     * @brief Selects an OpenCL platform, optionally via the OCL_PLATFORM environment variable.
     * @return The selected cl_platform_id.
     */
    inline cl_platform_id select_platform() {
        cl_uint n_platforms;
        check(clGetPlatformIDs(0, nullptr, &n_platforms), "Getting platform count");
        std::vector<cl_platform_id> platforms(n_platforms);
        check(clGetPlatformIDs(n_platforms, platforms.data(), nullptr), "Getting platforms");

        const char* env = std::getenv("OCL_PLATFORM");
        cl_uint index = (env && env[0] != '\0') ? std::atoi(env) : 0;

        if (index >= n_platforms) {
            std::cerr << "Invalid platform index: " << index << std::endl;
            std::exit(EXIT_FAILURE);
        }

        char name[BUFSIZE];
        check(clGetPlatformInfo(platforms[index], CL_PLATFORM_NAME, BUFSIZE, name, nullptr), "Getting platform name");
        std::cout << "Selected platform " << index << ": " << name << std::endl;

        return platforms[index];
    }

    /**
     * @brief Selects an OpenCL device, optionally via the OCL_DEVICE environment variable.
     * @param platform The OpenCL platform to search devices on.
     * @return The selected cl_device_id.
     */
    inline cl_device_id select_device(cl_platform_id platform) {
        cl_uint n_devices;
        check(clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, nullptr, &n_devices), "Getting device count");
        std::vector<cl_device_id> devices(n_devices);
        check(clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, n_devices, devices.data(), nullptr), "Getting devices");

        const char* env = std::getenv("OCL_DEVICE");
        cl_uint index = (env && env[0] != '\0') ? std::atoi(env) : 0;

        if (index >= n_devices) {
            std::cerr << "Invalid device index: " << index << std::endl;
            std::exit(EXIT_FAILURE);
        }

        char name[BUFSIZE];
        check(clGetDeviceInfo(devices[index], CL_DEVICE_NAME, BUFSIZE, name, nullptr), "Getting device name");
        std::cout << "Selected device " << index << ": " << name << std::endl;

        return devices[index];
    }

    /**
     * @brief Creates an OpenCL context for a single device.
     * @param platform The platform used.
     * @param device The device to create a context for.
     * @return A valid OpenCL context.
     */
    inline cl_context create_context(cl_platform_id platform, cl_device_id device) {
        cl_int err;
        cl_context_properties props[] = {
            CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(platform), 0
        };
        cl_context ctx = clCreateContext(props, 1, &device, nullptr, nullptr, &err);
        check(err, "Creating context");
        return ctx;
    }

    /**
     * @brief Creates a command queue with profiling enabled.
     * @param context The OpenCL context.
     * @param device The OpenCL device.
     * @return A command queue.
     */
    inline cl_command_queue create_queue(cl_context context, cl_device_id device) {
        cl_int err;
        cl_command_queue queue = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &err);
        check(err, "Creating command queue");
        return queue;
    }

    /**
     * @brief Creates and builds an OpenCL program from a source file.
     * @param filename The file containing the OpenCL kernel code.
     * @param context The OpenCL context.
     * @param device The target device.
     * @return A built OpenCL program.
     */
    inline cl_program create_program(const std::string& filename, cl_context context, cl_device_id device) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open kernel file: " << filename << std::endl;
            std::exit(EXIT_FAILURE);
        }

        std::ostringstream oss;
        oss << file.rdbuf();
        std::string source = oss.str();
        const char* src_ptr = source.c_str();
        cl_int err;

        cl_program program = clCreateProgramWithSource(context, 1, &src_ptr, nullptr, &err);
        check(err, "Creating program");

        err = clBuildProgram(program, 1, &device, "-I.", nullptr, nullptr);

        size_t log_size;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &log_size);
        std::vector<char> log(log_size);
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, log.data(), nullptr);
        if (!log.empty()) {
            std::cout << "=== BUILD LOG ===\n" << log.data() << "\n==================\n";
        }

        check(err, "Building program");
        return program;
    }

    /**
     * @brief Computes the runtime of an event in nanoseconds.
     * @param evt An OpenCL event.
     * @return Duration in nanoseconds.
     */
    inline cl_ulong runtime_ns(cl_event evt) {
        cl_ulong start = 0, end = 0;
        check(clGetEventProfilingInfo(evt, CL_PROFILING_COMMAND_START, sizeof(start), &start, nullptr), "Profiling start");
        check(clGetEventProfilingInfo(evt, CL_PROFILING_COMMAND_END, sizeof(end), &end, nullptr), "Profiling end");
        return end - start;
    }

    /**
     * @brief Computes the runtime of an event in milliseconds.
     * @param evt An OpenCL event.
     * @return Duration in milliseconds.
     */
    inline double runtime_ms(cl_event evt) {
        return runtime_ns(evt) * 1.0e-6;
    }

    /**
     * @brief Divides and rounds up (gws / lws).
     */
    inline size_t round_div_up(size_t gws, size_t lws) {
        return (gws + lws - 1) / lws;
    }

    /**
     * @brief Rounds global work size to the nearest multiple of local work size.
     */
    inline size_t round_mul_up(size_t gws, size_t lws) {
        return round_div_up(gws, lws) * lws;
    }
} // namespace ocl
 
#endif // OCL_UTILITY_HPP
 