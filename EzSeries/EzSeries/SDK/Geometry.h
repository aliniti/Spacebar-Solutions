#pragma once

#include "clipper.hpp"
#include "PluginSDK.h"

#define CircleLineSegmentN 16

namespace Geometry
{
	class Polygon
	{
	public:
		std::vector<Vector> Points;

		void Add(const Vector& point)
		{
			Points.push_back(point);
		}

		bool IsInside(const Vector& point)
		{
			return !IsOutside(point);
		}

		bool IsOutside(const Vector& point)
		{
			const auto p = ClipperLib::IntPoint(point.x, point.y);
			return ClipperLib::PointInPolygon(p, ToClipperPath()) != 1;
		}

		int PointInPolygon(const Vector& point)
		{
			const auto p = ClipperLib::IntPoint(point.x, point.y);
			return ClipperLib::PointInPolygon(p, ToClipperPath());
		}

		std::vector<ClipperLib::IntPoint> ToClipperPath()
		{
			std::vector<ClipperLib::IntPoint> result;

			for (const auto& point : Points)
				result.emplace_back(point.x, point.y);

			return result;
		}
	};

	class Arc
	{
	public:
		Vector End;
		int HitBox;
		Vector Start;
		float Distance;

		Arc(const Vector& start, const Vector& end, int hitbox)
		{
			Start = start;
			End = end;
			HitBox = hitbox;
			Distance = Start.Distance(End);
		}

		std::vector<Vector> CircleCircleIntersection(const Vector& center1, const Vector& center2, float radius1, float radius2)
		{
			std::vector<Vector> result;

			float d = center1.Distance(center2);

			if (d > radius1 + radius2 || d <= abs(radius1 - radius2))
			{
				return result;
			}

			float a = (radius1 * radius1 - radius2 * radius2 + d * d) / (2 * d);
			float h = (float)sqrt(radius1 * radius1 - a * a);
			Vector direction = (center2 - center1).Normalized();
			Vector pa = center1 + direction * a;
			Vector s1 = pa + direction.Perpendicular() * h;
			Vector s2 = pa - direction.Perpendicular() * h;

			result.push_back(s1);
			result.push_back(s2);
			return result;
		}

		Polygon ToPolygon(int offset = 0)
		{
			offset += HitBox;
			Polygon result = Polygon();

			float innerRadius = -0.1562f * Distance + 687.31f;
			float outerRadius = 0.35256f * Distance + 133.0f;

			outerRadius = outerRadius / (float)cos(2 * M_PI / CircleLineSegmentN);

			auto innerCenters = CircleCircleIntersection(Start, End, innerRadius, innerRadius);
			auto outerCenters = CircleCircleIntersection(Start, End, outerRadius, outerRadius);

			Vector innerCenter = innerCenters[0];
			Vector outerCenter = outerCenters[0];

			Vector direction = (End - outerCenter).Normalized();
			Vector end = (Start - outerCenter).Normalized();
			float maxAngle = (float)(direction.AngleBetween(end) * M_PI / 180.0f);

			float step = -maxAngle / CircleLineSegmentN;

			for (int i = 0; i < CircleLineSegmentN; i++)
			{
				float angle = step * i;
				Vector point = outerCenter + direction.Rotated(angle) * (outerRadius + 15 + offset);
				result.Add(point);
			}

			direction = (Start - innerCenter).Normalized();
			end = (End - innerCenter).Normalized();
			maxAngle = (float)(direction.AngleBetween(end) * M_PI / 180.0f);
			step = maxAngle / CircleLineSegmentN;

			for (int i = 0; i < CircleLineSegmentN; i++)
			{
				float angle = step * i;
				Vector point = innerCenter + direction.Rotated(angle) * fmax(0, innerRadius - offset - 20);
				result.Add(point);
			}

			return result;
		}
	};

	class Circle
	{
	public:
		Vector Center;
		float Radius;

		Circle(const Vector& center, float radius)
		{
			Center = center;
			Radius = radius;
		}

		Polygon ToPolygon(int offset = 0, float overrideWidth = -1)
		{
			Polygon result = Polygon();
			float outRadius = overrideWidth > 0
				? overrideWidth
				: (offset + Radius) / (float)cos(2 * M_PI / CircleLineSegmentN);

			double step = 2 * M_PI / CircleLineSegmentN;
			double angle = Radius;
			for (int i = 0; i <= CircleLineSegmentN; i++)
			{
				angle += step;
				Vector point = Vector(Center.x + outRadius * (float)cos(angle),
					Center.y + outRadius * (float)sin(angle), this->Center.z);
				result.Add(point);
			}

			return result;
		}
	};

