#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <tuple>
#include <assert.h>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <map>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "matrix.h"
#include "EasyBMP.h"
#include "Environment.h"
#include "Timer.h"

#define PI 3.1415926535897932384626433832795028841971
#define MAX_MARCH_STEPS 2000
#define RECURSIVE_STEPS 3

#define EPS 1e-7f

typedef Matrix<std::tuple<uint, uint, uint>> Image;
typedef std::tuple<uint, uint, uint> tuple3uint;


using glm::vec3;
using glm::vec2;
using glm::max;
using glm::min;

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::tuple;
using std::numeric_limits;