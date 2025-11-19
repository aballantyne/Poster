//g++ -std=c++11 *.cpp -o main `pkg-config --cflags --libs opencv4`
#include <opencv2/opencv.hpp>
#include <opencv2/freetype.hpp>
#include <cstdlib>
#include <ctime>
    #include <algorithm>

#include "TextDrawer.h"
using namespace cv;

Mat getSourceImage(){
    Mat color = imread("src/sourceImage.jpg");
    Mat gray;
    cvtColor(color, gray, COLOR_BGR2GRAY);
    return gray;
}
void blendImages(const Mat& img1, Mat& img2, Mat& output) {
    for(int x = 0; x < img1.cols; x++){
        for (int y = 0; y < img1.rows; y++){
            output.at<uchar>(y,x) = min(img1.at<uchar>(y,x) + img2.at<uchar>(y,x),255);
        }
    }
}
int findError(const Mat& current, const Mat& target){
    int totalError = 0; 
    for(int x = 0; x < target.cols; x++){
        for (int y = 0; y < target.rows; y++){
            totalError += ((int)target.at<uchar>(y,x) - (int)current.at<uchar>(y,x)) *  ((int)target.at<uchar>(y,x) - (int)current.at<uchar>(y,x));
        }
    }
    return totalError;
}
int randomInt(int min, int max){
    return rand() % (max - min + 1);
}
float randomFloat(float min, float max){
    return min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(max-min)));
}
const float MIN_FONT_SIZE = 1; 
const float MAX_FONT_SIZE = 300; 

const int MIN_TRANSPARANCY = 0;
const int MAX_TRANSPARANCY = 50;
TextData createTextChildMutation(const TextData& textData){
    const float MAX_FONT_SIZE_MULTIPLIER = 1.25; 
    const float MIN_FONT_SIZE_MULTIPLIER = 0.75; 

    const float MIN_POSITION_CHANGE = -20; 
    const float MAX_POSITION_CHANGE = 20; 

    const float MIN_ROTATION_CHANGE = 0;
    const float MAX_ROTATION_CHANGE = 0;
    
    const int MIN_TRANSPARANCY_CHANGE = -10;
    const int MAX_TRANSPARANCY_CHANGE = 10;


    float newFontSize = textData.fontSize * randomFloat(MIN_FONT_SIZE_MULTIPLIER, MAX_FONT_SIZE_MULTIPLIER);

    float newPositionX = textData.positionX + randomInt(MIN_POSITION_CHANGE, MAX_POSITION_CHANGE);
    float newPositionY = textData.positionY + randomInt(MIN_POSITION_CHANGE, MAX_POSITION_CHANGE);
    
    float newRotation = textData.rotation +  randomFloat(MIN_ROTATION_CHANGE, MAX_ROTATION_CHANGE);

    int newTransparancy = textData.transparancy + randomInt(MIN_TRANSPARANCY_CHANGE, MAX_TRANSPARANCY_CHANGE);

    // Set the mins and maxes
    newFontSize = max(newFontSize, MIN_FONT_SIZE);
    newFontSize = min(newFontSize, MAX_FONT_SIZE);
    
    newTransparancy = max(newTransparancy, MIN_TRANSPARANCY);
    newTransparancy = min(newTransparancy, MAX_TRANSPARANCY);

    return TextData(textData.text, newFontSize, newPositionX,newPositionY, newRotation, newTransparancy); 
}
TextData createNewTextData(Mat srcImage){
    TextData output;
    output.text = "test";
    output.fontSize = randomFloat(MIN_FONT_SIZE, MAX_FONT_SIZE);
    output.positionX = randomFloat(-srcImage.cols/2, srcImage.cols/2);
    output.positionY = randomFloat(-srcImage.rows/2, srcImage.rows/2);
    output.rotation = 0;
    output.transparancy = randomInt(MIN_TRANSPARANCY, MAX_TRANSPARANCY); 
    
    return output; 
}
int main(){
    srand(time(0)); 

    Ptr<freetype::FreeType2> ft2 = freetype::createFreeType2();
    ft2->loadFontData("/System/Library/Fonts/Supplemental/Arial Bold.ttf", 0);
    
    Mat srcImage = getSourceImage();
    Mat currentImage = Mat::zeros(srcImage.rows, srcImage.cols, CV_8UC1);    

    // TextData textData ("Test",30, 100, 0, 0, 255/2);
    // TextDrawer textDrawer = TextDrawer(textData, srcImage.rows,srcImage.cols, ft2);

    // imshow("origional", textDrawer.getImage()); 

    const int ITEMS_PER_GENERATION = 3000; 
    const int NUMBER_OF_GENERATIONS = 10; 
    const int NUMBER_BEST_KEPT = 500; 

    bool LOAD_CURRENT_IMAGE = true;
    string IMAGE_PATH = "output.png"; 

    if (LOAD_CURRENT_IMAGE){
        currentImage = imread(samples::findFile(IMAGE_PATH), CV_8UC1);
    }
    int t = 0;
    while(true){
        t++;
        vector<TextDrawer> textDrawers;

        for (int i = 0; i < ITEMS_PER_GENERATION; i++){
            textDrawers.push_back(TextDrawer(createNewTextData(srcImage), &srcImage, &currentImage, ft2));
        }
        cout << "Created" << endl;
        sort(textDrawers.begin(), textDrawers.end(), [](const TextDrawer &a, const TextDrawer &b) { return a.getErrorScore() < b.getErrorScore(); });
        cout << "Sorted; Best Error: " << textDrawers[0].getErrorScore()  << endl;


        for (int i = 0; i < NUMBER_OF_GENERATIONS; i++){
            textDrawers.erase(textDrawers.begin() + NUMBER_BEST_KEPT, textDrawers.end());
            for (int drawerIndex = 0; drawerIndex < NUMBER_BEST_KEPT; drawerIndex++){
                for (int j = 0; j < ITEMS_PER_GENERATION/NUMBER_BEST_KEPT; j++){
                    TextData newTextData = createTextChildMutation(textDrawers[drawerIndex].getTextData());
                    textDrawers.push_back(TextDrawer(newTextData, &srcImage, &currentImage, ft2));
                }
            }
            cout << "New Generation " << i << endl;
            sort(textDrawers.begin(), textDrawers.end(), [](const TextDrawer &a, const TextDrawer &b) { return a.getErrorScore() < b.getErrorScore(); });
            cout << "Sorted; Best Error: " << textDrawers[0].getErrorScore() << ", " <<  textDrawers[0].getPercentError() << endl;
        }
        currentImage = textDrawers[0].getImage(); 
        imwrite("output.png", currentImage);
        cout << t << endl; 
    }
    return 0;
}   
