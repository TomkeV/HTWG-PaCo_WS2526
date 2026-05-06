#include "renderer.hpp"
#include "cuboid.hpp"
#include "openCL.hpp"
#include "point.hpp"
#include <cstdlib>


#include <ctime>
#ifdef _WIN32
//#include <time.h>
#else
#include <sys/time.h>
#endif

#include <CL/cl.h>
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <omp.h>

// --------------------------------------------------------------------------------
// -------------------------------- System control --------------------------------
bool randomSize = false;
bool randomAngle = false;
// bool exitOnSidelength = false; // otherwise exit on treedepth
#define TREESIZE 15
#define STARTSIZE 2

// ---------------------------------------------------------------------------
// -------------------------------- Functions --------------------------------
/* chooseColour() decides which colour a render object is depicted with
 * depending on the sidelength a in correlation to the STARTSIZE
 */
Vector3D chooseColour(const Cuboid& cube) {
    Vector3D side = createVector3dFromPoints(cube.points[0], cube.points[1]);
    double sidelength = getLengthOf3DVector(side);
    double threshold = STARTSIZE / 7.0;

    if (sidelength >= threshold * 6.0) {
        return BROWN_DARK;
    } 
    else if (sidelength >= threshold * 4.0) {
        return BROWN_STANDARD;
    } 
    else if (sidelength >= threshold * 3.0) {
        return BROWN_LIGHT;
    } 
    else if (sidelength >= threshold * 2.0) {
        return GREEN_LIGHT; 
    } 
    else if (sidelength >= threshold) {
        return GREEN_STANDARD;
    } 
    return GREEN_DARK; // Default case
}

/* Adds a cuboid as a render object to the given renderer.
 * @param cube Cuboid to add
 * @param renderer Renderer to add cuboid to
 */
void addCuboidToRenderer(const Cuboid &cube, Renderer& renderer) {
	RenderObject cubeObject;
	cubeObject.verts.reserve(8);
	cubeObject.edges.reserve(12);
	const Point* const points = cube.points;

	for (int i = 0; i < 8; i++) {
		cubeObject.verts.push_back(toVector3D(points[i]));
	}
	for (int i = 0; i < 12; i++) {
		cubeObject.edges.push_back({ cube.xedges[i], cube.yedges[i] });
	}

	cubeObject.color = chooseColour(cube); // set color
#pragma omp critical
	renderer.addRenderObject(std::move(cubeObject));
}

/* Computes a cuboid with fixed width and depth and - depending on control settings - random height
 * that is rotated around a given angle positioned with a given vector from point (0|0|0). 
 * @param length Width and depth of the cuboid
 * @param angle Angle cuboid should be rotated
 * @param vector Delay of cuboid from (0|0|0)
 * @return new cuboid with given size in given position
 */
Cuboid getNextCuboid(double length, double angle, double* vector) {
	double height;
	if (randomSize) { 
		height = (rand() % 100 * (2*length)) / 100; // random height
	} else {
		height = length;
	}
	Cuboid newCuboid = createCuboidFromSidelength(height, length, STARTSIZE);
    return transform(newCuboid,angle, 'y', vector);
}

/* Compute sidelength of the next cuboid by using law of sine.
 * @param newSidelength Output param as vector { a, b, c } containing the lengths as double
 */
void getNewSidelength(Point corner_a, Point corner_b, double alpha, double* newSidelength) {
	// compute the angles
	double gamma = 90.0; // gamma is always the right angle.
	double beta = 180.0 - abs(alpha) - gamma;

	// compute the sides
	newSidelength[2] = sqrt(pow((corner_b.x - corner_a.x), 2) + pow((corner_b.y - corner_a.y), 2) + pow((corner_b.z - corner_a.z), 2)); // |AB|
	newSidelength[0] = (newSidelength[2] / std::sin(gamma / 180 * M_PI)) * std::sin(alpha / 180 * M_PI); // law of sine
	newSidelength[1] = (newSidelength[2] / std::sin(gamma / 180 * M_PI)) * std::sin(beta / 180 * M_PI); // law of sine
}

