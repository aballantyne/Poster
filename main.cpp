//g++ -std=c++11 *.cpp -o main `pkg-config --cflags --libs opencv4`
#include <opencv2/opencv.hpp>
#include <opencv2/freetype.hpp>
#include <cstdlib>
#include <ctime>
    #include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
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
int randomInt(int min, int max){
    return min + rand() % (max - min + 1);
}
float randomFloat(float min, float max){
    return min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(max-min)));
}
const float MIN_FONT_SIZE = 1; 
const float MAX_FONT_SIZE = 150; 

const int MIN_TRANSPARANCY = 0;
const int MAX_TRANSPARANCY = 50;
TextData createTextChildMutation(const TextData& textData){
    const float MAX_FONT_SIZE_MULTIPLIER = 1.25; 
    const float MIN_FONT_SIZE_MULTIPLIER = 0.75; 

    const float MIN_POSITION_CHANGE = -10; 
    const float MAX_POSITION_CHANGE = 10; 

    const float MIN_ROTATION_CHANGE = 0;
    const float MAX_ROTATION_CHANGE = 0;
    
    const int MIN_TRANSPARANCY_CHANGE = -5;
    const int MAX_TRANSPARANCY_CHANGE = 5;


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
TextData createNewTextData(Mat srcImage, vector<string> corpus){
    TextData output;
    output.text = corpus[randomInt(0,corpus.size()-1)];
    output.fontSize = randomFloat(MIN_FONT_SIZE, MAX_FONT_SIZE);
    output.positionX = randomFloat(0, srcImage.cols);
    output.positionY = randomFloat(0, srcImage.rows);
    output.rotation = 0;
    output.transparancy = randomInt(MIN_TRANSPARANCY, MAX_TRANSPARANCY); 
    
    return output; 
}
void loadCorpus(vector<string>& v){
     std::ifstream inputFile("src/corpus.txt"); // Replace "your_file.txt" with your file's name

    if (!inputFile.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
    }

    std::string line;
    while (std::getline(inputFile, line)) {
        v.push_back(line); 
    }
}
void createNewImage(){
    srand(time(0)); 

    Ptr<freetype::FreeType2> ft2 = freetype::createFreeType2();
    ft2->loadFontData("/System/Library/Fonts/Supplemental/Arial Bold.ttf", 0);
    
    Mat srcImage = getSourceImage();
    Mat currentImage = Mat::zeros(srcImage.rows, srcImage.cols, CV_8UC1);    

    vector<string> corpus; 

    loadCorpus(corpus);
    // TextData textData ("Test",30, 100, 0, 0, 255/2);
    // TextDrawer textDrawer = TextDrawer(textData, srcImage.rows,srcImage.cols, ft2);

    // imshow("origional", textDrawer.getImage()); 
    
    const int ITEMS_PER_GENERATION = 4000; 
    const int NUMBER_OF_GENERATIONS = 10;
    const int NUMBER_BEST_KEPT = 50; 

    const bool LOAD_CURRENT_IMAGE = true;
    const string IMAGE_PATH = "output.png"; 

    ofstream textOutput;

    if (LOAD_CURRENT_IMAGE){
        currentImage = imread(samples::findFile(IMAGE_PATH), CV_8UC1);
        textOutput.open("outputText.txt", std::ios_base::app);
    }else {
        textOutput.open("outputText.txt");
        textOutput << "[origional image resolution]:" <<  srcImage.rows << "x" << srcImage.cols << endl; 

    }

    int t = 0;
    while(true){
        t++;
        vector<TextDrawer> textDrawers;

        for (int i = 0; i < ITEMS_PER_GENERATION; i++){
            textDrawers.push_back(TextDrawer(createNewTextData(srcImage, corpus), &srcImage, &currentImage, ft2));
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
            cout << "Sorted; Best Error: " << std::fixed << std::setprecision(0)  << textDrawers[0].getErrorScore() << ", " << endl;
        }
        cout << ((float)textDrawers[0].getErrorScore() / ( srcImage.rows *  srcImage.cols) ) << endl; 
        TextData newText = textDrawers[0].getTextData(); 
        textOutput << newText.fontSize << "," << newText.positionX << "," << newText.positionY << "," << newText.rotation << "," << newText.text << "," << newText.transparancy << endl;

        currentImage = textDrawers[0].getImage(); 
        const string usedText = textDrawers[0].getTextData().text;

        corpus.erase(
            std::remove(corpus.begin(), corpus.end(), usedText),
            corpus.end()    
        );
        if (corpus.size() == 0){
            loadCorpus(corpus);
        }
        if (t % 50 == 0){
            imwrite("output.png", currentImage);
            cout << "Updated the image" << endl;
        }
        cout << t << endl; 
    }
}   

void upscaleImage(){
    const double SIZE_MULTIPLIER = 8.0;
    const bool CENTER_ORIGIN = true; 
    Ptr<freetype::FreeType2> ft2 = freetype::createFreeType2();
    ft2->loadFontData("/System/Library/Fonts/Supplemental/Arial Bold.ttf", 0);

    std::ifstream inputFile("outputText.txt"); // Replace "your_file.txt" with your file's name

    if (!inputFile.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
    }

    string headerInfo; 
    std::getline(inputFile, headerInfo);
    headerInfo = split(headerInfo, ":")[1]; // removes "[origional image resolution]:"

 
    int cols = stof(split(headerInfo,"x")[1]) * SIZE_MULTIPLIER;
    int rows = stof(split(headerInfo,"x")[0]) * SIZE_MULTIPLIER;

    Mat image = Mat::zeros(rows, cols, CV_8UC1);    
    
    int i = 0; 
    std::string line;
    while (std::getline(inputFile, line)) {
        TextData newTextData = TextData(line);
        newTextData.positionX *= SIZE_MULTIPLIER;
        newTextData.positionY *= SIZE_MULTIPLIER;
        newTextData.fontSize *= SIZE_MULTIPLIER;
        
        if(CENTER_ORIGIN){
            newTextData.positionX +=  cols/2.0;
            newTextData.positionY += rows/2.0 ;
        }
        image = TextDrawer(newTextData, &image, ft2).getImage();

        cout << i++ << endl; 
        
    }
    imwrite("upscaledOutput.png", image);

}
int main(){
    // createNewImage();
    upscaleImage();
    return 0;
}