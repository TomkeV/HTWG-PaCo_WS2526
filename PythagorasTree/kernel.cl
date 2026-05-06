#define matrixSize 4
#define STARTSIZE 2

// does not work properly
/*int randomHeight = 0; // 0 for false
int randomAngle = 0;*/

// ----------------------------------------------------------------------------------
// -------------------------------- Structs --------------------------------
// Struct Matrix. Defines a matrix with 16 values. 
struct Matrix {
	int rows;
	int cols;
	double matrix[16];
};

// Struct Point. Defines a point in a 3D coordinate system in form P = (x|y|z).
struct Point {
	double x;
	double y;
	double z;
};

// Struct Cuboid. Defines a cuboid by its eight corners. 
struct Cuboid {
	struct Point points[8];
	int xedges[12];
	int yedges[12];
	double angle;
};

// ----------------------------------------------------------------------------------
// -------------------------------- Functions --------------------------------
/* Computation of random numbers.
 * @return random number as int
 */
int rand() {
	return 42;
}

/* Computes the absolute value of a number
 * @param value Value to get absolute value from
 * @return value > 0 ? value : value * (-1)
 */
double absoluteValue(double value){
	if(value > 0){
		return value;
	}
	return value * -1;
}

/* Computation of the scalar product out of two vectors.
 * @param v1, v2 Vectors to get the scalar product from
 * @return v1(1) * v2(1) + v1(2) * v2(2) + ... as double
 */
double computeScalarproduct(double* v1, double* v2) {
	double res = 0;
	for (int i = 0; i < matrixSize; ++i) {
		res += v1[i] * v2[i];
	}
	return res;
}

/*
 */
void getNewSidelength(struct Point corner_a, struct Point corner_b, double alpha, double* newSidelength){
double gamma = 90.0; // gamma is always the right angle.
	double beta = 180.0 - absoluteValue(alpha) - gamma;

	// compute the sides
	newSidelength[2] = sqrt(pow((corner_b.x - corner_a.x), 2) + pow((corner_b.y - corner_a.y), 2) + pow((corner_b.z - corner_a.z), 2)); // |AB|
	newSidelength[0] = (newSidelength[2] / native_sin(gamma / 180 * M_PI)) * native_sin(alpha / 180 * M_PI); // law of sine
	newSidelength[1] = (newSidelength[2] / native_sin(gamma / 180 * M_PI)) * native_sin(beta / 180 * M_PI); // law of sine
}
// -------------------------------- Matrix functions --------------------------------
/* Getter for any element of a matrix. 
 * @param matrix Matrix to get the element from
 * @param r, c Index of the element
 * @return element of the matrix as double
 */
const double getMatrElem(struct Matrix matrix, int r, int c) {
	return matrix.matrix[r * matrix.cols + c];
}

/* Access to any row of a matrix.
 * @param m Matrix to get the row from
 * @param r Number of row to get
 * @param row Output param to return the wanted row
 * @return wanted row as double*
 */
double* getMatrRow(struct Matrix m, int r, double *row) {
	for (int i = 0; i < m.cols; ++i) {
		row[i] = getMatrElem(m, r, i);
	}
	return row;
}

/* Matrix multiplication.
 * Multiplicates two matrices.
 * @param m1, m2 matrices to be multiplicated
 * @return new matrix = m1 * m2 or empty matrix with rows = cols = -1 if dimensions are wrong 
 */
struct Matrix multiplicate(const struct Matrix m1, const struct Matrix m2) {
	// Check if dimensions fit
	if (m1.cols != m2.rows) {
		struct Matrix res_matrix;
		res_matrix.rows = -1;
		res_matrix.cols = -1;
		return res_matrix;
	}

	// switch rows and cols of second matrix for easier access while multiplication
	double m2_as_cols_values[16];
	for (int col = 0; col < m2.cols; ++col) {
		for (int row = 0; row < m2.rows; ++row) {
			double value = getMatrElem(m2, row, col);
			m2_as_cols_values[col * m2.rows + row] = value;
		}
	}
	// create transformed matrix from m2_as_cols_values
	struct Matrix m2_as_cols = { m2.cols, m2.rows };
	m2_as_cols.matrix[0] = m2_as_cols_values[0];
	m2_as_cols.matrix[1] = m2_as_cols_values[1];
	m2_as_cols.matrix[2] = m2_as_cols_values[2];
	m2_as_cols.matrix[3] = m2_as_cols_values[3];

	// multiplicate
	double res_matrix_values[4]; 
	for (int i = 0; i < m1.rows; ++i) {
		for (int j = 0; j < m2_as_cols.rows; ++j) {
			double row1[4];
			double row2[4];

			getMatrRow(m1, i, row1);
			getMatrRow(m2_as_cols, j, row2);

			double scalar_result = computeScalarproduct(row1, row2);
			res_matrix_values[i * m2.cols + j] = scalar_result;
		}
	}

