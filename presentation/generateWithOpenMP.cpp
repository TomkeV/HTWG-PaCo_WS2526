std::vector<Cuboid> generateWithOpenMP(std::vector<Cuboid> cuboids, int depth, Renderer& renderer) {
	// exit condition
	/*double siteLength = getSidelength(cuboid)[1];
	if (siteLength < 0.05) {
		return;
	}*/
#define SIZE 15
	int size = 0;
	for (int i = 0; i < SIZE + 1; i++) {
		size += pow(2, i);
	}

	std::vector<Cuboid> inputCuboids = cuboids;
	std::vector<Cuboid> resultCuboids;
	//inputCuboids.assign(cuboids.size(), cuboids);
	int limit = cuboids.size();

	// set number of threads
	int const maxThreads = omp_get_max_threads();
	omp_set_num_threads(maxThreads);

	for (int d = 0; d < depth; d++) {
		resultCuboids.reserve(pow(2, d + 1));

#pragma omp parallel for
		for (int i = 0; i < limit; i++) {
			// Define render object
			Cuboid myCuboid = inputCuboids[i];
			Point* points = myCuboid.points;

#pragma omp critical 
			{
				RenderObject cube;
				for (int j = 0; j < 8; j++) { // define verts of render object
					cube.verts.push_back(toVector3D(points[j]));
				}
				for (int j = 0; j < 12; j++) { // define edges of render object
					std::pair<int, int> edge = { myCuboid.xedges[j], myCuboid.yedges[j] };
					cube.edges.push_back(edge);
				}
				cube.color = chooseColour(myCuboid);
				renderer.addRenderObject(cube);
			}
			// Computation of next Cuboids
			double newAngle = myCuboid.angle - 30; // new angle
			double newSidelength[3];
			getNewSidelength(points[3], points[2], 30, newSidelength);

			double* vector_b = (double*)&points[4];
			Cuboid newCuboid_b = getNextCuboid(newSidelength[1], newAngle, vector_b);
			newCuboid_b.angle = newAngle;
			Point* cuboid_bPoints = newCuboid_b.points;

			double* vector_a = (double*)&cuboid_bPoints[1];
			Cuboid newCuboid_a = getNextCuboid(newSidelength[0], newAngle + 90, vector_a);
			newCuboid_a.angle = newAngle + 90;
#pragma omp critical
			{
				resultCuboids.push_back(newCuboid_b);
				resultCuboids.push_back(newCuboid_a);
			}
			//inputCuboids[i * 2 + depth * 2] = newCuboid_b;
			//inputCuboids[i * 2 + depth * 2 + 1] = newCuboid_a;	
		}
		inputCuboids = resultCuboids;
		limit = limit * 2;
		resultCuboids.clear();
	}
	return inputCuboids;
}
