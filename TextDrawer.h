#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/freetype.hpp>

using namespace cv;
using namespace std; 

vector<string> split(string s, string delimiter);
struct TextData{
    string text;
    float fontSize; 
    float positionX; 
    float positionY; 
    float rotation; 
    int transparancy;

    TextData(string text, float fontSize, float positionX, float positionY, float rotation, int transparancy) 
         : text(text), fontSize(fontSize), positionX(positionX), positionY(positionY), rotation(rotation), transparancy(transparancy)   
         {}

    // Decoding a string from a line from a textfile from this line :
    // textOutput << newText.fontSize << "," << newText.positionX << "," << newText.positionY << "," << newText.rotation << "," << newText.text << "," << newText.transparancy << endl;
    TextData(string dataLine){
        vector<string> splitString = split(dataLine, ",");
        fontSize     = stof(splitString[0]);
        positionX    = stof(splitString[1]);
        positionY    = stof(splitString[2]);
        rotation = stof(splitString[3]);
        text         = splitString[4];
        transparancy = stoi(splitString[5]);
    }
    TextData() {}

     
};
class TextDrawer {
private:
    Mat image;
    Mat differenceImage; 
    Mat* srcImage;
    Mat* currentImage; 

    TextData textData; 
    int transparancy; 
    int imageWidth; 
    int imageHeight;  
    Ptr<freetype::FreeType2> ft2; 

    double errorScore;

    void setUpImage(); 
    void placeText();
    void blendImages();
    void findError(); 
    public: 
    //use this  if you are generating a new image and you need to get error
    TextDrawer(TextData textData, Mat* srcImage, Mat* currentImage, Ptr<freetype::FreeType2> ft2) : textData(textData), ft2(ft2), srcImage(srcImage), currentImage(currentImage) {
        imageHeight = srcImage->rows;
        imageWidth = srcImage->cols;
        setUpImage();
        placeText();
        blendImages();
        findError(); 
    }
    //use this if you dont need error and youre upscaling an image
    TextDrawer(TextData textData, Mat* currentImage, Ptr<freetype::FreeType2> ft2) : textData(textData), ft2(ft2), srcImage(nullptr), currentImage(currentImage) {
        imageHeight = currentImage->rows;
        imageWidth = currentImage->cols;
        setUpImage();
        placeText();
        blendImages();
    }
     Mat getImage() const{ return image; }
     Mat getDiffImage() const{ return differenceImage; }
     TextData getTextData() const{ return textData; }

     double getErrorScore() const { return errorScore; }
};
