// Copyright (c) 2023 UltiMaker
// CuraEngine is released under the terms of the AGPLv3 or higher

#ifndef GEOMETRY_CLOSED_POLYLINE_H
#define GEOMETRY_CLOSED_POLYLINE_H

#include "geometry/Polyline.h"

namespace cura
{

class OpenPolyline;

/*! @brief This describes a polyline which forms a closed path.
 *  @sa https://github.com/Ultimaker/CuraEngine/wiki/Geometric-Base-Types#closedpolyline
 *
 *  The path may be closed:
 *    * Explicitely, which means the last point is at the same position as the first point.
 *      In this case, in order to iterate over the segments, you just have to iterate over
 *      the actual points.
 *    * Implicitely, which means the last and first point are at different positions. In this
 *      case, to iterate over the segments, you have to consider an additional segment
 *      between the last and first point
 *
 *  The difference is made because it is easier to iterate over segments when the path is
 *  explicitely closed, but ClipperLib uses implicitely closed paths. It is also a bit healthier
 *  to use implicitely closed because there is no risk that the first and last point become different
 */
class ClosedPolyline : public Polyline
{
private:
    bool explicitely_closed_{ false };

public:
    /*!
     * \brief Builds an empty closed polyline
     * \param explicitely_closed Indicates whether the line will be explicitely closed
     * \warning By default, the line is tagged as explicitely closed. We need this default
     *          constructor in various places, but be careful that the interpretation of the points
     *          added later will depend on this.
     */
    ClosedPolyline(bool explicitely_closed = false)
        : Polyline()
        , explicitely_closed_(explicitely_closed)
    {
    }

    /*! \brief Creates a copy of the given polyline */
    ClosedPolyline(const ClosedPolyline& other) = default;

    /*! \brief Constructor that takes ownership of the inner points list from the given polyline */
    ClosedPolyline(ClosedPolyline&& other) = default;

    /*!
     * \brief Constructor with a points initializer list, provided for convenience
     * \param explicitely_closed Specify whether the given points form an explicitely closed line
     */
    ClosedPolyline(const std::initializer_list<Point2LL>& initializer, bool explicitely_closed)
        : Polyline(initializer)
        , explicitely_closed_(explicitely_closed)
    {
    }

    /*!
     * \brief Constructor with an existing list of points
     * \param explicitely_closed Specify whether the given points form an explicitely closed line
     */
    explicit ClosedPolyline(const ClipperLib::Path& points, bool explicitely_closed)
        : Polyline(points)
        , explicitely_closed_(explicitely_closed)
    {
    }

    /*!
     * \brief Constructor that takes ownership of the given list of points
     * \param explicitely_closed Specify whether the given points form an explicitely closed line
     */
    explicit ClosedPolyline(ClipperLib::Path&& points, bool explicitely_closed)
        : Polyline(points)
        , explicitely_closed_(explicitely_closed)
    {
    }

    /*! @see Polyline::addClosingSegment() */
    virtual bool addClosingSegment() const
    {
        return ! explicitely_closed_;
    }

    /*! @see Polyline::addClosingSegment() */
    virtual size_t segmentsCount() const override;

    /*! @see Polyline::isValid() */
    virtual bool isValid() const override;

    ClosedPolyline& operator=(const ClosedPolyline& other)
    {
        Polyline::operator=(other);
        return *this;
    }

    ClosedPolyline& operator=(ClosedPolyline&& other)
    {
        Polyline::operator=(other);
        return *this;
    }

    bool isExplicitelyClosed() const
    {
        return explicitely_closed_;
    }

    /*!
     * \brief Sets whether the points set is to be treated as explicitely or implicitely closed
     * \warning This does not actually changes the points set, only the interpretation of it will
     *          change. So use this method only if you really know what you are doing.
     */
    void setExplicitelyClosed(bool explicitely_closed)
    {
        explicitely_closed_ = explicitely_closed;
    }

    /*!
     * Clipper function.
     * Returns false if outside, true if inside; if the point lies exactly on the border, will return 'border_result'.
     *
     * http://www.angusj.com/delphi/clipper/documentation/Docs/Units/ClipperLib/Functions/PointInPolygon.htm
     */
    bool inside(const Point2LL& p, bool border_result = false) const;

    bool inside(const ClipperLib::Path& polygon) const;

    /*!
     * \brief Converts the closed polyline to an open polyline which happens to have its end and start points at the same
     *        position, making it a pseudo-closed polyline. Although this should never be required in practice, there
     *        are many places in the code where this is done because historically we wouldn't make a clear difference
     *        between open and closed polylines
     * \return An open polyline instance, with the end point at the same position of the start point
     */
    OpenPolyline toPseudoOpenPolyline() const;
};

} // namespace cura

#endif // GEOMETRY_CLOSED_POLYLINE_H
