#include "dollar.hpp"

#include <cmath>

namespace dollar {
using std::pair;
using std::vector;
namespace {
float
pathLength(const vector<Point>& p)
{
    float length = 0;
    for (size_t i = 1; i < p.size(); ++i) {
        length += std::hypotf(p[i].first - p[i - 1].first, p[i].second - p[i - 1].second);
    }
    return length;
}
// Resample the points of a gesture into n evenly spaced points. Protractor uses
// the same resampling method as the $1 recognizer3 does, although Protractor
// only needs n = 16 points to perform optimally. The pseudo code of this step
// is borrowed from the $1 recognizer.
vector<Point>
resample(std::vector<Point> p, int n)
{
    float len = pathLength(p) / (n - 1);
    float residualD = 0;
    vector<Point> newPoints;
    newPoints.reserve(p.size());
    newPoints.push_back(p[0]);
    Point last = p[0];
    for (size_t i = 1; i < p.size(); ++i) {
        const float d = std::hypotf(p[i].first - p[i - 1].first, p[i].second - p[i - 1].second);
        if (residualD + d >= len) {
            const float frac = (len - residualD) / d;
            newPoints.push_back(std::make_pair(p[i - 1].first + frac * (p[i].first - last.first),
                                               p[i - 1].second + frac * (p[i].second - last.second)));
            residualD = 0;
            last = p[i];
            p[i] = newPoints.back();
        } else {
            residualD += d;
            last = p[i];
            ++i;
        }
    }
    return newPoints;
}

Point
centroid(const vector<Point>& p)
{
    float x = 0;
    float y = 0;
    for (const Point& pt : p) {
        x += pt.first;
        y += pt.second;
    }
    return std::make_pair(x / p.size(), y / p.size());
}

vector<Point>
centerPoints(const vector<Point>& p)
{
    const Point center = centroid(p);
    vector<Point> newPoints;
    newPoints.reserve(p.size());
    for (const Point& pt : p) {
        newPoints.push_back(std::make_pair(pt.first - center.first, pt.second - center.second));
    }
    return newPoints;
}

VectorizedStroke
vectorizeStroke(const vector<Point>& p, Orientation oSensitive)
{
    vector<Point> centeredPoints = centerPoints(p);
    double indicativeAngle = std::atan2(centeredPoints[0].second, centeredPoints[0].first);
    float delta = -indicativeAngle;
    if (oSensitive == Orientation::Sensitive) {
        static const float pi = atanf(1.0f) * 4;
        float baseOrientation = pi / 4.f * floorf((indicativeAngle + pi / 8.f) / (pi / 4.f));
        delta = baseOrientation - indicativeAngle;
    }
    float s = 0;
    vector<VecItem> vec;
    vec.reserve(2 * p.size());
    for (const Point& pt : centeredPoints) {
        const float newX = pt.first * cosf(delta) - pt.second * sinf(delta);
        const float newY = pt.second * cosf(delta) + pt.first * sinf(delta);
        vec.push_back(std::make_pair(newX, newY));
        s += newX * newX + newY * newY;
    }
    const float magnitude = sqrtf(s);
    for (VecItem& item : vec) {
        item.first /= magnitude;
        item.second /= magnitude;
    }
    return vec;
}

float
vecCrossProduct(const pair<float, float>& a, const pair<float, float>& b)
{
    return a.first * b.second - a.second * b.first;
}
}
Stroke::Stroke(const std::vector<Point>& points, Orientation orientationSensitivity, int samplePointCnt)
  : rawPoints(points)
  , vectorizedStroke(vectorizeStroke(resample(points, samplePointCnt), orientationSensitivity))
{
}

namespace detail {
float
optimalCosineDistance(const VectorizedStroke& stroke1, const VectorizedStroke& stroke2)
{
    float a = 0;
    float b = 0;
    for (size_t i = 0; i < stroke1.size(); i += 2) {
        a += vecCrossProduct(stroke1[i], stroke2[i]) + vecCrossProduct(stroke1[i + 1], stroke2[i + 1]);
        b += vecCrossProduct(stroke1[i], stroke2[i + 1]) + vecCrossProduct(stroke1[i + 1], stroke2[i]);
    }
    const float angle = atan2(b, a);
    return acosf(a * cosf(angle) + b * sinf(angle));
}
}
}
