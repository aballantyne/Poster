#include "TextDrawer.h"
#include "textDrawer.h"

// TextDrawer::TextDrawer(string text, float fontSize, float positionX, float positionY, float rotation, int imageWidth, int imageHight, Ptr<freetype::FreeType2> ft2){
//     setUpImage();
//     placeText();

//     imshow(image, "Test"); 
// }

void TextDrawer::setUpImage(){
    image = Mat::zeros(imageHeight, imageWidth, CV_8UC1);    
}

// If text is being cut off this is to blame
void TextDrawer::placeText(){
    ft2->putText(image, 
                 textData.text,        // text
                 Point(image.cols/2.0, image.rows/2.0),
                 textData.fontSize,                        // font height in pixels
                 Scalar(textData.transparancy),         
                 -1,                        // thickness (-1 = filled)
                 LINE_AA,                   
                 true);                      
}

void TextDrawer::rotate(){
    Point2f center(image.cols/2.0, image.rows/2.0); // rotate around center
    double scale = 1.0; // scale factor
    Mat rotMat = getRotationMatrix2D(center, textData.rotation, scale);

    warpAffine(image, image, rotMat, image.size());
}
void TextDrawer::changePosition(){
    Mat translationMat = (Mat_<double>(2,3) << 1, 0, textData.positionX,
                                               0, 1, -textData.positionY);
    warpAffine(image, image, translationMat, image.size());
}
void TextDrawer::blendImages(){
    add(image, *currentImage, image);
}

void TextDrawer::findError(){
    Mat diff;
    absdiff(image, *srcImage, diff);
    errorScore = static_cast<int>(cv::sum(diff)[0]); 
}

int TextDrawer::getAverageColorFromSource() {
    Mat imgF, srcF;
    image.convertTo(imgF, CV_32F, 1.0/255.0);
    srcImage->convertTo(srcF, CV_32F, 1.0/255.0);

    Mat averageColorMask;
    multiply(imgF, srcF, averageColorMask);

    float maskSum = sum(averageColorMask)[0];
    float imageSum = sum(imgF)[0];

    if(imageSum == 0) return 0; // avoid division by zero

    return static_cast<int>(255.0f * maskSum / imageSum);
}
