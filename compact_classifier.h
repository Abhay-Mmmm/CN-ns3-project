// Compact Image Classifier
#pragma once
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

enum FootballerID {MESSI=0,RONALDO,NEYMAR,MBAPPE,HAALAND,UNKNOWN};

class ImageClassifier {
public:
    bool LoadModel(const std::string& path) { 
        try { m_rec=cv::face::LBPHFaceRecognizer::create(); m_rec->read(path); m_cascade.load("models/haarcascade_frontalface_default.xml"); return true; } 
        catch(...) { return false; } 
    }
    FootballerID ClassifyImage(const std::string& path) {
        cv::Mat img=cv::imread(path), gray; cv::cvtColor(img,gray,cv::COLOR_BGR2GRAY); std::vector<cv::Rect> faces;
        m_cascade.detectMultiScale(gray,faces); if(faces.empty()) return UNKNOWN;
        cv::Mat face=gray(faces[0]); int label; double conf; m_rec->predict(face,label,conf);
        return (conf<100 && label>=0 && label<5) ? static_cast<FootballerID>(label) : UNKNOWN;
    }
    static std::string FootballerIDToString(FootballerID id) {
        static std::vector<std::string> names={"Messi","Ronaldo","Neymar","Mbappe","Haaland","Unknown"};
        return names[id];
    }
private:
    cv::Ptr<cv::face::LBPHFaceRecognizer> m_rec; cv::CascadeClassifier m_cascade;
};
