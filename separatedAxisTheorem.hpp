#ifndef SEPARATEDAXISTHEOREM_HPP_
#define SEPARATEDAXISTHEOREM_HPP_

#include <cmath>
#include <array>
#include <SFML/System/Vector2.hpp>

// Maciej Zwoliński
// 18/05/2018

// This is my implementation of colision check based on separated axis theorem for given shapes:
// Rectangles
// Triangles
// Circles
// Adding general polygons implementation is present in my plans, gonna do that when I'll have some more free time

// It uses left-handed Cartesian coordinate system ( y's raises to down )

// Everything got packed into namespace to not colide with the sfml classes and for readability
// I'm using sfml's vector's class just for my comfort, you can easily implement your own and replace it

// Members of the shapes classes are public, I've used python mentality here, again for my own comfort, feel free to add set/get's as you want
// Colision methods are static

namespace sat {

    // measures distance between given points 
    double distance( sf::Vector2f point1, sf::Vector2f point2 );
    double distance( double x1, double y1, double x2, double y2 );

    // returns coords of a point rotated over given center
    sf::Vector2f getRotated( sf::Vector2f point, sf::Vector2f center, double rotation );

    class Axis {
    // simple axis of y = slope * x + x0 equation,
    // if vertical then x = x0
        public:
        bool vertical;
        double slope;
        double x0;
        Axis( double slope_ = 0, double x0_ = 0, bool vertical_ = false );
        // constructs axis that passes through 2 points
        Axis( sf::Vector2f point1, sf::Vector2f point2 );
        // constructs axis with known slope and 1 point it passes,
        // if slope equals 0 then it will be considered as horizontal, not vertical
        Axis( double slope_, sf::Vector2f point );
        // replace axis with perpendicular to given one
        void perpendicular();
        // return coords of the crosspoint of 2 axis
        sf::Vector2f cross_point( Axis second );
    };
    class Segment {
    // Simple as that, 2 points: start and the end,
    // lenght is never used but I want to leave it for futural purposes ( maybe )
        public:
        sf::Vector2f start, end;
        double lenght;
        Segment( double x1 = 0, double y1 = 0, double x2 = 0, double y2 = 0 );
        Segment( sf::Vector2f point1, sf::Vector2f point2 );
    };
    class Rect {
        public:
        sf::Vector2f center, size;
        double rotation;
        Rect( double x = 0, double y = 0, double width = 0, double height = 0, double rotation_ = 0 );
        // Returns corner position considering rotation, corners are counted from 0 clockwise starting from left top
        sf::Vector2f getCorner( int n );
        // Returns coords of closest corner to point given
        sf::Vector2f getClosestCorner( sf::Vector2f point );
        // Returns axis, counted from 0 clockwise, starting at the top one
        Axis getAxis( int n );
        void rotate( double angle );
        void setRotation( double angle );
        void move( double x, double y );
        void setPosition( double x, double y );
    };
    class Triangle {
        public:
        std::array< sf::Vector2f, 3 > points;
        sf::Vector2f center;
        double rotation;
        // Constructs triangle from the 3 points given, center is set automatically by calculating triangle's center of gravity
        Triangle ( sf::Vector2f = sf::Vector2f(0,0), sf::Vector2f = sf::Vector2f(0,0), sf::Vector2f = sf::Vector2f(0,0), double rotation_ = 0 );
        // returns corners in order given in constructor counted from 0
        sf::Vector2f getCorner( int n );
        // same as in rect
        sf::Vector2f getClosestCorner( sf::Vector2f point );
        // returns triangle's center of gravity
        sf::Vector2f getCenterOfGravity();
        // returns axis in order given to constructor, counted from 0 ( which connects 0 and 1 corner )
        Axis getAxis( int n );
        void rotate( double angle );
        void setRotation( double angle );
        void move( double x, double y );
        void setPosition( double x, double y );
    };
    class Circle {
        public:
        sf::Vector2f center;
        double radius;
        Circle( double x = 0, double y = 0, double r = 0 );
        void move( double x, double y );
        void setPosition( double x, double y );
    };
    class Colision {
        Colision();
        public:
        // Projects shape classes to the given axis
        static sf::Vector2f projectPointToAxis( sf::Vector2f point, Axis axis );
        static Segment projectSegmentToAxis( Segment seg, Axis axis );
        static Segment projectRectToAxis( Rect rect, Axis axis );
        static Segment projectTriToAxis( Triangle tri, Axis axis );
        static Segment projectCircleToAxis( Circle circle, Axis axis );
        // check if two segments overlap on the given axis
        static bool segmentsOverlap( Segment seg1, Segment seg2, Axis axis );
        // colision check for axis aligned bounding boxes
        static bool checkAABB( Rect first, Rect second );
        // colision check between each of implemented shapes
        static bool check( Rect first, Rect second );
        static bool check( Triangle tri1, Triangle tri2 );
        static bool check( Triangle tri, Rect rect );
        static bool check( Rect rect, Triangle tri );
        static bool check( Circle circle1, Circle circle2 );
        static bool check( Circle circle, Rect rect );
        static bool check( Rect rect, Circle circle );
        static bool check( Circle circle, Triangle tri );
        static bool check( Triangle tri, Circle circle );
    };
}

#endif 