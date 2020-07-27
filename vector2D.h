#pragma once
class Vec2 {
public:
	double x, y;

	Vec2() {
		Vec2(x, y);
	}

	Vec2(double x, double y) {
		this->x = x;
		this->y = y;
	}

	void vec2Add(Vec2 a) {
		this->x = a.x;
		this->y = a.y;
	}

	void print() {
		printf("(x: %f, y:%f)", x, y);
	}

	void add(Vec2 b) {
		x += b.x;
		y += b.y;
	}

	void sub(Vec2 b) {
		x -= b.x;
		y -= b.y;
	}

	void mult(double s) {
		x *= s;
		y *= s;
	}

	void div(double s) {
		x /= s;
		y /= s;
	}

	// normalize vector
	void normalize() {
		this->div(this->length());
	}

	//length of this
	double length() {
		return (double)(sqrt(pow(x, 2) + pow(y, 2)));
	}
};