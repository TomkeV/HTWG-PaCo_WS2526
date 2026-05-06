#pragma once
#include <CL/cl.h>
#include <iostream>
#include <vector>

// print information about openCL platform
void printPlatformAttribute(cl_platform_id const& _id, cl_platform_info const& _attribute, size_t const& _bufferSize);

// print information about openCL device
void printDeviceAttribute(cl_device_id const& _id, cl_device_info const& _attribute, size_t const& _bufferSize);

// detect openCL platform
cl_platform_id getPlatform(size_t buffer, cl_int index);

// check devices of openCL platform
cl_device_id getDevice(cl_platform_id platform, cl_int index);

// create openCL context
cl_context getContext(cl_platform_id platform, cl_device_id *devices, cl_uint deviceCount);

// load openCL kernel as string
std::string loadKernelSource(const std::string& path);