	class Rectangle
	{
	public:
		Vector Direction;
		Vector Perpendicular;
		Vector REnd;
		Vector RStart;
		float Width;

		Rectangle(const Vector& start, const Vector& end, float widthStart)
		{
			RStart = start;
			REnd = end;
			Width = widthStart;
			Direction = (end - start).Normalized();
			Perpendicular = Direction.Perpendicular();
		}

		Polygon ToPolygon(int offset = 0, float overrideWidth = -1)
		{
			Polygon result = Polygon();

			result.Add(RStart +
				Perpendicular * (overrideWidth > 0 ? overrideWidth : Width + offset) -
				Direction * offset);
			result.Add(RStart -
				Perpendicular * (overrideWidth > 0 ? overrideWidth : Width + offset) -
				Direction * offset);
			result.Add(REnd -
				Perpendicular * (overrideWidth > 0 ? overrideWidth : Width + offset) +
				Direction * offset);
			result.Add(REnd +
				Perpendicular * (overrideWidth > 0 ? overrideWidth : Width + offset) +
				Direction * offset);

			return result;
		}
	};

	class Ring
	{
	public:
		Vector Center;
		float Radius;
		float RingRadius;

		Ring(const Vector& center, float radius, float ringRadius)
		{
			Center = center;
			Radius = radius;
			RingRadius = ringRadius;
		}

		Polygon ToPolygon(int offset = 0)
		{
			Polygon result;

			float outRadius = (float)(offset + this->Radius + this->RingRadius) / cosf(2 * M_PI / CircleLineSegmentN);
			float innerRadius = this->Radius - this->RingRadius - offset;

			for (int i = 0; i <= CircleLineSegmentN; i++)
			{
				float angle = i * 2 * M_PI / CircleLineSegmentN;
				auto point = Vector(
					this->Center.x - outRadius * cosf(angle),
					this->Center.y - outRadius * sinf(angle), this->Center.z);

				result.Add(point);
			}
			for (int i = 0; i <= CircleLineSegmentN; i++)
			{
				float angle = i * 2 * M_PI / CircleLineSegmentN;
				auto point = Vector(
					this->Center.x - innerRadius * cosf(angle),
					this->Center.y - innerRadius * sinf(angle), this->Center.z);

				result.Add(point);
			}
			return result;
		}
	};

	class Sector
	{
	public:
		float Angle;
		Vector Center;
		Vector Direction;
		float Radius;

		Sector(const Vector& center, const Vector& direction, float angle, float radius)
		{
			Center = center;
			Direction = direction;
			Angle = angle;
			Radius = radius;
		}

		Polygon ToPolygon(int offset = 0)
		{
			Polygon result = Polygon();
			float outRadius = (Radius + offset) / (float)cos(2 * M_PI / CircleLineSegmentN);

			result.Add(Center);
			Vector side1 = Direction.Rotated(-Angle * 0.5f);

			for (int i = 0; i <= CircleLineSegmentN; i++)
			{
				Vector cDirection = side1.Rotated(i * Angle / CircleLineSegmentN).Normalized();
				result.Add(Vector(Center.x + outRadius * cDirection.x,
					Center.y + outRadius * cDirection.y, Center.z));
			}

			return result;
		}
	};

	class Geometry
	{
	public:

		static std::vector<std::vector<ClipperLib::IntPoint>> ClipPolygons(std::vector<Polygon> polygons)
		{
			std::vector<std::vector<ClipperLib::IntPoint>> subj(polygons.size());
			std::vector<std::vector<ClipperLib::IntPoint>> clip(polygons.size());

			for (Polygon polygon : polygons)
			{
				subj.push_back(polygon.ToClipperPath());
				clip.push_back(polygon.ToClipperPath());
			}

			auto solution = std::vector<std::vector<ClipperLib::IntPoint>>();
			ClipperLib::Clipper c;
			c.AddPaths(subj, ClipperLib::PolyType::ptSubject, true);
			c.AddPaths(clip, ClipperLib::PolyType::ptClip, true);
			c.Execute(ClipperLib::ClipType::ctUnion, solution, ClipperLib::PolyFillType::pftNegative, ClipperLib::PolyFillType::pftNonZero);

			return solution;
		}

