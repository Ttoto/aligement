#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <sstream>
#include <eigen3/Eigen/Dense>

using namespace std;
using namespace cv;
using namespace Eigen;

struct CAMERA_POSE
{
    int                seq;
    Eigen::Vector3d    pos;
    Eigen::Quaterniond q;
};

std::vector<CAMERA_POSE> poses_loop1, poses_loop2;
fstream cvs_fs;






void make_pair_and_save(string filepath1, string filepath2, string filepathout)
{
    //find the nearest image in the map
    for(int l2idx=0; l2idx<poses_loop2.size()-1; l2idx++)
    {
        double nearest_distance=999;;
        int    nearest_idx=0;
        int    curr_idx=poses_loop2.at(l2idx).seq;;
        for(int l1idx=0; l1idx<poses_loop1.size()-1; l1idx++)
        {
            double distance=sqrt(pow(poses_loop2.at(l2idx).pos[0]-poses_loop1.at(l1idx).pos[0],2)+
                    pow(poses_loop2.at(l2idx).pos[1]-poses_loop1.at(l1idx).pos[1],2)+
                    pow(poses_loop2.at(l2idx).pos[2]-poses_loop1.at(l1idx).pos[2],2));
            if(distance<nearest_distance){
                nearest_distance = distance;
                nearest_idx = l1idx;
            }
        }
        int nearest_img_idx_in_loop1 = poses_loop1.at(nearest_idx).seq;

        if(nearest_distance<0.5 && nearest_img_idx_in_loop1!=0)
        {//merge img1 and img2 and save follow the name of img2
            Mat img1,img2;
            cout << "In loop2 the " << curr_idx << " matched " << nearest_img_idx_in_loop1 << " in loo1" << endl;
            img1 = imread(filepath1+to_string(nearest_img_idx_in_loop1)+".png", CV_LOAD_IMAGE_COLOR);
            img2 = imread(filepath2+to_string(curr_idx)+".png", CV_LOAD_IMAGE_COLOR);

            Mat img_merge(Size(img1.cols*2,img1.rows),img1.type(),Scalar::all(0));
            Mat matRoi = img_merge(Rect(0,0,img1.cols,img1.rows));
            img1.copyTo(matRoi);
            matRoi = img_merge(Rect(img1.cols,0,img1.cols,img1.rows));
            img2.copyTo(matRoi);
            imwrite(filepathout+to_string(curr_idx)+".png", img_merge);
        }
    }

}


void loadmap(string filepath, std::vector<CAMERA_POSE>& poses)
{
    poses.clear();
    cvs_fs.open(filepath+"map.csv", ios::in);
    vector<string> row;
    string line, word, temp;
    while (cvs_fs >> line) {
        row.clear();
        stringstream s(line);
        while (getline(s, word, ',')) {
            row.push_back(word);
        }
        cout << "idx:" << row[0] << " px:" << row[1] << " py: " << row[2] << " pz: " << row[3]
             << " qw:" << row[4] << " qx:" << row[5] << " qy: " << row[6] << " qz: " << row[7] << endl;
        CAMERA_POSE pose;
        pose.seq=stoi(row[0]);
        pose.pos[0]=stod(row[1]);
        pose.pos[1]=stod(row[2]);
        pose.pos[2]=stod(row[3]);
        pose.q.w()=stod(row[4]);
        pose.q.x()=stod(row[5]);
        pose.q.y()=stod(row[6]);
        pose.q.z()=stod(row[7]);
        poses.push_back(pose);
    }
    cvs_fs.close();
    cout  << "Finish" << endl << endl;
}


int main(int argc, char **argv)
{
    if(argc!=4)
    {
        cout << "please input loop1, loop2 and aligned result folder" << endl;
        return 0;
    }
    std::string loop1_path=argv[1];
    std::string loop2_path=argv[2];
    std::string save_path=argv[3];

    cout << "loop1 file path: " << loop1_path << endl;
    cout << "loop2 file path: " << loop2_path << endl;
    cout << "aligned result file path: " << loop2_path << endl;
    //load map
    loadmap(loop1_path,poses_loop1);
    loadmap(loop2_path,poses_loop2);

    cout << "Start Making Pairs" << endl;
    make_pair_and_save(loop1_path,loop2_path,save_path);


    return 0;
}
