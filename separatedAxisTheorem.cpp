#include "separatedAxisTheorem.hpp"

//Functions ----------------------------------------------------------
    double sat::distance( sf::Vector2f point1, sf::Vector2f point2 ) {
        return sqrt( pow( abs( point2.x - point1.x ), 2 ) +  pow( abs( point2.y - point1.y ), 2 ) );
    }

    double sat::distance( double x1, double y1, double x2, double y2 ) {
        return sqrt( pow( abs( x2 - x1 ), 2 ) +  pow( abs( y2 -y1 ), 2 ) );
    }

    sf::Vector2f sat::getRotated( sf::Vector2f point, sf::Vector2f center, double rotation ) {
        // translate x and y to the center of Cartesian coordinate system
        sf::Vector2f tmpPoint( point.x - center.x, point.y - center.y );
        // multiply by Rotation matrix
        double rx = tmpPoint.x * cos(rotation) - tmpPoint.y * sin(rotation);
        double ry = tmpPoint.x * sin(rotation) + tmpPoint.y * cos(rotation);
        // translate back to center
        return sf::Vector2f( rx + center.x, ry + center.y );
    }

//Axis ---------------------------------------------------------------------
    sat::Axis::Axis( double slope_, double x0_, bool vertical_ ) {
        vertical = vertical_;
        if ( !vertical )
            slope = slope_;
        else
            slope = 0;
        x0 = x0_;
    }

    sat::Axis::Axis( sf::Vector2f point1, sf::Vector2f point2 ) {
        if ( point1.x != point2.x || point1.y != point2.y ) {
            if ( point1.x != point2.x ) {
                vertical = false;
                if ( point1.y != point2.y )
                    slope = ( point2.y - point1.y )/( point2.x - point1.x );
                else
                    slope = 0;
                 x0 = point1.y - slope * point1.x;
            } else {
                vertical = true;
                slope = 0;
                x0 = point1.x;
            }
        }
    }

    sat::Axis::Axis( double slope_, sf::Vector2f point ) {
        vertical = false;
        slope = slope_;
        x0 = point.y - slope * point.x;
    }

    void sat::Axis::perpendicular() {
        if( slope )
            slope = ( -1 )/slope;
        else if ( !slope && !vertical )
            vertical = true;
        else
            vertical = false;
    }

    sf::Vector2f sat::Axis::cross_point( sat::Axis second ) {
        if ( slope != second.slope && !vertical && !second.vertical ) {
            double x = ( x0 - second.x0 )/( second.slope - slope );
            double y = slope * x + x0;
            return sf::Vector2f( x, y ); 
        } else if ( vertical && !second.vertical ) {
            double x = x0;
            double y = second.slope * x + second.x0;
            return sf::Vector2f( x, y ); 
        } else if ( !vertical && second.vertical ) {
            double x = second.x0;
            double y = slope * second.x0 + x0;
            return sf::Vector2f( x, y ); 
        }
        bool parallel = true;
        throw parallel;
    }
//Segments ----------------------------------------------------------------------
    sat::Segment::Segment( double x1, double y1, double x2, double y2 ) {
        if ( x1 < x2 ) {
            start = sf::Vector2f( x1, y1 );
            end = sf::Vector2f( x2, y2 );
        } else {
            start = sf::Vector2f( x2, y2 );
            end = sf::Vector2f( x1, y1 );
        }
        lenght = distance( x1, y1, x2, y2 );
    }
    sat::Segment::Segment( sf::Vector2f point1, sf::Vector2f point2 ) {
        if ( point1.x < point2.x ) {
            start = point1;
            end = point2;
        } else {
            start = point2;
            end = point1;
        }
        lenght = distance( point1, point2 );
    }