	// create multiplicated matrix from computed rows
	struct Matrix res_matrix = { m1.rows, m2.cols};
	res_matrix.matrix[0] = res_matrix_values[0];
	res_matrix.matrix[1] = res_matrix_values[1];
	res_matrix.matrix[2] = res_matrix_values[2];
	res_matrix.matrix[3] = res_matrix_values[3];

	return res_matrix;
}

// -------------------------------- Point functions --------------------------------
/* Transforms a Point in 3D space by a given angle, axis and vector.
 * Angle and axis are needed for rotation, vector delays point.
 * @param point Point to be transformed
 * @param alpha Angle for rotation
 * @param axis Axis for rotation
 * @param v Vector for delaying the point
 * @return transformed point
 */
struct Point transform3D(struct Point point, double alpha, char axis, double* v) {
	// Convert 3D point in homogenous matrix 
	struct Matrix pointHomogenous = { 4, 1};
	pointHomogenous.matrix[0] = point.x;
	pointHomogenous.matrix[1] = point.y;
	pointHomogenous.matrix[2] = point.z;
	pointHomogenous.matrix[3] = 1;

	// select correct rotational matrix belongig to axis and angle delaying by v
	struct Matrix rotation = {4,4};
	
	float sin_value = native_sin(alpha / 180 * M_PI);
	float cos_value = native_cos(alpha / 180 * M_PI);
	if (axis == 'x') { // x-axis
		rotation.matrix[0] = 1;  rotation.matrix[1] = 0;         rotation.matrix[2] = 0;          rotation.matrix[3] = v[0];
		rotation.matrix[4] = 0;  rotation.matrix[5] = cos_value; rotation.matrix[6] = -sin_value; rotation.matrix[7] = v[1];
		rotation.matrix[8] = 0;  rotation.matrix[9] = sin_value; rotation.matrix[10] = cos_value; rotation.matrix[11] = v[2];
		rotation.matrix[12] = 0; rotation.matrix[13] = 0;        rotation.matrix[14] = 0;         rotation.matrix[15] = 1; 
	} else if (axis == 'y') { // y-axis
		rotation.matrix[0] = cos_value;  rotation.matrix[1] = 0;  rotation.matrix[2] = sin_value;  rotation.matrix[3] = v[0];
		rotation.matrix[4] = 0;          rotation.matrix[5] = 1;  rotation.matrix[6] = 0;          rotation.matrix[7] = v[1];
		rotation.matrix[8] = -sin_value; rotation.matrix[9] = 0;  rotation.matrix[10] = cos_value; rotation.matrix[11] = v[2];
		rotation.matrix[12] = 0;         rotation.matrix[13] = 0; rotation.matrix[14] = 0;         rotation.matrix[15] = 1; 
	} else { // z-axis
		rotation.matrix[0] = cos_value; rotation.matrix[1] = -sin_value; rotation.matrix[2] = 0;  rotation.matrix[3] = v[0];
		rotation.matrix[4] = sin_value; rotation.matrix[5] = cos_value;  rotation.matrix[6] = 0;  rotation.matrix[7] = v[1];
		rotation.matrix[8] = 0;         rotation.matrix[9] = 0;          rotation.matrix[10] = 1; rotation.matrix[11] = v[2];
		rotation.matrix[12] = 0;        rotation.matrix[13] = 0;         rotation.matrix[14] = 0; rotation.matrix[15] = 1; 
	}

	// rotate point
	struct Matrix res = multiplicate(rotation, pointHomogenous);

	// convert result back into point instead of matrix
	struct Point res_point = { getMatrElem(res,0,0), getMatrElem(res,1,0), getMatrElem(res,2,0)};
	
	return res_point;
}

// -------------------------------- Cuboid functions --------------------------------
/* Access to the Points that define a Cuboid.
 * @return all Points of the cuboid as Point*
 */
struct Point* getPoints(struct Cuboid *cuboid){
    return (cuboid->points);
}

/* "Constructor". 
 * Creates a new cuboid from the given sidelength height, width and depth.
 */
struct Cuboid createCuboidFromSidelength(double height, double width, double depth) {
    struct Cuboid new_cuboid;
	struct Point one = { 0, 0, 0 };
	new_cuboid.points[0] = one;
	struct Point two = { width, 0, 0 };
	new_cuboid.points[1] = two;
	struct Point three = { width, depth, 0 };
	new_cuboid.points[2] = three;
	struct Point four = { 0, depth, 0 };
	new_cuboid.points[3] = four;
	struct Point five = { 0, 0, height };
	new_cuboid.points[4] = five;
	struct Point six = { width, 0, height };
	new_cuboid.points[5] = six;
	struct Point seven = { width, depth, height };
	new_cuboid.points[6] = seven;
	struct Point eight = { 0, depth, height };
	new_cuboid.points[7] = eight;

