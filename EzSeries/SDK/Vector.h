#pragma once

#ifndef M_PI
#define M_PI 3.14159265359f
#endif

struct ProjectionInfo;
struct IntersectionResult;

struct Vector4
{
	float     x, y, z, w;
	Vector4() { x = y = z = w = 0.0f; }
	Vector4(float _x, float _y, float _z, float _w) { x = _x; y = _y; z = _z; w = _w; }
};

struct Vector2
{
	float     x, y;
	Vector2() { x = y = 0.0f; }
	Vector2(float _x, float _y) { x = _x; y = _y; }
	float operator[] (size_t i) const { return (&x)[i]; }
};

class Vector
{
public:
	float x, z, y;

	Vector();
	Vector(float x, float y, float z = 0);
	Vector(float xyz);

	~Vector();

	bool IsValid() const;

	bool operator==(Vector const& other) const;
	bool operator!=(Vector const& other) const;
	bool IsZero(float tolerance = 0.01f) const;

	Vector& operator*=(const Vector& v);
	Vector& operator*=(float s);

	Vector& operator/=(const Vector& v);
	Vector& operator/=(float s);

	Vector& operator+=(const Vector& v);
	Vector& operator+=(float fl);

	Vector& operator-=(const Vector& v);
	Vector& operator-=(float fl);

	Vector operator-(const Vector& v) const;
	Vector operator-(float mod) const;
	Vector operator+(const Vector& v) const;
	Vector operator+(float mod) const;

	Vector operator/(const Vector& v) const;
	Vector operator/(float mod) const;
	Vector operator*(const Vector& v) const;
	Vector operator*(float mod) const;

	Vector& operator=(const Vector& v);

	Vector& SwitchYZ();
	Vector& Negate();

	float Length() const;
	float LengthSquared() const;

	Vector Normalized() const;
	float NormalizeInPlace() const;

	float Distance(Vector const& to) const;
	float Distance(Vector const& segment_start, Vector const& segment_end, bool only_if_on_segment = false, bool squared = false) const;
	float DistanceSquared(Vector const& to) const;

	ProjectionInfo ProjectOn(Vector const& segment_start, Vector const& segment_end) const;
	IntersectionResult Intersection(Vector const& line_segment_end, Vector const& line_segment2_start, Vector const& line_segment2_end) const;

	float DotProduct(Vector const& other) const;
	float CrossProduct(Vector const& other) const;
	float Polar() const;
	float AngleBetween(Vector const& other) const;

	bool Close(float a, float b, float eps) const;

	Vector Rotated(float angle) const;
	Vector Perpendicular() const;
	Vector Extend(Vector const& to, float distance) const;

	bool IsWall() const;
	bool IsWallOfGrass() const;
	bool IsBuilding() const;
	bool IsOnScreen() const;
	bool IsUnderAllyTurret() const;
	bool IsUnderEnemyTurret() const;

	Vector2 WorldToScreen() const;
	Vector& To3DWorld();
	Vector& To3DPlayer();

	int CountAlliesInRange(float range) const;
	int CountEnemiesInRange(float range) const;
};

struct ProjectionInfo
{
	bool IsOnSegment;
	Vector LinePoint;
	Vector SegmentPoint;

	ProjectionInfo(bool is_on_segment, Vector const& segment_point, Vector const& line_point);
};

struct IntersectionResult
{
	bool Intersects;
	Vector Point;

	IntersectionResult(bool intersects = false, Vector const& point = Vector());
};