//Rectangulars -------------------------------------------------------------------
    sat::Rect::Rect( double x, double y, double width, double height, double rotation_ ) {
        center = sf::Vector2f( x, y );
        size = sf::Vector2f( width, height );
        rotation = rotation_;
    }
    sf::Vector2f sat::Rect::getCorner( int n ) {
        sf::Vector2f point;
        switch (n) {
            case 0: 
                point = sf::Vector2f( center.x - size.x/2, center.y - size.y/2 );
                break;
            case 1:
                point = sf::Vector2f( center.x + size.x/2, center.y - size.y/2 );
                break;
            case 2:
                point = sf::Vector2f( center.x + size.x/2, center.y + size.y/2 );
                break;
            case 3:
                point = sf::Vector2f( center.x - size.x/2, center.y + size.y/2 );
                break;
        }
        return getRotated( point, center, rotation );
    }
    sf::Vector2f sat::Rect::getClosestCorner( sf::Vector2f point ) {
        sf::Vector2f closest = getCorner(0);
        for ( int i = 1; i < 4; i++ ) {
            if ( distance( getCorner( i ), point ) < distance( closest, point ) )
                closest = getCorner( i );
        }
        return closest;
    }
    sat::Axis sat::Rect::getAxis( int n ) { 
        return Axis( getCorner(n), getCorner( ( n+1 )%4 ) ); 
    }
    void sat::Rect::rotate( double angle ) { 
        rotation += angle;
    }
    void sat::Rect::setRotation( double angle ) { 
        rotation = angle; 
    }
    void sat::Rect::move( double x, double y ) {
        center += sf::Vector2f( x, y );
    }
    void sat::Rect::setPosition( double x, double y ) {
        center = sf::Vector2f( x, y );
    }
//TRIANGLES ---------------------------------------------------------------------
    sat::Triangle::Triangle ( sf::Vector2f first, sf::Vector2f second, sf::Vector2f third, double rotation_ ) { 
        points[0] = first;
        points[1] = second;
        points[2] = third;
        rotation = rotation_;
        center = getCenterOfGravity();
    }
    sf::Vector2f sat::Triangle::getCorner( int n ) {
        return getRotated( points[n], center, rotation );
    }
    sf::Vector2f sat::Triangle::getClosestCorner( sf::Vector2f point ) {
        sf::Vector2f closest = getCorner(0);
        for ( int i = 1; i < 3; i++ ) {
            if ( distance( getCorner(i), point ) < distance( closest, point ) )
                closest = getCorner(i);
        }
        return closest;
    }
    sf::Vector2f sat::Triangle::getCenterOfGravity() {
        sf::Vector2f middlePoint1( ( getCorner(2).x + getCorner(1).x )/2, ( getCorner(2).y + getCorner(1).y)/2 );
        sf::Vector2f middlePoint2( ( getCorner(0).x + getCorner(2).x )/2, ( getCorner(0).y + getCorner(2).y)/2 );
        Axis middleAxis1( getCorner(0), middlePoint1 );
        Axis middleAxis2( getCorner(1), middlePoint2 );
        return middleAxis1.cross_point( middleAxis2 );
    }
    sat::Axis sat::Triangle::getAxis( int n ) { 
        return Axis( getCorner(n), getCorner( (n+1)%3 ) ); 
    }
    void sat::Triangle::rotate( double angle ) { 
        rotation += angle;
    }
    void sat::Triangle::setRotation( double angle ) { 
        rotation = angle;
    }
    void sat::Triangle::move( double x, double y ) {
        for ( auto& point : points )
            point += sf::Vector2f( x, y );
        center += sf::Vector2f( x, y );
    }
    void sat::Triangle::setPosition( double x, double y ) {
        sf::Vector2f offset( x - center.x, y - center.y );
        for ( auto& point : points )
            point += offset;
        center = sf::Vector2f( x, y );
    }
//CIRCLES -----------------------------------------------------------------------
    sat::Circle::Circle( double x, double y, double r ) {
        center = sf::Vector2f( x, y );
        radius = r;
    }
    void sat::Circle::move( double x, double y ) {
        center += sf::Vector2f( x, y );
    }
    void sat::Circle::setPosition( double x, double y ) {
        center = sf::Vector2f( x, y );
    }