// -----------------------------------------------------------------------
// -------------------------------- Serial--------------------------------

// -------------------------------- Recursion ----------------------------
int depth = 0;
// does not work!
/*void generateSerialRecursive(Cuboid cuboid, Renderer& renderer, double oldAngle) {
	depth++;
	// exit condition
	double siteLength = getSidelength(cuboid)[0];
	if (siteLength < 0.5) {
		return;
	}
	if (depth > TREESIZE) {
		return;
	}

	// Definition of render object
	RenderObject cube;
	Point* points = cuboid.points; // getPoints(cuboid);
	for (int i = 0; i < 8; i++) {
		cube.verts.push_back(toVector3D(points[i]));
	}
	for (int i = 0; i < 12; i++) {
		std::pair<int, int> edge = { cuboid.xedges[i], cuboid.yedges[i] };
		cube.edges.push_back(edge);
	}
	//cube.edges = cuboid.edges;
	cube.color = chooseColour(cuboid);
	renderer.addRenderObject(cube);
	// Computation of next Cuboids
	double randAngle = rand() % 45 + 5; // random faktor
	double newAngle = oldAngle - randAngle; // random angle for main branch

	double newSidelength[3]; 
	getNewSidelength(points[3], points[2], randAngle, newSidelength);
	double* vector_b = toArray(points[4]); // { points[4].x(),points[4].y(),points[4].z() };
	Cuboid newCuboid_b = getNextCuboid(newSidelength[1], newAngle, vector_b);	//new cuboid for the main branch (with the 
	//site lenght of the b site of the triangle)

	Point* cuboid_bPoints = newCuboid_b.points; // getPoints(newCuboid_b);
	double* vector_a = toArray(cuboid_bPoints[1]);
	Cuboid newCuboid_a = getNextCuboid(newSidelength[1], newAngle + 90, vector_a);	//new cuboid for the site branch (with the 
	//site lenght of the a site of the triangle)
	generateSerialRecursive(newCuboid_b, renderer, newAngle);
	generateSerialRecursive(newCuboid_a, renderer, newAngle + 90);
}*/

// -------------------------------- Iteration ----------------------------
std::vector<Cuboid> generateSerialIterative(std::vector<Cuboid>& cuboids, int depth, Renderer& renderer) {
	std::vector<Cuboid> inputCuboids = std::move(cuboids);
	std::vector<Cuboid> resultCuboids;

	int limit = inputCuboids.size();

	for (int d = 0; d < depth; d++) {
		resultCuboids.resize(limit * 2);

		for (int i = 0; i < limit; i++) {
			const Cuboid& myCuboid = inputCuboids[i];
			const Point* points = myCuboid.points;

			// Definition of render object
			RenderObject cube;
			cube.verts.reserve(8);
			cube.edges.reserve(12);
			for (int j = 0; j < 8; j++) {
				cube.verts.push_back(toVector3D(points[j]));
			}
			for (int j = 0; j < 12; j++) {
				cube.edges.push_back({ myCuboid.xedges[j], myCuboid.yedges[j] });
			}

			// set color
			cube.color = chooseColour(myCuboid);

			renderer.addRenderObject(std::move(cube));

			// Computation of next Cuboids
			double newAngle;
			double newSidelength[3];

			if (randomAngle) {
				double randAngle = rand() % 45 + 5; // random angle
				newAngle = myCuboid.angle - randAngle;
				getNewSidelength(points[3], points[2], randAngle, newSidelength);
			}
			else {
				newAngle = myCuboid.angle - 30;
				getNewSidelength(points[3], points[2], 30, newSidelength);
			}

			double* vector_b = (double*)&points[4];
			Cuboid newCuboid_b = getNextCuboid(newSidelength[1], newAngle, vector_b); // new cuboid for the main branch
			newCuboid_b.angle = newAngle;

			Point* cuboid_bPoints = newCuboid_b.points;
			double* vector_a = (double*)&cuboid_bPoints[1];
			Cuboid newCuboid_a = getNextCuboid(newSidelength[0], newAngle + 90, vector_a); // new cuboid for the site branch
			newCuboid_a.angle = newAngle + 90;

			// Assign the new Cuboids to the result vector
			resultCuboids[i * 2 + 1] = newCuboid_b;
			resultCuboids[i * 2] = newCuboid_a;
		}

		inputCuboids = std::move(resultCuboids);
		limit = limit * 2;
		resultCuboids = std::vector<Cuboid>(); // clear and deallocate
	}
	return inputCuboids;
}

