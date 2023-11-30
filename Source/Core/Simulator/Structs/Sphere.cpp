#include <Simulator/Structs/Sphere.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Shapes {


BoundingBox Sphere::GetBoundingBox() {
	BoundingBox bb;

	bb.bb_point1[0] = Center_nm[0] - Radius_nm;
	bb.bb_point1[1] = Center_nm[1] - Radius_nm;
	bb.bb_point1[2] = Center_nm[2] - Radius_nm;

	bb.bb_point1[0] = Center_nm[0] + Radius_nm;
	bb.bb_point1[1] = Center_nm[1] + Radius_nm;
	bb.bb_point1[2] = Center_nm[2] + Radius_nm;

	return bb;
}

}; // Close Namespace Shapes
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
