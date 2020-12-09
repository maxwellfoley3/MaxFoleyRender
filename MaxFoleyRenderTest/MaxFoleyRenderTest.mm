//
//  MaxFoleyRenderTest.m
//  MaxFoleyRenderTest
//
//  Created by Maxwell Foley on 12/6/20.
//  Copyright © 2020 Maxwell Foley. All rights reserved.
//

#import <XCTest/XCTest.h>
#include "Ray.h"
#include "Tri.h"

@interface MaxFoleyRenderTest : XCTestCase

@end

@implementation MaxFoleyRenderTest

- (void)setUp {
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
}

- (void)testExample {
    // This is an example of a functional test case.
    // Use XCTAssert and related functions to verify your tests produce the correct results.
}

- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}

- (void)testRayTriangleIntersection {
    MFR::Tri t1 = MFR::Tri(MFR::Point(-5,-5,-5),MFR::Point(5,-5,-5),MFR::Point(0,5,-5));
    MFR::Ray r1 = MFR::Ray(MFR::Point(0,0,0),MFR::Vector(0,0,-1));
    float b1[3];
    float dist1;
    bool h1 = t1.intersect(r1,b1,dist1);
    XCTAssert(h1 == true)
}

@end