		static std::vector<Vector> CutPath(std::vector<Vector> path, float distance)
		{
			std::vector<Vector> result;
			auto Distance = distance;

			if (distance < 0)
			{
				path[0] = path[0] + (path[1] - path[0]).Normalized() * distance;
				return path;
			}

			for (auto i = 0; i < path.size() - 1; i++)
			{
				auto dist = path[i].Distance(path[i + 1]);
				if (dist > Distance)
				{
					result.push_back(path[i] + (path[i + 1] - path[i]).Normalized() * Distance);
					for (auto j = i + 1; j < path.size(); j++)
					{
						result.push_back(path[j]);
					}
					break;
				}
				Distance -= dist;
			}
			return result.size() > 0 ? result : std::vector<Vector>{ path.back() };
		}

		static float PathLength(std::vector<Vector> path)
		{
			float distance = 0.0f;
			for (int i = 0; i < path.size() - 1; i++)
			{
				distance += path[i].Distance(path[i + 1]);
			}
			return distance;
		}

		static Vector VectorMovementCollision(const Vector& startPoint1, const Vector& endPoint1, float v1, const Vector& startPoint2, float v2, float& t1, float delay = 0.f)
		{
			auto sP1x = startPoint1.x;
			auto sP1y = startPoint1.y;
			auto eP1x = endPoint1.x;
			auto eP1y = endPoint1.y;
			auto sP2x = startPoint2.x;
			auto sP2y = startPoint2.y;

			float d = eP1x - sP1x, e = eP1y - sP1y;
			float dist = (float)sqrt(d * d + e * e);
			t1 = NAN;
			float S = abs(dist) > FLT_EPSILON ? v1 * d / dist : 0.f,
				K = (abs(dist) > FLT_EPSILON) ? v1 * e / dist : 0.f;

			float r = sP2x - sP1x, j = sP2y - sP1y;
			auto c = r * r + j * j;

			if (dist > 0.f)
			{
				if (abs(v1 - FLT_MAX) < FLT_EPSILON)
				{
					auto t = dist / v1;
					t1 = v2 * t >= 0.f ? t : NAN;
				}
				else if (abs(v2 - FLT_MAX) < FLT_EPSILON)
				{
					t1 = 0.f;
				}
				else
				{
					float a = S * S + K * K - v2 * v2, b = -r * S - j * K;

					if (abs(a) < FLT_EPSILON)
					{
						if (abs(b) < FLT_EPSILON)
						{
							t1 = (abs(c) < FLT_EPSILON) ? 0.f : NAN;
						}
						else
						{
							auto t = -c / (2 * b);
							t1 = (v2 * t >= 0.f) ? t : NAN;
						}
					}
					else
					{
						auto sqr = b * b - a * c;
						if (sqr >= 0)
						{
							auto nom = (float)sqrt(sqr);
							auto t = (-nom - b) / a;
							t1 = v2 * t >= 0.f ? t : NAN;
							t = (nom - b) / a;
							auto t2 = (v2 * t >= 0.f) ? t : NAN;

							if (!isnan(t2) && !isnan(t1))
							{
								if (t1 >= delay && t2 >= delay)
								{
									t1 = fmin(t1, t2);
								}
								else if (t2 >= delay)
								{
									t1 = t2;
								}
							}
						}
					}
				}
			}
			else if (abs(dist) < FLT_EPSILON)
			{
				t1 = 0.f;
			}

			return (!isnan(t1)) ? Vector(sP1x + S * t1, sP1y + K * t1, 0) : Vector(0, 0, 0);
		}

		static Vector PositionAfter(std::vector<Vector> self, int t, int speed, int delay = 0)
		{
			int distance = fmax(0, t - delay) * speed / 1000;

			for (int i = 0; i <= self.size() - 2; i++)
			{
				Vector from = self[i];
				Vector to = self[i + 1];
				int d = (int)to.Distance(from);
				if (d > distance)
				{
					return from + (to - from).Normalized() * distance;
				}
				distance -= d;
			}

			return self[self.size() - 1];
		}

		static Polygon ToPolygon(std::vector<ClipperLib::IntPoint> v)
		{
			Polygon polygon = Polygon();
			for (ClipperLib::IntPoint point : v)
				polygon.Add(Vector(point.X, point.Y, 0));
			return polygon;
		}

		static std::vector<Polygon> ToPolygons(std::vector<std::vector<ClipperLib::IntPoint>> v)
		{
			std::vector<Polygon> result;
			for (auto item : v)
				result.push_back(ToPolygon(item));
			return result;
		}
	};
}
