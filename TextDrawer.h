#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/freetype.hpp>

using namespace cv;
using namespace std; 

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
    TextData() {}
};
class TextDrawer {
private:
    Mat image;
    Mat* srcImage;
    Mat* currentImage; 

    TextData textData; 
    int transparancy; 
    int imageWidth; 
    int imageHeight;  
    Ptr<freetype::FreeType2> ft2; 

    int errorScore;

    void setUpImage(); 
    void placeText();
    void rotate();
    void changePosition(); 
    void blendImages();
    void findError(); 

    int getAverageColorFromSorce(); 
    
public: 
    TextDrawer(TextData textData, Mat* srcImage, Mat* currentImage, Ptr<freetype::FreeType2> ft2) : textData(textData), ft2(ft2), srcImage(srcImage), currentImage(currentImage) {
        imageHeight = srcImage->rows;
        imageWidth = srcImage->cols;
        setUpImage();
        placeText();
        rotate();
        changePosition(); 
        blendImages();
        findError(); 

    }
     Mat getImage() const{ return image; }
     TextData getTextData() const{ return textData; }

     int getErrorScore() const { return errorScore; }
     float getPercentError() const { return (float)255 * (float)errorScore / (float)(imageHeight * imageHeight * imageWidth * imageWidth); }
};
