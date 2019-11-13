#include "Vector.h"
#include "PluginSDK.h"

Vector::Vector()
{
	this->x = 0; this->y = 0; this->z = 0;
}

Vector::Vector(float x, float y, float z)
{
	this->x = x; this->y = y; this->z = z;
}

Vector::Vector(float xyz)
{
	this->x = this->y = this->z = xyz;
}

Vector::~Vector() = default;

bool Vector::IsValid() const
{
	return this->x != 0 && this->y != 0;
}

bool Vector::IsZero(float tolerance) const
{
	return this->x > -tolerance && this->x < tolerance &&
		this->y > -tolerance && this->y < tolerance;
}

bool Vector::operator==(Vector const& other) const
{
	return other.x == this->x && other.y == this->y;
}

bool Vector::operator!=(Vector const& other) const
{
	return other.x == this->x && other.y == this->y;
}

Vector& Vector::operator*=(const Vector& v)
{
	this->x *= v.x;
	this->y *= v.y;
	return *this;
}

Vector& Vector::operator*=(float s)
{
	this->x *= s;
	this->y *= s;
	return *this;
}

Vector& Vector::operator/=(const Vector& v)
{
	this->x /= v.x;
	this->y /= v.y;
	return *this;
}

Vector& Vector::operator/=(float s)
{
	this->x /= s;
	this->y /= s;
	return *this;
}

Vector& Vector::operator+=(const Vector& v)
{
	this->x += v.x;
	this->y += v.y;
	return *this;
}

Vector& Vector::operator+=(float fl)
{
	this->x += fl;
	this->y += fl;
	return *this;
}

Vector& Vector::operator-=(const Vector& v)
{
	this->x -= v.x;
	this->y -= v.y;
	return *this;
}

Vector& Vector::operator-=(float fl)
{
	this->x -= fl;
	this->y -= fl;
	return *this;
}

Vector Vector::operator-(const Vector& v) const
{
	Vector result(this->x - v.x, this->y - v.y);
	return result;
}

Vector Vector::operator-(float mod) const
{
	Vector result(this->x - mod, this->y - mod);
	return result;
}

Vector Vector::operator+(const Vector& v) const
{
	Vector result(this->x + v.x, this->y + v.y);
	return result;
}

Vector Vector::operator+(float mod) const
{
	Vector result(this->x + mod, this->y + mod);
	return result;
}

Vector Vector::operator/(const Vector& v) const
{
	Vector result(this->x / v.x, this->y / v.y);
	return result;
}

Vector Vector::operator/(float mod) const
{
	Vector result(this->x / mod, this->y / mod);
	return result;
}

Vector Vector::operator*(const Vector& v) const
{
	Vector result(this->x * v.x, this->y * v.y);
	return result;
}

Vector Vector::operator*(float mod) const
{
	Vector result(this->x * mod, this->y * mod);
	return result;
}

Vector& Vector::operator=(const Vector& v) = default;

Vector& Vector::SwitchYZ()
{
	const auto temp = this->y;

	this->y = this->z;
	this->z = temp;
	return *this;
}

Vector& Vector::Negate()
{
	this->x = -this->x;
	this->y = -this->y;
	this->z = -this->z;

	return *this;
}

float Vector::Length() const
{
	return sqrtf(this->x * this->x + this->y * this->y);
}

float Vector::LengthSquared() const
{
	return this->x * this->x + this->y * this->y;
}

Vector Vector::Normalized() const
{
	auto const length = this->Length();
	if (length != 0)
	{
		auto const inv = 1.0f / length;
		return { this->x * inv, this->y * inv, this->z };
	}

	return *this;
}

float Vector::NormalizeInPlace() const
{
	auto v = *this;
	auto const l = this->Length();

	if (l != 0.0f)
	{
		v /= l;
	}
	else
	{
		v.x = v.y = 0.0f; v.z = 1.0f;
	}
	return l;
}

float Vector::Distance(Vector const& to) const
{
	Vector delta;

	delta.x = x - to.x;
	delta.y = y - to.y;

	return delta.Length();
}

float Vector::Distance(Vector const& segment_start, Vector const& segment_end, bool only_if_on_segment,
	bool squared) const
{
	auto const projection_info = this->ProjectOn(segment_start, segment_end);

	if (projection_info.IsOnSegment || !only_if_on_segment)
	{
		return squared
			? this->DistanceSquared(projection_info.SegmentPoint)
			: this->Distance(projection_info.SegmentPoint);
	}
	return FLT_MAX;
}

float Vector::DistanceSquared(Vector const& to) const
{
	Vector delta;

	delta.x = x - to.x;
	delta.y = y - to.y;

	return delta.LengthSquared();
}

ProjectionInfo Vector::ProjectOn(Vector const& segment_start, Vector const& segment_end) const
{
	float rs;
	auto const cx = x;
	auto const cy = y;
	auto const ax = segment_start.x;
	auto const ay = segment_start.y;
	auto const bx = segment_end.x;
	auto const by = segment_end.y;

	const auto rl = ((cx - ax) * (bx - ax) + (cy - ay) * (by - ay)) / (pow(bx - ax, 2) + pow(by - ay, 2));
	const auto point_line = Vector(ax + rl * (bx - ax), ay + rl * (by - ay), 0);

	if (rl < 0)
	{
		rs = 0;
	}
	else if (rl > 1)
	{
		rs = 1;
	}
	else
	{
		rs = rl;
	}

	auto const is_on_segment = rs == rl;
	auto const point_segment = is_on_segment ? point_line : Vector(ax + rs * (bx - ax), ay + rs * (by - ay), 0);

	return ProjectionInfo(is_on_segment, point_segment, point_line);
}

