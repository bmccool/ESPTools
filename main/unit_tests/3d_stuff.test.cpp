#include <catch2/catch_all.hpp>
#include "3d_stuff.h"

TEST_CASE("First Test", "[test]"){
    REQUIRE(1 == 1);
}

TEST_CASE("system::walk_perp_z", "[test, system]"){
    /* 
    Given a pole at (1,1,1) and a point at (1,10,1)
    Walking this point perpendicular to the resultant force
    vector and the z axis we would expect the next integer point to 
    be (0,10,1)
    */
    Pole p1(1, 1, 1);
    Point3 p2(1, 10, 1);
    System system;
    system.poles.push_back(p1);
    //system.draw_rings(screen_buffer);
    Point3 walked_point = system.walk_perp_z(Vec3(0, -9, 0), p2);

    Point3 expected_point(0, 10, 1);
    REQUIRE(walked_point == expected_point);
}

TEST_CASE("system::walk_perp_z_circle", "[test, system]"){
    /* 
    Given a pole at (1,1,1) and a point at (1,10,1)
    Walking this point perpendicular to the resultant force
    vector and the z axis we would expect the next integer point to 
    be (0,10,1)
    */
    
    System system;
    system.poles.push_back(Pole(0,0,0));

    // Circle with origin (0,0,0) and starting point (0,5,0)
    REQUIRE(system.walk_perp_z(system.force_vector(Point3( 0,  5, 0)), Point3( 0,  5, 0)) == Point3(-1,  5, 0));
    REQUIRE(system.walk_perp_z(system.force_vector(Point3(-1,  5, 0)), Point3(-1,  5, 0)) == Point3(-2,  5, 0));
    REQUIRE(system.walk_perp_z(system.force_vector(Point3(-2,  5, 0)), Point3(-2,  5, 0)) == Point3(-3,  5, 0));
    REQUIRE(system.walk_perp_z(system.force_vector(Point3(-3,  5, 0)), Point3(-3,  5, 0)) == Point3(-4,  4, 0));
    REQUIRE(system.walk_perp_z(system.force_vector(Point3(-4,  4, 0)), Point3(-4,  4, 0)) == Point3(-5,  3, 0));    
    //REQUIRE(system.walk_perp_z(Vec3( 1,  0, 0), Point3(-5,  0, 0)) == Point3(0, 5, 0));
    //REQUIRE(system.walk_perp_z(Vec3( 0,  1, 0), Point3( 0, -5, 0)) == Point3(0, 5, 0));
    //REQUIRE(system.walk_perp_z(Vec3(-1,  0, 0), Point3( 5,  0, 0)) == Point3(0, 5, 0));
}

TEST_CASE("System::hug_target_magnitude", "[test, System]"){
    Pole pole(0, 0, 0);
    System system;
    system.poles.push_back(pole);
    // Test cardinal directions decreasing
    REQUIRE(system.hug_target_magnitude(0.1, Point3( 0, 5, 0)) == Point3( 0, 4, 0));
    REQUIRE(system.hug_target_magnitude(0.1, Point3( 0,-5, 0)) == Point3( 0,-4, 0));
    REQUIRE(system.hug_target_magnitude(0.1, Point3( 5, 0, 0)) == Point3( 4, 0, 0));
    REQUIRE(system.hug_target_magnitude(0.1, Point3(-5, 0, 0)) == Point3(-4, 0, 0));

    // Test cardinal directions increasing
    REQUIRE(system.hug_target_magnitude(0.00001, Point3( 0, 5, 0)) == Point3( 0, 6, 0));
    REQUIRE(system.hug_target_magnitude(0.00001, Point3( 0,-5, 0)) == Point3( 0,-6, 0));
    REQUIRE(system.hug_target_magnitude(0.00001, Point3( 5, 0, 0)) == Point3( 6, 0, 0));
    REQUIRE(system.hug_target_magnitude(0.00001, Point3(-5, 0, 0)) == Point3(-6, 0, 0));    

    // Trouble test cases
    system.poles[0] = Pole(64.8014, 33.0442, 1.50572);
    REQUIRE(system.hug_target_magnitude(0.03, Point3(65, 38, 2)) == Point3(65, 39, 2));     
}

TEST_CASE("System::discrete_step", "[test, System]"){
    Pole pole(64.8014, 33.0442, 1.5057);
    System system;
    system.poles.push_back(pole);
    REQUIRE(system.discrete_step(-system.force_vector(Point3(65, 38, 2)), Point3(65, 38, 2)) == Point3(65, 39, 2));
}

TEST_CASE("Vec3::to_unit", "[test, Vec3]"){
    Vec3 v(0, -0.0625, 0);
    v.to_unit();
    REQUIRE(v == Vec3(0, -1, 0));
}

TEST_CASE("Vec3::Cross", "[test, Vec3]"){
    // Unit vector cross products
    REQUIRE((Vec3(1, 0, 0) * Vec3(0, 1, 0)) == (Vec3( 0,  0,  1)));
    REQUIRE((Vec3(1, 0, 0) * Vec3(0, 0, 1)) == (Vec3( 0, -1,  0)));
    REQUIRE((Vec3(0, 1, 0) * Vec3(1, 0, 0)) == (Vec3( 0,  0, -1)));
    REQUIRE((Vec3(0, 1, 0) * Vec3(0, 0, 1)) == (Vec3( 1,  0,  0)));
    REQUIRE((Vec3(0, 0, 1) * Vec3(1, 0, 0)) == (Vec3( 0,  1,  0)));
    REQUIRE((Vec3(0, 0, 1) * Vec3(0, 1, 0)) == (Vec3(-1,  0,  0)));

    // Custom cases
    REQUIRE((Vec3(4.69768e-05, -4.7523e-05, 0) * Vec3(0, 0, 1)) == Vec3(-4.7523e-05, -4.69768e-05, 0));
}

