//
//  TriTree.h
//  MaxFoleyRender
//
//  Created by Maxwell Foley on 2/13/18.
//  Copyright © 2018 Maxwell Foley. All rights reserved.
//

#ifndef TriTree_h
#define TriTree_h

#include <vector>
#include "common.h"

namespace MFR {
	class KDNode {
		public:
			KDNode * left;
			KDNode * right;
			std::vector<Tri> tris;
            uint trisCount;
			Box boundingBox;
			KDNode(){}
			~KDNode(){
				delete left;
				delete right;
			}
	};
    
    class TriResult {
        public:
            Tri tri;
            bool found;
            TriResult() {
                tri = Tri();
                found = false;
            }
            TriResult(Tri t) {
                tri = t;
                found = true;
            }
    };

	class TriTree {
        public:
            int nodeCount;
            int nodesTouched;
            int mostRecentNodesTouched;

		private:
			KDNode * root;
            int totalNodesTouched;
            int timesRun;
		
			KDNode * build(std::vector<Tri> & tris, int depth) {
				KDNode * node = new KDNode();
                node->trisCount = uint(tris.size());

				if(tris.size() == 0) {
					return node;
				}
				node->boundingBox = tris[0].getBoundingBox();
				//expand bounding box
				for(int i = 1; i < tris.size(); i++)
				{
					node->boundingBox.expand(tris[i].getBoundingBox());
				}
				
				//if we have less than 5 triangles here stop subdividing, very arbitrary and maybe needs changing TODO
				if(tris.size() <= 5) {
                    node->tris = tris;
					node->left = NULL;
					node->right = NULL;
					return node;
				}
	
				Point midpoint = Point(0,0,0);
				for(int i = 0; i < tris.size(); i++)
				{
					midpoint = midpoint + (tris[i].getMidpoint()/tris.size());
				}
				
				std::vector<Tri> leftTris;
				std::vector<Tri> rightTris;
				
				//alternate x y and z axis every time we go another level deeper
                int axis = node->boundingBox.longest_axis();
				bool rightSide = true;

				for(int i = 0; i < tris.size(); i++)
				{
					switch(axis) {
						case 0:
							rightSide = midpoint.x() > tris[i].getMidpoint().x(); break;
						case 1:
							rightSide = midpoint.y() > tris[i].getMidpoint().y(); break;
						case 2:
							rightSide = midpoint.z() > tris[i].getMidpoint().z(); break;
					}
					
					if(rightSide) {
						rightTris.push_back(tris[i]);
					}
					else {
						leftTris.push_back(tris[i]);
					}
				}
				
				//idk why i need this but worried itll break without it
				if(leftTris.size() == 0 && rightTris.size() > 0) leftTris = rightTris;
				if(rightTris.size() == 0 && leftTris.size() > 0) rightTris = leftTris;
				
				node->left = build(leftTris, depth+1);
				node->right = build(rightTris, depth+1);
				return node;
			}
		
