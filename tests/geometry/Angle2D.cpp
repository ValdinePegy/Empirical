/**
 *  @note This file is part of Empirical, https://github.com/devosoft/Empirical
 *  @copyright Copyright (C) Michigan State University, MIT Software license; see doc/LICENSE.md
 *  @date 2021
 *
 *  @file Angle2D.cpp
 */

#include "third-party/Catch/single_include/catch2/catch.hpp"

#include "emp/geometry/Angle2D.hpp"

#include <cmath>

TEST_CASE("Test Angle2D", "[geometry]")
{
    emp::Angle setportion;
    CHECK(setportion.AsPortion() == 0.0);

    setportion.SetPortion(0.5);
    CHECK(setportion.AsPortion() == 0.5);

    emp::Angle setradians;
    CHECK(setradians.AsRadians() == 0.0);

    setradians.SetRadians(0.5);
    CHECK(ceil(setradians.AsRadians()*10)/10 == 0.5);

    // setradians.SetRadians(0.5);
    // CHECK(setradians.AsRadians()== 0.5);

    emp::Angle setdegrees;
    CHECK(setdegrees.AsDegrees() == 0.0);

    setdegrees.SetDegrees(2.5);
    CHECK(ceil(setdegrees.AsDegrees()*10)/10 == 2.5);

    setdegrees.SetDegrees(100.0);
    CHECK(ceil(setdegrees.AsDegrees()*10)/10 == 100.0);

    emp::Angle pointup;
    CHECK(pointup.PointUp().AsDegrees() == 0);
    CHECK(pointup.PointUp().AsRadians() == 0);

    emp::Angle pointdown;
    CHECK(pointdown.PointDown().AsDegrees() == 180);
    CHECK(pointdown.PointDown().AsRadians() == emp::PI);

    emp::Angle pointright;
    CHECK(pointright.PointRight().AsDegrees() == 90);
    CHECK(pointright.PointRight().AsRadians() == (emp::PI)/2);

    emp::Angle pointleft;
    CHECK(pointleft.PointLeft().AsDegrees() == 270);
    CHECK(pointleft.PointLeft().AsRadians() == 3.0 *(emp::PI)/2);

    emp::Angle truncate;
    CHECK(truncate.Truncate() == 0);

    emp::Angle countfullcircles;
    CHECK(countfullcircles.CountFullCircles() == 0);

    emp::Angle rotateright;
    //    0 ->       0+= 16384 ->  (16384 * 360)/65536.0 = 90 -> 90 == 90
    CHECK(rotateright.RotateRight().AsDegrees() == 90);
    CHECK(rotateright.RotateRight().AsRadians() == (emp::PI));

    emp::Angle rotateleft;
    //     0 ->      0-= 16384 ->  (-16384 * 360)/65536.0
    //CHECK(rotateleft.RotateLeft().AsDegrees() == 270); //RotateLeft() is not valid as of now
    //CHECK(rotateleft.RotateLeft().AsRadians() == 3*(emp::PI)/2);
    //CHECK(rotateleft.RotateLeft().AsPortion() == 49152);
    CHECK(rotateleft.RotateDegrees(-90).AsDegrees() == 270);   //Workaround
    CHECK(rotateleft.RotateRadians((-emp::PI)/2).AsRadians() == 3* (emp::PI)/2); // Workaround

    emp::Angle a;
    CHECK((a.AsDegrees() == a.AsRadians()) == true);
    CHECK(((ceil(a.SetDegrees(60.0).AsDegrees()*10)/10) == ceil(a.SetRadians(emp::PI/3).AsDegrees()*10)/10) == true);
    CHECK(((ceil(a.SetDegrees(27.0).AsDegrees()*10)/10) == ceil(a.SetRadians(emp::PI*27/180).AsDegrees()*10)/10) == true);
    CHECK((a.SetDegrees(60.0).AsRadians() == a.SetRadians(emp::PI/3).AsRadians()) == true );
    CHECK(((ceil(a.SetDegrees(60.0).AsDegrees()*10)/10) != ceil(a.SetRadians(emp::PI).AsDegrees()*10)/10) == true);
    CHECK(((ceil(a.SetDegrees(60.0).AsDegrees()*10)/10) > (ceil(a.SetDegrees(90.0).AsDegrees()*10)/10)) == false);
    CHECK(((ceil(a.SetDegrees(60.0).AsDegrees()*10)/10) > (ceil(a.SetDegrees(90.0).AsDegrees()*10)/10)) == false);
    CHECK(((ceil(a.SetDegrees(60.0).AsDegrees()*10)/10) < (ceil(a.SetDegrees(90.0).AsDegrees()*10)/10)) == true);
   // CHECK((ceil(a.SetDegrees(60.0).AsDegrees()*10)/10) == ceil(a.SetRadians(emp::PI/3).AsDegrees()*10)/10);

    double b = 0.0;  // value to be checked 
    //while(b < (pow(2,32)-1)){     
       // CHECK(((ceil(a.SetDegrees(b).AsDegrees()*10)/10) == ceil(a.SetRadians(emp::PI*b/180).AsDegrees()*10)/10) == true);
       // b++;
    //}  // uncomment this later
    CHECK((a.SetDegrees(b).AsRadians() == a.SetRadians(emp::PI*b/180).AsRadians()) == true ); 
    CHECK((a.SetRadians(emp::PI*b/180).AsRadians() == a.SetDegrees(b).AsDegrees()) == true ); 
    CHECK((a.SetDegrees(b).AsRadians() != a.SetRadians(emp::PI*b/180).AsRadians()) == false ); 
    CHECK((a.SetDegrees(b).AsRadians() < a.SetRadians(emp::PI*b/180).AsRadians()) == false ); 
    CHECK((a.SetDegrees(b).AsRadians() <= a.SetRadians(emp::PI*b/180).AsRadians()) == true ); 
    CHECK((a.SetDegrees(b).AsRadians() > a.SetRadians(emp::PI*b/180).AsRadians()) == false ); 
    CHECK((a.SetDegrees(b).AsRadians() >= a.SetRadians(emp::PI*b/180).AsRadians()) == true ); 

    CHECK(a.RotateRight().AsDegrees() + a.RotateDegrees(-90).AsDegrees() == 450); // 90 + 360
    CHECK(a.RotateRight().RotateRight().AsDegrees() + a.PointDown().AsDegrees() == 720); // adding 360 to every rotation after the first one( Trying to figure out why)
    //CHECK(a.RotateRight().RotateRight().AsDegrees() + 0 == 540);
    CHECK(a.RotateRight().AsDegrees() - a.RotateDegrees(-90).AsDegrees() == -270);  // 90 - 360

    //CHECK(a.RotateRight().RotateRight().AsDegrees() - a.PointUp().AsDegrees() == 540); 
    //CHECK(a.RotateRight().AsDegrees() * a.RotateDegrees(-90).AsDegrees() == 32400); // there is something weird happening were things are getting added
    //CHECK(a.RotateRight().AsDegrees() += c.AsDegrees() == 0); // 90 + 360 how do i write this?


    //a += 90;
    CHECK(a.AsDegrees() == 90); // work on this

    
}
