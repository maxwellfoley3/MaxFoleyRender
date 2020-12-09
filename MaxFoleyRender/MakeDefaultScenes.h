//
//  MakeDefaultScenes.h
//  MaxFoleyRender
//
//  Created by Maxwell Foley on 12/6/20.
//  Copyright © 2020 Maxwell Foley. All rights reserved.
//

#ifndef MakeDefaultScenes_h
#define MakeDefaultScenes_h

#import "Color.h"
#import "Light.h"
#import "Material.h"
#import "Vector.h"
#import "SceneObject.h"

//testing scene
std::shared_ptr<MFR::Scene> makeSingleCubeScene() {
    std::shared_ptr<MFR::Scene> scene = std::make_shared<MFR::Scene>();
    std::shared_ptr<MFR::Material> greenMaterial = std::make_shared<MFR::Material>(MFR::Color(0.0,1.0,0.0));
    std::shared_ptr<MFR::Material> whiteMaterial = std::make_shared<MFR::Material>(MFR::Color(1.0,1.0,1.0));

    std::shared_ptr<MFR::Material> redMaterial = std::make_shared<MFR::Material>(MFR::Color(1.0,0.0,0.0));
    std::shared_ptr<MFR::SceneObject> cube = std::make_shared<MFR::SceneObject>("cube2.obj");
    cube->setMaterialOnAll(redMaterial);
    cube->scale = MFR::Point(1.0,4.0,1.0);
    cube->position = MFR::Point(0.0,0.0,-8.0);
    cube->rotation = MFR::Point(45.0,45.0,0);
    scene->objects.push_back(cube);
    
    std::shared_ptr<MFR::Material> blueMaterial = std::make_shared<MFR::Material>(MFR::Color(0.0,0.0,1.0));
    std::shared_ptr<MFR::SceneObject> cube2 = std::make_shared<MFR::SceneObject>("cube2.obj");
    cube2->setMaterialOnAll(blueMaterial);
    cube2->scale = MFR::Point(4.0,.5,.5);
    cube2->position = MFR::Point(0.0,0.0,4.0);
    cube2->rotation = MFR::Point(-45.0,45.0,-15.0);
    scene->objects.push_back(cube2);

    std::shared_ptr<MFR::Camera> camera = std::make_shared<MFR::Camera>();
    camera->position = MFR::Point(0,0,25);
    camera->focalLength = -0.1;
    camera->fov = .436332;
    scene->camera = camera;
    
    /*
     std::shared_ptr<MFR::Light> light = std::make_shared<MFR::Light>();
     light->position = MFR::Point(0,5.0,0);
     scene->lights.push_back(light);*/
    
    std::shared_ptr<MFR::Light> light = std::make_shared<MFR::Light>();
    light->position = MFR::Point(1.0,3.0,-2.0);
    light->shadowsEnabled = false;
    light->strength = 10.0;
    scene->lights.push_back(light);
    
    std::shared_ptr<MFR::Light> light3 = std::make_shared<MFR::Light>();
    light3->position = MFR::Point(10000000.0,-7000000000.0,-2.0);
    light3->shadowsEnabled = false;
    light3->strength = 1.0;
    //scene->lights.push_back(light3);

    std::shared_ptr<MFR::Light> light2 = std::make_shared<MFR::Light>();
    light2->position = MFR::Point(0.0,2.0,0.0);
    light2->shadowsEnabled = false;
    light2->strength = 1.0;
    
    return scene;
    
}