// ------------------------------------------------------------------------
// -------------------------------- OpenMP --------------------------------
/* Computes the next level of cuboids based on the current level in parallel.
 */
std::vector<Cuboid> generateWithOpenMP(std::vector<Cuboid>& cuboids, int depth, Renderer& renderer) {
	std::vector<Cuboid> inputCuboids = std::move(cuboids);
	std::vector<Cuboid> resultCuboids;

	int limit = inputCuboids.size();
	int const maxThreads = omp_get_max_threads();
	omp_set_num_threads(maxThreads);

	for (int d = 0; d < depth; d++) {
		resultCuboids.resize(limit * 2);
#pragma omp parallel for
		for (int i = 0; i < limit; i++) {
			const Cuboid& myCuboid = inputCuboids[i];
			addCuboidToRenderer(myCuboid, renderer);
			const Point* points = myCuboid.points;

			// Computation of next Cuboids
			double newAngle;
			double newSidelength[3];

			if (randomAngle) {
				double randAngle = rand() % 45 + 5; // random angle
				newAngle = myCuboid.angle - randAngle;
				getNewSidelength(points[3], points[2], randAngle, newSidelength);
			} else {
				newAngle = myCuboid.angle - 30;
				getNewSidelength(points[3], points[2], 30, newSidelength);
			}

			double* vector_b = (double*)&points[4];
			Cuboid newCuboid_b = getNextCuboid(newSidelength[1], newAngle, vector_b);	//new cuboid for the main branch (with the
																						//site lenght of the b site of the triangle)
			newCuboid_b.angle = newAngle;
			Point* cuboid_bPoints = newCuboid_b.points;
			double* vector_a = (double*)&cuboid_bPoints[1];
			Cuboid newCuboid_a = getNextCuboid(newSidelength[0], newAngle + 90, vector_a);	//new cuboid for the site branch (with the
																							//site lenght of the a site of the triangle)
			newCuboid_a.angle = newAngle + 90;

			resultCuboids[i*2+1] = newCuboid_b;
			resultCuboids[i*2] = newCuboid_a;
		}
		inputCuboids = std::move(resultCuboids);
		limit = limit * 2;
		resultCuboids = std::vector<Cuboid>(); // clear and deallocate
	}
	return inputCuboids;
}

// ------------------------------------------------------------------------
// -------------------------------- OpenCL --------------------------------
/* Computes the next level of cuboids based on the current level in parallel.
 * @param cuboids Current level of cuboids
 * @param renderer
 * @param depth
 * @param build_time
 * @return
 */
