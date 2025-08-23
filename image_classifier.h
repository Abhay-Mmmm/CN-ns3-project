/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Image Classifier Header
 * 
 * This module provides OpenCV-based image classification for footballer recognition
 * Integrates with NS-3 simulation for content-aware routing
 */

#ifndef IMAGE_CLASSIFIER_H
#define IMAGE_CLASSIFIER_H

#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <string>
#include <vector>
#include <map>

// Footballer ID enumeration (matching main simulation)
enum FootballerID {
    MESSI = 0,
    RONALDO = 1,
    NEYMAR = 2,
    MBAPPE = 3,
    HAALAND = 4,
    UNKNOWN = -1
};

class ImageClassifier {
public:
    ImageClassifier();
    ~ImageClassifier();
    
    // Initialize the classifier with training data
    bool Initialize(const std::string& modelPath = "");
    
    // Train the classifier with footballer images
    bool TrainWithImages(const std::map<FootballerID, std::vector<std::string>>& imagePaths);
    
    // Classify a single image
    FootballerID ClassifyImage(const std::string& imagePath);
    FootballerID ClassifyImage(const cv::Mat& image);
    
    // Get confidence score of last classification
    double GetConfidence() const { return m_lastConfidence; }
    
    // Load and preprocess image
    cv::Mat LoadAndPreprocessImage(const std::string& imagePath);
    
    // Save/Load trained model
    bool SaveModel(const std::string& modelPath);
    bool LoadModel(const std::string& modelPath);
    
    // Utility functions
    static std::string FootballerIDToString(FootballerID id);
    static FootballerID StringToFootballerID(const std::string& name);

private:
    // Face detection
    cv::CascadeClassifier m_faceCascade;
    
    // Face recognition
    cv::Ptr<cv::face::LBPHFaceRecognizer> m_faceRecognizer;
    
    // Training data
    std::vector<cv::Mat> m_trainImages;
    std::vector<int> m_trainLabels;
    
    // Classification confidence
    double m_lastConfidence;
    
    // Helper methods
    bool DetectFace(const cv::Mat& image, cv::Rect& faceRect);
    cv::Mat PreprocessFace(const cv::Mat& face);
    bool LoadHaarCascade();
};

#endif // IMAGE_CLASSIFIER_H
