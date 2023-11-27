#ifndef DOLLAR_PROTARCTOR_H_
#define DOLLAR_PROTARCTOR_H_

#include <utility>
#include <vector>

namespace dollar {
using Point = std::pair<float, float>;
using VecItem = std::pair<float, float>;
using VectorizedStroke = std::vector<VecItem>;

enum class Orientation
{
    Sensitive = 0,
    Insensitive = 1,
};
// This represents a vectorized stroke
class Stroke
{
    std::vector<Point> rawPoints;
    VectorizedStroke vectorizedStroke;

  public:
    // @param points: a vector of sampled points on this stroke.
    // Must contains at least 2 points. No duplicated points allowed.
    Stroke(const std::vector<Point>& points, Orientation orientationSensitivity, int samplePointCnt = 16);
    const std::vector<Point>& getRawPoints() const { return rawPoints; }
    // This returns a VectorizedStroke with length 2n, where n
    // is the number of input points
    const VectorizedStroke& getVectorizedStroke() const { return vectorizedStroke; }
};

using Score = float;

// Returns an iterator of the most matching template and its score
// ItT should be a forward_iterator pointing to {const Stroke}
// This version never returns {templateEnd}
template<typename ItT>
std::pair<ItT, Score>
recognize(const Stroke& target, ItT templateItBegin, ItT templateItEnd);

// Shorthand of the above function that returns {templateEnd}
// when the most matching score is below {threshold}
template<typename ItT>
ItT
recognize(const Stroke& target, ItT templateItBegin, ItT templateItEnd, Score threshold)
{
    auto [it, score] = recognize(target, templateItBegin, templateItEnd);
    if (score < threshold) {
        return templateItEnd;
    }
    return it;
}

namespace detail {
float
optimalCosineDistance(const VectorizedStroke& stroke1, const VectorizedStroke& stroke2);
}

// impl
//
// @internal
// @{
template<typename ItT>
std::pair<ItT, Score>
recognize(const Stroke& target, ItT templateItBegin, ItT templateItEnd)
{
    float maxScore = 0;
    ItT matchedIt = templateItEnd;
    for (ItT templateIt = templateItBegin; templateIt != templateItEnd; ++templateIt) {
        const float d = detail::optimalCosineDistance(target.getVectorizedStroke(), templateIt->getVectorizedStroke());
        const float score = 1.0f / d;
        if (score > maxScore) {
            maxScore = score;
            matchedIt = templateIt;
        }
    }
    return { matchedIt, maxScore };
}
// @}
}

#endif