std::vector<Cuboid> generateWithOpenCL(std::vector<Cuboid>& cuboids, Renderer& renderer, unsigned const int depth, double &build_time) {
	if (depth == 0) {
		return std::move(cuboids);
	}

	// generate random numbers for the kernel
	int randoms[42];
	for (int i = 0; i < 42; i++) {
		randoms[i] = randomAngle? rand() + 1 : 30;
	}

	cl_platform_id platform = getPlatform(100, 0);
	cl_device_id device = getDevice(platform, 0);

	cl_uint num_devices = 1;
	cl_device_id devices[1] = { device };

	cl_context context = getContext(platform, devices, num_devices);
	if (context == nullptr) {
		return std::vector<Cuboid>();
	}

	// create Command Queue
	cl_int err = 0;
	cl_command_queue queue = clCreateCommandQueueWithProperties(context, device, NULL, &err);
	if (CL_SUCCESS != err) {
		std::cout << "Queue error code: " << err << std::endl;
		clReleaseContext(context);
		return std::vector<Cuboid>();
	}

	// load Kernel from file
	const char* kernelSource;
	const std::string kernelSource_str = loadKernelSource("kernel.cl");
	kernelSource = kernelSource_str.c_str();

	// create program
	cl_uint count = 1;
	cl_program program = clCreateProgramWithSource(context, count, &kernelSource, NULL, &err);
	if (CL_SUCCESS != err) {
		std::cout << "CreateProgram returned error: " << err << std::endl;
		clReleaseCommandQueue(queue);
		clReleaseContext(context);
		return std::vector<Cuboid>();
	}

	// compile program
	clock_t start, end;
	start = clock();
	err = clBuildProgram(program, num_devices, devices, NULL, NULL, NULL);
	end = clock();
	build_time = ((double)(end - start)) / CLOCKS_PER_SEC;

	if (CL_SUCCESS != err) {
		std::cout << "Build program returned error: " << err << std::endl;
		size_t logSize;
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize);
		std::vector<char> log(logSize);
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, logSize, log.data(), nullptr);
		std::cout << "Build info: " << log.data() << std::endl;
		clReleaseProgram(program);
		clReleaseCommandQueue(queue);
		clReleaseContext(context);
		return std::vector<Cuboid>();
	}

	// create Kernel for specific Device
	cl_kernel kernel = clCreateKernel(program, "buildTree", &err);
	if (CL_SUCCESS != err) {
		std::cout << "Create kernel error: " << err << std::endl;
		clReleaseProgram(program);
		clReleaseCommandQueue(queue);
		clReleaseContext(context);
		return std::vector<Cuboid>();
	}

	std::vector<Cuboid> input = std::move(cuboids);
	std::vector<Cuboid> result;

	// get buffer size for all iterations
	size_t maxInputSize = input.size();
	for (unsigned int d = 0; d < depth; d++) {
		maxInputSize *= 2;
	}

	// one time: create buffer with maximum size (Double-Buffering)
	cl_mem bufferA = clCreateBuffer(context, CL_MEM_READ_WRITE, maxInputSize * sizeof(Cuboid), NULL, &err);
	if (CL_SUCCESS != err) {
		std::cout << "Buffer A creation error: " << err << std::endl;
		clReleaseKernel(kernel);
		clReleaseProgram(program);
		clReleaseCommandQueue(queue);
		clReleaseContext(context);
		return std::vector<Cuboid>();
	}

	cl_mem bufferB = clCreateBuffer(context, CL_MEM_READ_WRITE, maxInputSize * sizeof(Cuboid), NULL, &err);
	if (CL_SUCCESS != err) {
		std::cout << "Buffer B creation error: " << err << std::endl;
		clReleaseMemObject(bufferA);
		clReleaseKernel(kernel);
		clReleaseProgram(program);
		clReleaseCommandQueue(queue);
		clReleaseContext(context);
		return std::vector<Cuboid>();
	}

	// pass random numbers to kernel
	cl_mem randomBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * 42, randoms, &err);
	if (CL_SUCCESS != err) {
		std::cout << "Random buffer creation error: " << err << std::endl;
		clReleaseMemObject(bufferA);
		clReleaseMemObject(bufferB);
		clReleaseKernel(kernel);
		clReleaseProgram(program);
		clReleaseCommandQueue(queue);
		clReleaseContext(context);
		return std::vector<Cuboid>();
	}

	// upload initial data
	cl_mem inputBuffer = bufferA;
	cl_mem outputBuffer = bufferB;

	err = clEnqueueWriteBuffer(queue, inputBuffer, CL_FALSE, 0, input.size() * sizeof(Cuboid), input.data(), 0, NULL, NULL);
	if (CL_SUCCESS != err) {
		std::cout << "Initial write buffer error: " << err << std::endl;
		clReleaseMemObject(bufferA);
		clReleaseMemObject(bufferB);
		clReleaseKernel(kernel);
		clReleaseProgram(program);
		clReleaseCommandQueue(queue);
		clReleaseContext(context);
		return std::vector<Cuboid>();
	}

	err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &randomBuffer);

	size_t currentSize = input.size();

	for (unsigned int local_depth = 0; local_depth < depth; local_depth++) {
		// add Render-Objects hinzufügen (if CL_FALSE: parallel to GPU-Upload)
#pragma omp parallel for
#ifdef _WIN32
		for (int i = 0; i < input.size(); i++) {
			addCuboidToRenderer(input[i], renderer);
		}
#else
		for (const auto& c : input) {
			addCuboidToRenderer(c, renderer);
		}
#endif

		// set Kernel arguments
		err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &outputBuffer);
		err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &inputBuffer);
		if (CL_SUCCESS != err) {
			std::cout << "Set kernel argument error: " << err << std::endl;
			break;
		}

		// run Kernel - NULL for local_work_size let OpenCL choose the optimal size
		size_t globalSize = currentSize;
		err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, NULL, 0, NULL, NULL);
		if (CL_SUCCESS != err) {
			std::cout << "Run error: " << err << std::endl;
			break;
		}

		// swap Buffer (Double-Buffering)
		std::swap(inputBuffer, outputBuffer);
		currentSize *= 2;

		// read results only in last iteration or for next CPU iteration
		if (local_depth == depth - 1) {
			result.resize(currentSize);
			clEnqueueReadBuffer(queue, inputBuffer, CL_TRUE, 0, currentSize * sizeof(Cuboid), result.data(), 0, NULL, NULL);
		} else {
			// refresh input-vector for next iteration
			input.resize(currentSize);
			clEnqueueReadBuffer(queue, inputBuffer, CL_TRUE, 0, currentSize * sizeof(Cuboid), input.data(), 0, NULL, NULL);
		}
	}

	// free all OpenCL kernel resources
	clReleaseMemObject(bufferA);
	clReleaseMemObject(bufferB);
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);

	return result.empty() ? std::move(input) : std::move(result);
}