//there is not yet any way to import scenes or a UI to create them, so I define the scene I want rendered in this function
std::shared_ptr<MFR::Scene> makeCornellBoxScene() {
    std::shared_ptr<MFR::Scene> scene = std::make_shared<MFR::Scene>();
    
    std::shared_ptr<MFR::Material> greenMaterial = std::make_shared<MFR::Material>(MFR::Color(0.0,1.0,0.0));
    std::shared_ptr<MFR::Material> redMaterial = std::make_shared<MFR::Material>(MFR::Color(1.0,0.0,0.0));
    
    
    //left wall is red
    std::shared_ptr<MFR::SceneObject> leftWall = std::make_shared<MFR::SceneObject>("cube2.obj");
    leftWall->setMaterialOnAll(redMaterial);
    leftWall->scale = MFR::Point(0.2,10.0,10.0);
    leftWall->position = MFR::Point(-5.0,0.0,0.0);
    leftWall->rotation = MFR::Point(90.0,0.0,0.0);
    scene->objects.push_back(leftWall);
    
    //right wall is green
    std::shared_ptr<MFR::SceneObject> rightWall = std::make_shared<MFR::SceneObject>("cube2.obj");
    rightWall->setMaterialOnAll(greenMaterial);
    rightWall->scale = MFR::Point(0.2,10.0,10.0);
    rightWall->position = MFR::Point(5.0,0.0,0.0);
    rightWall->rotation = MFR::Point(90.0,0.0,0.0);
    scene->objects.push_back(rightWall);
    
    //top wall
    std::shared_ptr<MFR::Material> whiteMaterial = std::make_shared<MFR::Material>(MFR::Color(1.0,1.0,1.0));
    std::shared_ptr<MFR::SceneObject> topWall = std::make_shared<MFR::SceneObject>("cube2.obj");
    topWall->setMaterialOnAll(whiteMaterial);
    topWall->scale = MFR::Point(0.2,10.0,10.0);
    topWall->position = MFR::Point(0.0,5.0,0.0);
    topWall->rotation = MFR::Point(0.0,0.0,90.0);
    scene->objects.push_back(topWall);
    
    //bottom wall aka floor
    std::shared_ptr<MFR::SceneObject> bottomWall = std::make_shared<MFR::SceneObject>("cube2.obj");
    bottomWall->setMaterialOnAll(whiteMaterial);
    bottomWall->scale = MFR::Point(0.2,10.0,10.0);
    bottomWall->position = MFR::Point(0.0,-5.0,0.0);
    bottomWall->rotation = MFR::Point(0.0,0.0,90.0);
    scene->objects.push_back(bottomWall);
    
    //back wall
    std::shared_ptr<MFR::SceneObject> backWall = std::make_shared<MFR::SceneObject>("cube2.obj");
    backWall->setMaterialOnAll(whiteMaterial);
    backWall->scale = MFR::Point(0.2,10.0,10.0);
    backWall->position = MFR::Point(0.0,0.0,-5.0);
    backWall->rotation = MFR::Point(0.0,90.0,0.0);
    scene->objects.push_back(backWall);
    
    //front cube
    std::shared_ptr<MFR::SceneObject> frontCube = std::make_shared<MFR::SceneObject>("cube2.obj");
    frontCube->setMaterialOnAll(greenMaterial);
    frontCube->scale = MFR::Point(2.0,2.0,2.0);
    frontCube->position = MFR::Point(.2,-4.0,.2);
    frontCube->rotation = MFR::Point(0.0,30.0,0.0);
    scene->objects.push_back(frontCube);
    
    //back cube
    std::shared_ptr<MFR::SceneObject> backCube = std::make_shared<MFR::SceneObject>("cube2.obj");
    backCube->setMaterialOnAll(redMaterial);
    backCube->scale = MFR::Point(2.0,4.0,1.0);
    backCube->position = MFR::Point(-2.0,-3.0,-2.0);
    backCube->rotation = MFR::Point(0.0,16.0,0.0);
    scene->objects.push_back(backCube);
    
    std::shared_ptr<MFR::Camera> camera = std::make_shared<MFR::Camera>();
    camera->position = MFR::Point(0,0,25);
    camera->focalLength = -0.1;
    camera->fov = .436332;
    scene->camera = camera;
    
    
    std::shared_ptr<MFR::Light> light = std::make_shared<MFR::Light>();
    light->position = MFR::Point(0,5.0,0);
    light->strength = 20.0;
    scene->lights.push_back(light);
    
    std::shared_ptr<MFR::Light> light4 = std::make_shared<MFR::Light>();
    light4->position = MFR::Point(5.0,0,0);
    light4->strength = 20.0;
    scene->lights.push_back(light4);
    
    
     
     std::shared_ptr<MFR::Light> light2 = std::make_shared<MFR::Light>();
     light2->position = MFR::Point(1.0,3.0,-2.0);
     light2->shadowsEnabled = true;
     light2->strength = 2.0;
 //    scene->lights.push_back(light2);
     
     
     std::shared_ptr<MFR::Light> light3 = std::make_shared<MFR::Light>();
     light3->position = MFR::Point(0.0,2.0,0.0);
     light3->shadowsEnabled = true;
     light3->strength = 2.0;
 //    scene->lights.push_back(light3);
    
    return scene;
}


#endif /* MakeDefaultScenes_h */
