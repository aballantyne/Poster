#include "TextDrawer.h"

void TextDrawer::setUpImage(){
    image = Mat::zeros(imageHeight, imageWidth,CV_8UC1 );    
}

// If text is being cut off this is to blame
void TextDrawer::placeText(){
    ft2->putText(image, 
                 textData.text,        
                 Point(textData.positionX, textData.positionY),
                 textData.fontSize,                        
                 Scalar(textData.transparancy),         
                 -1,                        // thickness (-1 = filled)
                 LINE_AA,                   
                 true);                      
}

void TextDrawer::blendImages(){
    add(image, *currentImage, image);
}

void TextDrawer::findError(){
    absdiff(image, *srcImage, differenceImage);
    errorScore = (cv::sum(differenceImage)[0]); 
    // absdiff(image, *srcImage, differenceImage);
    // Mat squared;
    // multiply(differenceImage, differenceImage, squared);
    // errorScore = sum(squared)[0];
}
vector<string> split(string s, string delimiter) {
    vector<string> output; 

    auto pos = s.find(delimiter);

    while (pos != string::npos) {
        output.push_back(s.substr(0, pos));

        s.erase(0, pos + delimiter.length());

        pos = s.find(delimiter);
    }
    output.push_back(s); 
    return output;
}