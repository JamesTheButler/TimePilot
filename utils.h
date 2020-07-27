static class Utils {
public:
	//angle in degree towards angle in radians
	static double degToRad(double angle){
		return (double)(M_PI * angle) / 180.;
	}

	//angle in radians to angle in degree
	static double radToDeg(double angle) {
		return (double) (angle*180.) / (double)M_PI;
	}

	//returns the distance between to points specified by (x1,y1) and (x2,y2)
	static double getDistance(double x1, double y1, double x2, double y2) {
		return sqrt(pow(abs(x1 - x2), 2) + pow(abs(y1 - y2), 2));
	}
	
	//vector functions
	//vec a + vec b
	static Vec2 addVectors(Vec2 a, Vec2 b) {
		return Vec2(a.x + b.x, a.y + b.y);
	}
	//vec a - vec b
	static Vec2 subVectors(Vec2 a, Vec2 b) {
		return Vec2(a.x - b.x, a.y - b.y);
	}
	//vec a * scalar
	static Vec2 multVectorScalar(Vec2 a, float s) {
		return Vec2(a.x * s, a.y * s);
	}
	//vec a / scalar
	static Vec2 divVectorScalar(Vec2 a, float s) {
		return Vec2(a.x / s, a.y / s);
	}

	//distace between two position vectors
	static double getDistance(Vec2 a, Vec2 b) {
		return getDistance(a.x, a.y, b.x, b.y);
	}
};