// ----------------------------------------------------------------------
// -------------------------------- main --------------------------------
int main(int argc, char* argv[]) {
	bool show = true;
	int serial_depth = 2;
	int open_mp_depth = 2;
	int open_cl_depth = 2;

	// Parse command line arguments TODO: error handling for invalid values
	// Check for argument name in the argv array, 
	// continue if there is one more argument after the name and then parse the numeric / boolean value.
	// CMD-Line arguments can be out of order in this implementation. 
	for (int i = 1; i < argc; i++) {
		std::string arg = argv[i];
		if (arg == "--show" && i + 1 < argc) {
			show = (std::string(argv[++i]) == "true");
		} else if (arg == "--serial-depth" && i + 1 < argc) {
			serial_depth = std::atoi(argv[++i]);
		} else if (arg == "--openmp-depth" && i + 1 < argc) {
			open_mp_depth = std::atoi(argv[++i]);
		} else if (arg == "--opencl-depth" && i + 1 < argc) {
			open_cl_depth = std::atoi(argv[++i]);
		} else if (arg == "--random-size" && i + 1 < argc) {
			randomSize = (std::string(argv[++i]) == "true");
		} else if (arg == "--random-angle") {
			randomAngle = true;
		}
	}
	if (randomAngle || randomSize) {
		std::srand(std::time(nullptr));
	}

	std::cout << "-----------------------------------" << std::endl;
	std::cout << "Settings:" << std::endl;
	std::cout << " Show: " << (show ? "true" : "false") << std::endl;
	std::cout << " Random Tree: " << (randomSize ? "true" : "false") << std::endl;
	std::cout << " Serial Depth: " << serial_depth << std::endl;
	std::cout << " OpenMP Depth: " << open_mp_depth << std::endl;
	std::cout << " OpenCL Depth: " << open_cl_depth << std::endl;

	// Speicher-Warnung bei großen Tiefen
	int total_depth = serial_depth + open_mp_depth + open_cl_depth;
	size_t estimated_cuboids = (size_t)1 << total_depth; // 2^total_depth
	size_t estimated_mb = (estimated_cuboids * (sizeof(Cuboid) + sizeof(RenderObject) + 8*sizeof(Vector3D) + 12*sizeof(std::pair<int,int>))) / (1024*1024);
	std::cout << " Estimated cuboids: " << estimated_cuboids << std::endl;
	std::cout << " Estimated memory: ~" << estimated_mb << " MB" << std::endl;
	if (estimated_mb > 4000) {
		std::cerr << "WARNING: Estimated memory usage exceeds 4 GB! Consider reducing depth." << std::endl;
	}
	std::cout << "-----------------------------------" << std::endl;

	Renderer myRenderer = Renderer();

	std::vector<Cuboid> cuboids = {createCuboidFromSidelength(STARTSIZE, STARTSIZE, STARTSIZE)};
	
#ifdef _WIN32
	clock_t start, end;
#else
	struct timeval start, end;
#endif

	if (serial_depth > 0) {
#ifdef _WIN32
		start = clock();
#else
		gettimeofday(&start, NULL);
#endif
		cuboids = generateSerialIterative(cuboids, serial_depth, myRenderer);
#ifdef _WIN32
		end = clock();
		double exec_t_serial_iterative = ((double)(end - start)) / CLOCKS_PER_SEC;
#else
		gettimeofday(&end, NULL);
		double exec_t_serial_iterative = 	(end.tv_sec - start.tv_sec) + 
              								(end.tv_usec - start.tv_usec) / 1000000.0;
#endif
		std::cerr << "SERD:" << serial_depth << ":T:" << exec_t_serial_iterative << "" << std::endl;
	}
	// Parallel Execution - OpenMP
	if (open_mp_depth > 0) {
#ifdef _WIN32
		start = clock();
#else
		gettimeofday(&start, NULL);
#endif
		cuboids = generateWithOpenMP(cuboids, open_mp_depth, myRenderer);
#ifdef _WIN32
		end = clock();
		double exec_t_openMP = ((double)(end - start)) / CLOCKS_PER_SEC;
#else
		gettimeofday(&end, NULL);
		double exec_t_openMP = 	(end.tv_sec - start.tv_sec) + 
              					(end.tv_usec - start.tv_usec) / 1000000.0;
#endif
		std::cerr << "OMPD:" << open_mp_depth << ":T:" << exec_t_openMP << "" << std::endl;
	}
	// Parallel Execution - OpenCL
	if (open_cl_depth > 0) {
		double build_time = 0;
#ifdef _WIN32
		start = clock();
#else
		gettimeofday(&start, NULL);
#endif
		cuboids = generateWithOpenCL(cuboids, myRenderer, open_cl_depth, build_time);
#ifdef _WIN32
		end = clock();
		double exec_t_openCL = ((double)(end - start)) / CLOCKS_PER_SEC;
#else
		gettimeofday(&end, NULL);
		double exec_t_openCL = 	(end.tv_sec - start.tv_sec) + 
              					(end.tv_usec - start.tv_usec) / 1000000.0;
#endif
		std::cerr << "OCLD:" << open_cl_depth << ":T:" << exec_t_openCL << "" << std::endl;
		std::cerr << "BOCT:" << build_time << "" << std::endl;
	}

#pragma omp parallel for
#ifdef _WIN32
	for (int i = 0; i < cuboids.size(); i++) {
		addCuboidToRenderer(cuboids[i], myRenderer);
	}
#else
	for (Cuboid& c : cuboids) {
		addCuboidToRenderer(c, myRenderer);
	}
#endif
	// Serial execution (recursive)
	// generateSerialRecursive(cuboids[0], myRenderer, 0); //building the tree recursive

    if (show) {
		myRenderer.run();
	}

    return 0;
}
