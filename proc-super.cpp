#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Kabsch.hpp"

using namespace std;


Eigen::Matrix3Xd loadLandmarks(const char *filename);

int main(int argc, char *argv[])
{
     TestFind3DAffineTransform();

    if (argc < 4)
    {
        cerr << "Usage: ./proc <face data> <landmark data> <reference landmark data>" << endl;
        return -1;
    }

    ifstream infile(argv[1]);
    string line, label;
    
    
    
    Eigen::Matrix3Xd landmarks = loadLandmarks(argv[2]);
    Eigen::Matrix3Xd refLandmarks = loadLandmarks(argv[3]);

    Eigen::Affine3d A = Find3DAffineTransform(landmarks, refLandmarks);

    cerr << A(0,0) << " " << A(0,1) << " " << A(0,2) << " " << A(0,3) << endl;
    cerr << A(1,0) << " " << A(1,1) << " " << A(1,2) << " " << A(1,3) << endl;
    cerr << A(2,0) << " " << A(2,1) << " " << A(2,2) << " " << A(2,3) << endl;
    cerr << A(3,0) << " " << A(3,1) << " " << A(3,2) << " " << A(3,3) << endl;


    glm::mat4 translation;// = procTranslate(landmarks, refLandmarks);

    glm::mat4 rotation(1.0f);
    glm::mat4 scale(1.0f);

    glm::mat4 m = scale * rotation * translation;



    // vertex list
    float v0, v1, v2;
    
    while (getline(infile, line))
    {
        istringstream iss(line);
        iss >> label;
        if (label == "v")
        {
            iss >> v0 >> v1 >> v2;
            Eigen::Vector4d v(v0, v1, v2, 1);
            v = A*v;
            cout << label << " " << v(0) << " " << v(1) << " " << v(2) << endl;
        }
        else
        {
            cout << line << endl;
        }
    }
}


Eigen::Matrix3Xd loadLandmarks(const char *filename)
{
	ifstream infile(filename);
	string line;
    vector<glm::vec3> landmarkVec;

	float v0, v1, v2;

	for (int i = 0; getline(infile, line); i++)
    {
        istringstream iss(line);
        iss >> v0 >> v1 >> v2;
        landmarkVec.push_back(glm::vec3(v0, v1, v2));
    }

    int numLandmarks = landmarkVec.size();
    Eigen::Matrix3Xd landmarks(3, numLandmarks);

    for (int i = 0; i < numLandmarks; i++)
    {
        landmarks(0, i) = landmarkVec[i][0];
        landmarks(1, i) = landmarkVec[i][1];
        landmarks(2, i) = landmarkVec[i][2];
    }
    return landmarks;
}