//COLISION --------------------------------------------------------------------------
    sat::Colision::Colision(){}
    sf::Vector2f sat::Colision::projectPointToAxis( sf::Vector2f point, Axis axis ) {
        if ( axis.slope != 0 ) {
            Axis tmpAxis( (-1)/axis.slope, point );        
            return tmpAxis.cross_point( axis );
        } else if ( axis.vertical ) {
            return sf::Vector2f( axis.x0, point.y );
        } else {
            return sf::Vector2f( point.x, axis.x0 );
        }
    }
    sat::Segment sat::Colision::projectSegmentToAxis( Segment seg, Axis axis ) {
        sf::Vector2f point1 = projectPointToAxis( seg.start, axis );
        sf::Vector2f point2 = projectPointToAxis( seg.end, axis );
        return Segment( point1, point2 );
    }
    sat::Segment sat::Colision::projectRectToAxis( Rect rect, Axis axis ) {
        sf::Vector2f points[4];
        for ( int i = 0; i < 4; i++ )
            points[i] = projectPointToAxis( rect.getCorner(i), axis );
        sf::Vector2f min = points[0], max = points[0];
        for ( auto& point : points ) {
            if ( axis.vertical ? point.y < min.y : point.x < min.x )
                min = point;
            if ( axis.vertical ? point.y > max.y : point.x > max.x )
                max = point;
        }
        return Segment( min, max );
    }
    sat::Segment sat::Colision::projectTriToAxis( Triangle tri, Axis axis ) {
        sf::Vector2f points[3];
        for ( int i = 0; i < 3; i++ )
            points[i] = projectPointToAxis( tri.getCorner(i), axis );
        sf::Vector2f min = points[0], max = points[0];
        for ( auto& point : points ) {
            if ( axis.vertical ? point.y < min.y : point.x < min.x )
                min = point;
            if ( axis.vertical ? point.y > max.y : point.x > max.x )
                max = point;
        }
        return Segment( min, max );
    }
    sat::Segment sat::Colision::projectCircleToAxis( Circle circle, Axis axis ) {
        if ( axis.vertical ) {
            sf::Vector2f point1( axis.x0, circle.center.x - circle.radius );
            sf::Vector2f point2( axis.x0, circle.center.x + circle.radius );
            return Segment( point1, point2 );
        } else {
            sf::Vector2f centerProj = projectPointToAxis( circle.center, axis ); 
            sf::Vector2f crosspoint1 = getRotated( sf::Vector2f( centerProj.x + circle.radius, centerProj.y ), centerProj, atan( axis.slope ) );
            sf::Vector2f crosspoint2 = getRotated( sf::Vector2f( centerProj.x - circle.radius, centerProj.y ), centerProj, atan( axis.slope ) );
            return Segment( crosspoint1, crosspoint2 );
        }
    }
    bool sat::Colision::segmentsOverlap( Segment seg1, Segment seg2, Axis axis ) {
        Segment first = projectSegmentToAxis( seg1, axis );
        Segment second = projectSegmentToAxis( seg2, axis );
        if ( axis.slope != 0 || axis.vertical ) {
            if ( first.start.y < first.end.y ) {
                if ( first.start.y < second.end.y && second.start.y < first.end.y )
                    return true;
                else return false;
            } else {
                if ( first.start.y > second.end.y && second.start.y > first.end.y )
                    return true;
                else return false;
            }
        } else {
            if ( first.start.x < second.end.x && second.start.x < first.end.x )
                return true;
            else return false;
        }
        
    }
    bool sat::Colision::checkAABB( Rect first, Rect second) {
        Segment topSide1( first.getCorner(3), first.getCorner(2) );
        Segment topSide2( second.getCorner(3), second.getCorner(2) );
        Segment leftSide1( first.getCorner(3), first.getCorner(0) );
        Segment leftSide2( second.getCorner(3), second.getCorner(0) );
        if ( segmentsOverlap( topSide1, topSide2, first.getAxis(0) ) && segmentsOverlap( leftSide1, leftSide2, first.getAxis(1) ) )
            return true;
        return false;
    }
    bool sat::Colision::check( Rect first, Rect second ) {
        // do for every Axis in rects
        for ( int i = 0; i < 4; i++ ) {
            Axis axis = i < 2 ? first.getAxis( i%2 ) : second.getAxis( i%2 );
            Segment seg1 = projectRectToAxis( first, axis );
            Segment seg2 = projectRectToAxis( second, axis );
            if ( ! segmentsOverlap( seg1, seg2, axis ) )
                return false;
        }
        return true;
    }
    bool sat::Colision::check( Triangle tri1, Triangle tri2 ) {
        // do for every Axis in both polygons
        for ( int i = 0; i < 6; i++ ) {
            Axis axis = i < 3 ? tri1.getAxis( i%3 ) : tri2.getAxis( i%3 );
            axis.perpendicular();
            Segment seg1 = projectTriToAxis( tri1, axis );
            Segment seg2 = projectTriToAxis( tri2, axis );
            if ( ! segmentsOverlap( seg1, seg2, axis ) )
                return false;
        }
        return true;
    }
    bool sat::Colision::check( Triangle tri, Rect rect ) {
        // do for every Axis in both polygons
        for ( int i = 0; i < 5; i++ ) {
            Axis axis = i < 3 ? tri.getAxis( i%3 ) : rect.getAxis( i%2 );
            axis.perpendicular();
            Segment seg1 = projectTriToAxis( tri, axis );
            Segment seg2 = projectRectToAxis( rect, axis );
            if ( ! segmentsOverlap( seg1, seg2, axis ) )
                return false;
        }
        return true;
    }
    bool sat::Colision::check( Rect rect, Triangle tri ) {
        return check( tri, rect );
    }
    bool sat::Colision::check( Circle circle1, Circle circle2 ) {
        // do for Axis between centers
        Axis axis( circle1.center, circle2.center );
        Segment seg1 = projectCircleToAxis( circle1, axis );
        Segment seg2 = projectCircleToAxis( circle2, axis );
        return segmentsOverlap( seg1, seg2, axis );
    }
    bool sat::Colision::check( Rect rect, Circle circle ) {
        // do for Axis between centers, Axis between closest corner and center and each Axis in polygon
        Axis axis( rect.center, circle.center );
        Segment seg1 = projectRectToAxis( rect, axis );
        Segment seg2 = projectCircleToAxis( circle, axis );
        if ( !segmentsOverlap( seg1, seg2, axis ) )
            return false;
        axis = Axis( rect.getClosestCorner( circle.center ), circle.center );
        seg1 = projectRectToAxis( rect, axis );
        seg2 = projectCircleToAxis( circle, axis );
        if ( !segmentsOverlap( seg1, seg2, axis ) )
            return false;
        for ( int i = 0; i < 2; i++ ) {
            axis = rect.getAxis(i);
            seg1 = projectRectToAxis( rect, axis );
            seg2 = projectCircleToAxis( circle, axis );
            if ( !segmentsOverlap( seg1, seg2, axis ) )
                return false;
        }
        return true;
    }
    bool sat::Colision::check( Triangle tri, Circle circle ) {
        // do for Axis between centers, Axis between closest corner and center and each Axis in polygon
        Axis axis( tri.center, circle.center );
        Segment seg1 = projectTriToAxis( tri, axis );
        Segment seg2 = projectCircleToAxis( circle, axis );
        if ( !segmentsOverlap( seg1, seg2, axis ) )
            return false;
        axis = Axis( tri.getClosestCorner( circle.center ), circle.center );
        seg1 = projectTriToAxis( tri, axis );
        seg2 = projectCircleToAxis( circle, axis );
        if ( !segmentsOverlap( seg1, seg2, axis ) )
            return false;
        for ( int i = 0; i < 3; i++ ) {
            axis = tri.getAxis(i);
            seg1 = projectTriToAxis( tri, axis );
            seg2 = projectCircleToAxis( circle, axis );
            if ( !segmentsOverlap( seg1, seg2, axis ) )
                return false;
        }
        return true;
    }
    bool sat::Colision::check( Circle circle, Rect rect ) {
        return check( rect, circle );
    }
    bool sat::Colision::check( Circle circle, Triangle tri ) {
        return check( tri, circle );
    }
    