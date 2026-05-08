#include <CL/cl.h>
#include <iostream>
#include <vector>
#include "openCL.hpp"
#include <fstream>
#include <sstream>

/* Prints out information about an openCL platform.
 * @param _id ID of platform
 * @param _attribute Attribute wanted - e.g. name or version
 * @param _bufferSize Space for saving response
*/
void printPlatformAttribute(cl_platform_id const& _id, cl_platform_info const& _attribute, size_t const& _bufferSize) {
	char* attribute = new char[_bufferSize]; 
	size_t returnedSize = 0;
	cl_int const error = clGetPlatformInfo(_id, _attribute, _bufferSize, attribute, &returnedSize); // ask for information
	if (CL_SUCCESS != error) { // check for success
		std::cerr << "Couldn't get platform attribute " << _attribute << "." << std::endl;
	} /* else { // print information to cout
		std::string const string(attribute, returnedSize); 
		std::cout << string.c_str() << std::endl; 
	}*/
	delete[] attribute; // free memory
}

/* Prints out information about an openCL device.
 * @param _id ID of device 
 * @param _attribute Attribute wanted - e.g. name or compute units
 * @param _bufferSize Space for saving response
 */
void printDeviceAttribute(cl_device_id const& _id, cl_device_info const& _attribute, size_t const& _bufferSize) {
	char* attribute = new char[_bufferSize];
	size_t unusedReturnedSize = 0;
	cl_int const error = clGetDeviceInfo(_id, _attribute, _bufferSize, attribute, &unusedReturnedSize); // ask for information 
	if (CL_SUCCESS != error) { // check for success
		std::cerr << "Couldn't get device attribute " << _attribute << "." << std::endl;
	} /* else { // print information to cout
		std::cout << attribute << std::endl;
	}*/
	delete[] attribute; // free memory
}

/* Detects openCL platforms.
 * @param buffer Size of buffer for attribute output
 * @param index Index of desired platform in list of all platforms
 * @return platform by its id; -1 on error
 */
cl_platform_id getPlatform(size_t buffer, cl_int index) {
	cl_uint detectedNumberOfPlatforms = 0;
	cl_uint const SEARCH_PLATFORMS = 5;
	cl_platform_id platforms[SEARCH_PLATFORMS];
	cl_int error = clGetPlatformIDs(SEARCH_PLATFORMS, platforms, &detectedNumberOfPlatforms); // search for platforms
	if (CL_SUCCESS == error) { // check for success and print number of platforms
		// std::cout << "Detected OpenCL platforms: " << detectedNumberOfPlatforms << std::endl;

		// get and print name of platform as openCL attribute
		size_t platformBufferSize = buffer;
		printPlatformAttribute(platforms[index], CL_PLATFORM_NAME, platformBufferSize);
	
		return platforms[index];
	} else { // in case of error
		std::cerr << "Couldn't detect OpenCL platforms. Error code: " << error << std::endl;
		return (cl_platform_id) -1;
	}
}


/* Checks for devices of an openCL platform.
 * @platform ID of platform their devices are wanted
 * @param index Index of desired device
 * @return id of device at index or -1 on error
 */
cl_device_id getDevice(cl_platform_id platform, cl_int index) {
	cl_uint numberOfDevices = 0;
	cl_uint const MAX_SEARCH_DEVICES = 30;
	cl_device_id devices[MAX_SEARCH_DEVICES];
	cl_int error = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, MAX_SEARCH_DEVICES, devices, &numberOfDevices);
	if (CL_SUCCESS == error) { // check for success and print number of devices
		// std::cout << "Detected devices: " << numberOfDevices << std::endl;
		
		// get and print device name as openCL attribute
		size_t bufferSizeDevice = 100;
		printDeviceAttribute(devices[index], CL_DEVICE_NAME, bufferSizeDevice); 

		return devices[index];
	} else { 
		std::cerr << "Couldn't detect OpenCL devices. Error code: " << error << std::endl;
		numberOfDevices = 0;
		return (cl_device_id) -1;
	}
}

/* Creates an OpenCL context for the given platform and device type. 
 * Platform must be specified in the context properties, else -32 (CL_INVALID_PLATFORM) is returned.
 * @param platform OpenCL platform ID	
 * @param deviceType type of device (e.g., CL_DEVICE_TYPE_GPU)
 * @return Created OpenCL context, nullptr if an error occurred
 */
cl_context getContext(cl_platform_id platform, cl_device_id *devices, cl_uint deviceCount) {
	// define properties for context
	cl_context_properties contextProperties[] = {
		CL_CONTEXT_PLATFORM,
		(cl_context_properties) platform,
		0
	};

	// try to create context
	cl_int contextErr;
	cl_context context = clCreateContext(contextProperties, deviceCount, devices, NULL, NULL, &contextErr);
	if (CL_SUCCESS != contextErr) { // check for error
		std::cout << "Context error: " << contextErr << " in Line " << __LINE__ << " in " << __FILE__ << std::endl;
		return nullptr;
	}

	// get size of buffer for saving device id in context
	size_t buffer_size = 0;
	clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &buffer_size);

	// get device information
	size_t numDevices = buffer_size / sizeof(cl_device_id); // compute number of devices
	std::vector<cl_device_id> devicesInfo(numDevices);
	
	// print out content of buffer before filling it
	/*for (int i = 0; i < numDevices; i++) {
		std::cout << "buffer before initialization: " << devicesInfo[i] << std::endl; 
	}*/

	cl_int infoError = clGetContextInfo(context, CL_CONTEXT_DEVICES, buffer_size, devicesInfo.data(), NULL); // get device information from context and write them to deviceInfo
	// TODO: Verlagern der Ausgabe / Prüfen ob != 0
	//std::cout << "error: " << infoError << std::endl; // check for error
	/*for (int i = 0; i < numDevices; i++) { // print content of buffer
		std::cout << "Context Info: " << devicesInfo[i] << std::endl;
	}*/

	return context;
}

/* Loads source code for openCL kernel from file.
 * @param path Path to file
 * @return kernel code as string
 */
std::string loadKernelSource(const std::string& path) {
	std::ifstream file(path); // create input file
	if (!file.is_open()) {
		throw std::runtime_error("Kernel file not found");
	}

	std::ostringstream oss; // create buffer
	oss << file.rdbuf(); // read content of file to buffer
	return oss.str(); // convert buffer to string
}

