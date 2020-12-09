//
//  Test.h
//  MaxFoleyRender
//
//  Created by Maxwell Foley on 12/6/20.
//  Copyright © 2020 Maxwell Foley. All rights reserved.
//

#ifndef Test_h
#define Test_h

#import "common.h"
#import "MakeDefaultScenes.h"
#import "Ray.h"
#import "Tri.h"
#import "Vector.h"

void rayTriangleIntersectionUnitTests() {
    //seems good based on these two basic ones, can do more later
    MFR::Tri t1 = MFR::Tri(MFR::Point(-5,-5,-5),MFR::Point(5,-5,-5),MFR::Point(0,5,-5));
    MFR::Ray r1 = MFR::Ray(MFR::Point(0,0,0),MFR::Vector(0,0,-1));
    float b1[3];
    float dist1;
    bool h1 = t1.intersect(r1,b1,dist1);
    std::cout << "test 1 straight in front: " << h1 << " " << "b: " << b1[0] << " " << b1[1] << " " << b1[2] << " dist: " << dist1 << std::endl;
    
    MFR::Tri t2 = MFR::Tri(MFR::Point(-5,-5,-5),MFR::Point(5,-5,-5),MFR::Point(0,5,-5));
    MFR::Ray r2 = MFR::Ray(MFR::Point(0,0,0),MFR::Vector(0,1,0));
    float b2[3];
    float dist2;
    bool h2 = t2.intersect(r2,b2,dist2);
    std::cout << "test 2 straight in front but ray goes up: " << h2 << " " << "b: " << b2[0] << " " << b2[1] << " " << b2[2] << " dist: " << dist2 << std::endl;
    
    MFR::Tri t3 = MFR::Tri(MFR::Point(-5,-5,0),MFR::Point(5,-5,0),MFR::Point(0,5,0));
    MFR::Ray r3 = MFR::Ray(MFR::Point(100,200,300),MFR::Vector(-1,-2,-3));
    float b3[3];
    float dist3;
    bool h3 = t1.intersect(r3,b3,dist3);
    std::cout << "test 3 ray comes from afar and hits origin: " << h3 << " " << "b: " << b3[0] << " " << b3[1] << " " << b3[2] << " dist: " << dist3 << std::endl;
    
    MFR::Tri t4 = MFR::Tri(MFR::Point(-5,-5,0),MFR::Point(5,-5,0),MFR::Point(0,5,0));
    MFR::Ray r4 = MFR::Ray(MFR::Point(100,300,300),MFR::Vector(-1,-2,-3));
    float b4[3];
    float dist4;
    bool h4 = t1.intersect(r4,b4,dist4);
    std::cout << "test 4 like the last one but it misses: " << h4 << " " << "b: " << b4[0] << " " << b4[1] << " " << b4[2] << " dist: " << dist4 << std::endl;
}

void rayBoxIntersectionUnitTests() {
    //all this shit seems to work as far as i can tell
    
    MFR::Box box1 = MFR::Box(MFR::Point(-10.0,-10.0,-10.0),20,20,5);
    MFR::Ray r1 = MFR::Ray(MFR::Point(0,0,0),MFR::Vector(0,0,-1));
    bool h1 = box1.intersect(r1);
    std::cout << "test 1 simple hit: " << h1 << std::endl;
    
    MFR::Box box2 = MFR::Box(MFR::Point(-10.0,-10.0,-10.0),20,20,5);
    MFR::Ray r2 = MFR::Ray(MFR::Point(0,0,0),MFR::Vector(0,-1,1));
    bool h2 = box2.intersect(r2);
    std::cout << "test 2 simple miss: " << h2 << std::endl;
    
    MFR::Box box3 = MFR::Box(MFR::Point(-30.0,-30.0,-30.0),60,60,60);
    MFR::Ray r3 = MFR::Ray(MFR::Point(0,0,0),MFR::Vector(0,-1,1));
    bool h3 = box3.intersect(r3);
    std::cout << "test 3 from tha inside: " << h3 << std::endl;
    
    MFR::Box box4 = MFR::Box(MFR::Point(-30.0,-30.0,-30.0),60,60,60);
    MFR::Ray r4 = MFR::Ray(MFR::Point(1,2,3),MFR::Vector(.4,1,-1));
    bool h4 = box4.intersect(r4);
    std::cout << "test 4 from tha inside another: " << h4 << std::endl;
    
    MFR::Box box5 = MFR::Box(MFR::Point(-1.0,-1.0,-1.0),2,2,2);
    MFR::Ray r5 = MFR::Ray(MFR::Point(100,200,300),MFR::Vector(-1,-2,-3));
    bool h5 = box5.intersect(r5);
    std::cout << "test 5 coming from an angle to hit the tiny box at the center: " << h5 << std::endl;
    
    MFR::Box box6 = MFR::Box(MFR::Point(-1.0,-1.0,-1.0),2,2,2);
    MFR::Ray r6 = MFR::Ray(MFR::Point(100,200,300),MFR::Vector(-1,-4,-3));
    bool h6 = box5.intersect(r6);
    std::cout << "test 6 similar to the last one but it should fail: " << h6 << std::endl;
    
    MFR::Box box7 = MFR::Box(MFR::Point(-1000.0,-1000.0,-1000.0),2000,2000,2000);
    MFR::Ray r7 = MFR::Ray(MFR::Point(-1000,-1005,-1010),MFR::Vector(1,-6,-11));
    bool h7 = box7.intersect(r7);
    std::cout << "test 7 barely missing a huge box: " << h7 << std::endl;
    
    MFR::Box box8 = MFR::Box(MFR::Point(-5.1,-5.1,-5.1),10.201,10.201,10.201);
    MFR::Ray r8 = MFR::Ray(MFR::Point(0, 0, 25),MFR::Vector(-0.131728, 0.0439092, -0.990313));
    bool h8 = box8.intersect(r8);
    std::cout << "test 8: " << h8 << std::endl;
    
    //Test timing of test eight
    for (int i = 0; i < 100000; i++) {
        box8.intersect(r8);
    }
}

void triangleBoundingBoxUnitTests() {
    MFR::Tri t1 = MFR::Tri(MFR::Point(-5,-5,-5),MFR::Point(5,-5,-5),MFR::Point(0,5,-5));
    MFR::Box b1 = t1.getBoundingBox();
    std::cout << "test1 " << b1 << std::endl;
    
}

void triTreeIntersectionUnitTests() {
    std::shared_ptr<MFR::Scene> scene = makeSingleCubeScene();
    scene->poseMesh();
    TriTree tt = scene->posedMesh;
    float b[3] = {0,0,0};
    float distance = 10000000;
    Ray r = Ray(Point(0,0,25),Vector(0,0,-1));
   // logLevel = 2;
    auto t1 = std::chrono::high_resolution_clock::now();
    
/*
    for(int i = 0; i < 250000; i++) {
        TriResult tr = tt.intersectRay(r, b, distance);
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>( t2 - t1 ).count();
    std::cout << "Repeated tritee test " << duration << std::endl;*/
    logLevel = 1;

   // std::cout << "It should hit " << tr.found << std::endl;

}
#endif /* Test_h */