IntersectionResult Vector::Intersection(Vector const& line_segment_end, Vector const& line_segment2_start,
	Vector const& line_segment2_end) const
{
	const Vector side1 = { line_segment_end.x - this->x, line_segment_end.y - this->y };
	const Vector side2 = { line_segment2_end.x - line_segment2_start.x, line_segment2_end.y - line_segment2_start.y };

	const auto s = (-side1.y * (this->x - line_segment2_start.x) + side1.x * (this->y - line_segment2_start.y)) / (-side2.x * side1.y + side1.x * side2.y);
	const auto t = (side2.x * (this->y - line_segment2_start.y) - side2.y * (this->x - line_segment2_start.x)) / (-side2.x * side1.y + side1.x * side2.y);

	if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
		return { true,{ this->x + t * side1.x,  this->y + t * side1.y } };

	return { false,{} };
}

float Vector::DotProduct(Vector const& other) const
{
	return this->x * other.x + this->y * other.y;
}

float Vector::CrossProduct(Vector const& other) const
{
	return other.y * this->x - other.x * this->y;
}

float Vector::Polar() const
{
	if (this->Close(x, 0.f, 0.f))
	{
		if (y > 0.f)
		{
			return 90.f;
		}
		return y < 0.f ? 270.f : 0.f;
	}

	auto theta = atan(y / x) * 180.f / M_PI;
	if (x < 0.f)
	{
		theta = theta + 180.f;
	}
	if (theta < 0.f)
	{
		theta = theta + 360.f;
	}
	return theta;
}

float Vector::AngleBetween(Vector const& other) const
{
	auto theta = Polar() - other.Polar();
	if (theta < 0.f)
	{
		theta = theta + 360.f;
	}
	if (theta > 180.f)
	{
		theta = 360.f - theta;
	}
	return theta;
}

bool Vector::Close(float a, float b, float eps) const
{
	if (abs(eps) < FLT_EPSILON)
	{
		eps = static_cast<float>(1e-9);
	}
	return abs(a - b) <= eps;
}

// Expects angle in radians!
Vector Vector::Rotated(float angle) const
{
	auto const c = cos(angle);
	auto const s = sin(angle);

	return { static_cast<float>(x * c - y * s), static_cast<float>(y * c + x * s) };
}

Vector Vector::Perpendicular() const
{
	return { -y, x };
}

Vector Vector::Extend(Vector const& to, float distance) const
{
	const auto from = *this;
	const auto result = from + (to - from).Normalized() * distance;
	return result;
}

bool Vector::IsWall() const
{
	return g_NavMesh->HasFlag(*this, kNavFlagsWall);
}

bool Vector::IsWallOfGrass() const
{
	return g_NavMesh->HasFlag(*this, kNavFlagsGrass);
}

bool Vector::IsBuilding() const
{
	return g_NavMesh->HasFlag(*this, kNavFlagsBuilding);
}

bool Vector::IsOnScreen() const
{
	return this->x <= g_Renderer->ScreenWidth() && this->x >= 0 && this->y <= g_Renderer->ScreenHeight() && this->y >= 0;
}

bool Vector::IsUnderAllyTurret() const
{
	for (auto turret : g_ObjectManager->GetByType(EntityType::AITurretClient))
	{
		if (turret->IsValidTarget(FLT_MAX, FALSE) && turret->IsAlly() && turret->IsInAutoAttackRange(*this))
		{
			return true;
		}
	}

	return false;
}

bool Vector::IsUnderEnemyTurret() const
{
	if (this->IsValid())
	{
		for (auto turret : g_ObjectManager->GetByType(EntityType::AITurretClient))
		{
			if (turret->IsValidTarget(FLT_MAX, TRUE) && turret->IsEnemy() && turret->IsInAutoAttackRange(*this))
			{
				return true;
			}
		}
	}

	return false;
}

Vector2 Vector::WorldToScreen() const
{
	return g_Renderer->WorldToScreen(*this);
}

Vector& Vector::To3DWorld()
{
	this->z = g_NavMesh->GetHeightForPosition(*this);
	return *this;
}

Vector& Vector::To3DPlayer()
{
	this->z = g_LocalPlayer->Position().z;
	return *this;
}

int Vector::CountAlliesInRange(float range) const
{
	auto count = 0;

	for (auto hero : g_ObjectManager->GetByType(EntityType::AIHeroClient))
	{
		if (hero->IsValidTarget(FLT_MAX, FALSE) && hero->IsAlly() && hero->Distance(*this) < range)
		{
			count++;
		}
	}

	return count;
}

int Vector::CountEnemiesInRange(float range) const
{
	auto count = 0;

	for (auto hero : g_ObjectManager->GetByType(EntityType::AIHeroClient))
	{
		if (hero->IsValidTarget(FLT_MAX, TRUE) && hero->Distance(*this) < range)
		{
			count++;
		}
	}

	return count;
}

ProjectionInfo::ProjectionInfo(const bool is_on_segment, Vector const& segment_point, Vector const& line_point) :
	IsOnSegment(is_on_segment), LinePoint(line_point), SegmentPoint(segment_point)
{
}

IntersectionResult::IntersectionResult(const bool intersects, Vector const& point) :
	Intersects(intersects), Point(point)
{
}