	// set xedges
	new_cuboid.xedges[0] = 0;
	new_cuboid.xedges[1] = 1;
	new_cuboid.xedges[2] = 2;
	new_cuboid.xedges[3] = 3;
	new_cuboid.xedges[4] = 4;
	new_cuboid.xedges[5] = 5;
	new_cuboid.xedges[6] = 6;
	new_cuboid.xedges[7] = 7;
	new_cuboid.xedges[8] = 0;
	new_cuboid.xedges[9] = 1;
	new_cuboid.xedges[10] = 2;
	new_cuboid.xedges[11] = 3;

	// set yedges
	new_cuboid.yedges[0] = 1;
	new_cuboid.yedges[1] = 2;
	new_cuboid.yedges[2] = 3;
	new_cuboid.yedges[3] = 0;
	new_cuboid.yedges[4] = 5;
	new_cuboid.yedges[5] = 6;
	new_cuboid.yedges[6] = 7;
	new_cuboid.yedges[7] = 4;
	new_cuboid.yedges[8] = 4;
	new_cuboid.yedges[9] = 5;
	new_cuboid.yedges[10] = 6;
	new_cuboid.yedges[11] = 7;

    return new_cuboid;
}

/* Turns the cuboid by the given angle around the given axis 
 * and moves it from (0|0|0) to the place the vector shows
 * @returns new cuboid
 */
struct Cuboid transform(struct Cuboid cuboid, double alpha, char axis, double* v) {
    struct Cuboid new_cuboid;
    struct Point* old_points = getPoints(&cuboid);

    for (int i = 0; i < 8; i++) {
        struct Point current = old_points[i];
        new_cuboid.points[i] = transform3D(current,alpha, axis, v); // transforms each point to its new position
    }

	// set xedges
	new_cuboid.xedges[0] = 0;
	new_cuboid.xedges[1] = 1;
	new_cuboid.xedges[2] = 2;
	new_cuboid.xedges[3] = 3;
	new_cuboid.xedges[4] = 4;
	new_cuboid.xedges[5] = 5;
	new_cuboid.xedges[6] = 6;
	new_cuboid.xedges[7] = 7;
	new_cuboid.xedges[8] = 0;
	new_cuboid.xedges[9] = 1;
	new_cuboid.xedges[10] = 2;
	new_cuboid.xedges[11] = 3;

	// set yedges
	new_cuboid.yedges[0] = 1;
	new_cuboid.yedges[1] = 2;
	new_cuboid.yedges[2] = 3;
	new_cuboid.yedges[3] = 0;
	new_cuboid.yedges[4] = 5;
	new_cuboid.yedges[5] = 6;
	new_cuboid.yedges[6] = 7;
	new_cuboid.yedges[7] = 4;
	new_cuboid.yedges[8] = 4;
	new_cuboid.yedges[9] = 5;
	new_cuboid.yedges[10] = 6;
	new_cuboid.yedges[11] = 7;
    return new_cuboid;
}

/*
 */
struct Cuboid getNextCuboid(double length, double angle, double* vector){
	double height;
	// randomization not possible yet
	/*if (randomHeight > 0) {
		height = (rand() % 100 * (2*length)) / 100; // random height
	} else {*/
		height = length;
	//}
	struct Cuboid newCuboid = createCuboidFromSidelength(height, length, STARTSIZE);
    return transform(newCuboid, angle, 'y', vector);
}

// ----------------------------------------------------------------------------------
// -------------------------------- kernel functions --------------------------------
__kernel
void buildTree(__global struct Cuboid* result, __global struct Cuboid* cuboid, __global int* rand_numbers) {
	// Computation of next Cuboids
	int id = get_global_id(0);
	struct Cuboid myCuboid = cuboid[id];
	struct Point * points = getPoints(&myCuboid);

	double newAngle;
	double newSidelength[3];
	
	// Random buffer is 42 - and contains no random number if it should not be random :D
	float randAngle = rand_numbers[id % 42] % 45; // random angle
	newAngle = myCuboid.angle - randAngle;
	getNewSidelength(points[3], points[2], randAngle, newSidelength);

	double* vector_b = (double*) &points[4]; 
	struct Cuboid newCuboid_b = getNextCuboid(newSidelength[1], newAngle, vector_b);	//new cuboid for the main branch (with the 
																						//site lenght of the b site of the triangle)
	newCuboid_b.angle = newAngle;

	struct Point * cuboid_bPoints = getPoints(&newCuboid_b); 
	double* vector_a = (double*) &cuboid_bPoints[1]; 
	struct Cuboid newCuboid_a = getNextCuboid(newSidelength[0], newAngle + 90, vector_a); //new cuboid for the site branch (with the 
																							//site lenght of the a site of the triangle)
	newCuboid_a.angle = newAngle + 90;
	
	result[id * 2] = newCuboid_b;
	result[id * 2 + 1] = newCuboid_a;
}