			TriResult irHelper(KDNode * node, Ray ray, float b[3], float& dist, int depth)
			{
                if(depth == 0) {
                    nodesTouched = 0;
                }
                
                nodesTouched++;
                
                auto t1 = std::chrono::high_resolution_clock::now();
                TriResult toReturn = TriResult();
                
				if(node->boundingBox.intersect(ray))
				{
					//if we are not at a leaf, recurse
					if(node->left != NULL || node->right != NULL)
					{
						TriResult hitLeft = irHelper(node->left, ray, b, dist, depth+1);
						TriResult hitRight = irHelper(node->right, ray, b, dist, depth+1);
                        
						if(hitLeft.found) { toReturn = hitLeft; }
						else if(hitRight.found) { toReturn = hitRight; }
						/*else{
							toReturn = NULL;
						}*/
					}
					else
					{
						//we are at a leaf, now just iterate through triangles like normal
						for(int i = 0; i < node->tris.size(); i++)
						{
							bool hit = node->tris[i].intersect(ray,b,dist);
                            if(hit) toReturn = TriResult(node->tris[i]);
						}
					}
				}
                
                auto t2 = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
                if(logLevel >= 2) {
                    for(int i = 0; i < depth; i++) {
                        std::cout << "-";
                    }
                    
                    std::cout << "Duration: " << duration << " Tri count: " << node->trisCount << " " << node->tris.size() << " Tri found: " << (toReturn.found) <<
                    " Nodes touched: " << nodesTouched << std::endl ;
                }
                
                if(depth == 0 && toReturn.found) {
                    totalNodesTouched += nodesTouched;
                    mostRecentNodesTouched = nodesTouched;
                    nodesTouched = 0;
                    timesRun++;
                }
                
				//if the ray doesnt intersect the bounding box
				return toReturn;
				
			}
			std::string preorderPrint(KDNode * node, int depth) const
			{
				//if node is null or empty, we hit bottom, dont print anything
				if(!node || node->trisCount == 0)
				{
					return "";
				}
				
				//otherwise form a string to represent this node
				std::string str;
				
				for(int i = 0; i < depth; i++)
				{
					str.append("-");
				}
				
				str.append(std::to_string(node->trisCount));
				str.append(" tris");
				
				str.append(", box size: ");
				str.append(std::to_string(node->boundingBox.width));
				str.append(" x ");
				str.append(std::to_string(node->boundingBox.height));
				str.append(" x ");
				str.append(std::to_string(node->boundingBox.depth));
				str.append(" origin: ");
				str.append(std::to_string(node->boundingBox.origin.x()));
				str.append(", ");
				str.append(std::to_string(node->boundingBox.origin.y()));
				str.append(", ");
				str.append(std::to_string(node->boundingBox.origin.z()));
				str.append("\n");
				
				//TEMPORARY:: check to see if this leaf has the triangle we are looking for
				/*
				for(std::vector<std::shared_ptr<MFR::Tri>>::iterator it = node->tris.begin(); it != node->tris.end(); ++it) {
					if((*it)->points[0].x() == -5 &&
					(*it)->points[0].y() == 5 &&
					(*it)->points[0].z() == -4.9 &&
					(*it)->points[1].x() == -5 &&
					(*it)->points[1].y() == -5 &&
					(*it)->points[1].z() == -4.9 &&
					(*it)->points[2].x() == 5 &&
					(*it)->points[2].y() == -5 &&
					(*it)->points[2].z() == -4.9)
					{
						str.append("THIS IS THE NODE WHERE OUR TRIANGLE IS ^^^ \n");
					}
				}*/
				
				//if it is a leaf, print the triangles
				if(node->left == NULL && node->right == NULL)
				{

					for(std::vector<MFR::Tri>::iterator it = node->tris.begin(); it != node->tris.end(); ++it) {
						for(int i = 0; i < depth+1; i++)
						{
							str.append("-");
						}
					
						std::stringstream ss;
						ss << *it;char  c[100];
						ss.get(c,100,'@');
						str.append(c);
						str.append(")\n");
					}
					
				}
				
				//then inorder print
				std::string first = preorderPrint(node->left, depth+1);
				str.append(first);
				std::string last = preorderPrint(node->right, depth+1);
				str.append(last);
				return str;
			}
		
		public:
			TriTree() {
                nodeCount = 0;
                nodesTouched = 0;
                mostRecentNodesTouched = 0;
                totalNodesTouched = 0;
                timesRun = 0;
			}
		
			TriTree(std::vector<Tri> tris)
			{
                nodeCount = tris.size();
                nodesTouched = 0;
                mostRecentNodesTouched = 0;
                totalNodesTouched = 0;
                timesRun = 0;
				root = build(tris,0);
			}
		
			TriResult intersectRay(Ray ray, float * b, float & dist)
			{
				return irHelper(root,ray,b,dist,0);
			}
		
			/*std::vector<Tri> getTriVector()
			{
				return root->tris;
			}*/
        
            double averageNodesTouched()
            {
                return double(totalNodesTouched)/double(timesRun);
            }
		
			friend std::ostream& operator<<(std::ostream &strm, const TriTree &tt) {
				return strm << tt.preorderPrint(tt.root,1);
			}
		
	};
}

#endif /* TriTree_h */

// https://en.wikipedia.org/wiki/K-d_tree
// http://tuprints.ulb.tu-darmstadt.de/893/2/Adamson_Kapitel5-7.pdf
// http://www.cs.utah.edu/~reinhard/papers/egwr2k.pdf
// https://blog.frogslayer.com/kd-trees-for-faster-ray-tracing-with-triangles/ this is the one
