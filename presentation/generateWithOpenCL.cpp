std::vector<Cuboid> generateWithOpenCL(std::vector<Cuboid>& cuboids, Renderer& renderer, unsigned const int depth, double& build_time) {
	cl_platform_id platform = getPlatform(100, 0);
	cl_device_id device = getDevice(platform, 0);
	cl_context context = getContext(platform, CL_DEVICE_TYPE_ALL);
	if (context == nullptr) {
		return std::vector<Cuboid>();
	}

	// create command queue 
	cl_int err = 0;
	cl_command_queue queue = clCreateCommandQueueWithProperties(context, device, NULL, &err);
	std::cout << "Queue: " << queue << std::endl;
	if (CL_SUCCESS != err) {
		std::cout << "Queue error code: " << err << std::endl;
		return std::vector<Cuboid>();
	}

	// load kernel from kernel.cl
	const char* kernelSource;
	const std::string kernelSource_str = loadKernelSource("kernel.cl");
	kernelSource = kernelSource_str.c_str();

	// create program
	cl_uint count = 1;
	cl_program program = clCreateProgramWithSource(context, count, &kernelSource, NULL, &err);
	if (CL_SUCCESS != err) {
		std::cout << "CreateProgram returned error: " << err << std::endl;
		return std::vector<Cuboid>();
	}

	// compile program
	cl_uint num_devices = 1;
	cl_device_id devices[1] = { device };

	// time mesurement
	clock_t start, end;
	start = clock();
	err = clBuildProgram(program, num_devices, devices, NULL, NULL, NULL);
	end = clock();
	build_time = ((double)(end - start)) / CLOCKS_PER_SEC; // Convert to seconds

	if (CL_SUCCESS != err) {
		std::cout << "Build program returned error: " << err << std::endl;
		size_t logSize;
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize);
		std::vector<char> log(logSize);
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, logSize, log.data(), nullptr);
		std::cout << "Build info: " << log.data() << std::endl;
		return std::vector<Cuboid>();
	}

	// check for compilation errors
	size_t param_value_size_ret;
	err = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &param_value_size_ret); // request size and write to param_value_size_ret
	if (CL_SUCCESS != err) {
		std::cout << "Get build info returned error: " << err << std::endl;
		return std::vector<Cuboid>();
	}

	std::vector<char> param_value_size(param_value_size_ret); // create buffer with requested size
	err = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, param_value_size_ret, param_value_size.data(), NULL); // request information
	if (CL_SUCCESS != err) {
		std::cout << "Get build info returned error: " << err << std::endl;
		return std::vector<Cuboid>();
	}

	std::cout << "Build status: " << param_value_size.data() << std::endl;

	// create kernel for specific device
	cl_int* kernel_err = 0;
	cl_kernel kernel = clCreateKernel(program, "buildTree", kernel_err);

	std::vector<Cuboid> input = cuboids;
	std::vector<Cuboid> result(input.size() * 2);
	result.resize(input.size() * 2);


	int local_depth = 0;

	while (local_depth < depth) {
		for (auto& c : input) {
			addCuboidToRenderer(c, renderer);
		}

		// create buffer quboid
		size_t bufferSize = 1;
		cl_int* bufferAErr = 0;
		cl_mem bufferCuboid = clCreateBuffer(context, CL_MEM_READ_ONLY, input.size() * sizeof(Cuboid), NULL, &err);
		if (CL_SUCCESS != err) {
			std::cout << "Buffer A creation error: " << err << std::endl;
			return std::vector<Cuboid>();
		}

		// create resultbuffer
		cl_mem resultBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE, input.size() * 2 * sizeof(Cuboid), NULL, &err);
		if (CL_SUCCESS != err) {
			std::cout << "Output buffer creation error: " << err << std::endl;
			return std::vector<Cuboid>();
		}

		err = clEnqueueWriteBuffer(queue, bufferCuboid, CL_BLOCKING, 0, input.size() * sizeof(Cuboid), input.data(), 0, NULL, NULL);
		if (CL_SUCCESS != err) {
			std::cout << "Write buffer B writing error: " << err << std::endl;
			return std::vector<Cuboid>();
		}

		err = clEnqueueWriteBuffer(queue, resultBuffer, CL_BLOCKING, 0, input.size() * 2 * sizeof(Cuboid), result.data(), 0, NULL, NULL);
		if (CL_SUCCESS != err) {
			std::cout << "Write result buffer writing error: " << err << std::endl;
			return std::vector<Cuboid>();
		}

		// set kernel arguments
		err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &resultBuffer);
		err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferCuboid);
		if (CL_SUCCESS != err) {
			std::cout << "Set kernel argument error. " << err << std::endl;
			return std::vector<Cuboid>();
		}

		// running a kernel
		cl_uint  work_dim = 1; // dimension (1D, 2D oder 3D)
		const size_t global_work_size = input.size(); // set number of work items (threads) per dimension
		const size_t local_work_size = 1; // set size of work group
		err = clEnqueueNDRangeKernel(queue, kernel, work_dim, NULL, &global_work_size, &local_work_size, 0, NULL, NULL);
		if (CL_SUCCESS != err) {
			std::cout << "Run error: " << err << std::endl;
			return std::vector<Cuboid>();
		}

		result.assign(input.size() * 2, Cuboid());

		// read values from device 
		cl_bool blocking_read = CL_TRUE;
		clEnqueueReadBuffer(queue, resultBuffer, blocking_read, 0, result.size() * sizeof(Cuboid), result.data(), 0, NULL, NULL);

		input = result;
		result = std::vector<Cuboid>(input.size() * 2);
		local_depth += 1;
	}

	return input;
}
