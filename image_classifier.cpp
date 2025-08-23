/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Image Classifier Implementation
 * 
 * OpenCV-based image classification for footballer recognition
 */

#include "image_classifier.h"
#include <iostream>
#include <filesystem>

ImageClassifier::ImageClassifier()
    : m_lastConfidence(0.0)
{
    // Initialize face recognizer
    m_faceRecognizer = cv::face::LBPHFaceRecognizer::create();
    
    // Load Haar cascade for face detection
    LoadHaarCascade();
}

ImageClassifier::~ImageClassifier()
{
}

bool ImageClassifier::Initialize(const std::string& modelPath)
{
    if (!modelPath.empty())
    {
        return LoadModel(modelPath);
    }
    return true;
}

bool ImageClassifier::LoadHaarCascade()
{
    // Try to load Haar cascade from common OpenCV paths
    std::vector<std::string> cascadePaths = {
        "/usr/share/opencv4/haarcascades/haarcascade_frontalface_alt.xml",
        "/usr/local/share/opencv4/haarcascades/haarcascade_frontalface_alt.xml",
        "haarcascade_frontalface_alt.xml",
        "../data/haarcascade_frontalface_alt.xml"
    };
    
    for (const auto& path : cascadePaths)
    {
        if (m_faceCascade.load(path))
        {
            std::cout << "Loaded Haar cascade from: " << path << std::endl;
            return true;
        }
    }
    
    std::cerr << "Warning: Could not load Haar cascade. Face detection will be limited." << std::endl;
    return false;
}

bool ImageClassifier::TrainWithImages(const std::map<FootballerID, std::vector<std::string>>& imagePaths)
{
    m_trainImages.clear();
    m_trainLabels.clear();
    
    for (const auto& playerData : imagePaths)
    {
        FootballerID playerId = playerData.first;
        const auto& paths = playerData.second;
        
        std::cout << "Training with " << paths.size() << " images for " 
                  << FootballerIDToString(playerId) << std::endl;
        
        for (const auto& imagePath : paths)
        {
            cv::Mat image = LoadAndPreprocessImage(imagePath);
            if (!image.empty())
            {
                m_trainImages.push_back(image);
                m_trainLabels.push_back(static_cast<int>(playerId));
                std::cout << "  Added training image: " << imagePath << std::endl;
            }
            else
            {
                std::cerr << "  Failed to load image: " << imagePath << std::endl;
            }
        }
    }
    
    if (m_trainImages.size() > 0)
    {
        std::cout << "Training recognizer with " << m_trainImages.size() << " images..." << std::endl;
        m_faceRecognizer->train(m_trainImages, m_trainLabels);
        std::cout << "Training completed successfully!" << std::endl;
        return true;
    }
    
    std::cerr << "No valid training images found!" << std::endl;
    return false;
}

FootballerID ImageClassifier::ClassifyImage(const std::string& imagePath)
{
    cv::Mat image = LoadAndPreprocessImage(imagePath);
    if (image.empty())
    {
        std::cerr << "Failed to load image: " << imagePath << std::endl;
        return UNKNOWN;
    }
    
    return ClassifyImage(image);
}

FootballerID ImageClassifier::ClassifyImage(const cv::Mat& image)
{
    if (image.empty())
    {
        std::cerr << "Empty image provided for classification" << std::endl;
        return UNKNOWN;
    }
    
    int predictedLabel = -1;
    double confidence = 0.0;
    
    try
    {
        m_faceRecognizer->predict(image, predictedLabel, confidence);
        m_lastConfidence = confidence;
        
        // Lower confidence value means better match in LBPH
        // Threshold can be adjusted based on your training data
        if (confidence < 100.0)  // Adjust threshold as needed
        {
            FootballerID result = static_cast<FootballerID>(predictedLabel);
            std::cout << "Classified as: " << FootballerIDToString(result) 
                      << " (confidence: " << confidence << ")" << std::endl;
            return result;
        }
    }
    catch (const cv::Exception& e)
    {
        std::cerr << "OpenCV exception during classification: " << e.what() << std::endl;
    }
    
    std::cout << "Classification uncertain (confidence: " << confidence << ")" << std::endl;
    return UNKNOWN;
}

cv::Mat ImageClassifier::LoadAndPreprocessImage(const std::string& imagePath)
{
    // Load image
    cv::Mat image = cv::imread(imagePath);
    if (image.empty())
    {
        std::cerr << "Could not load image: " << imagePath << std::endl;
        return cv::Mat();
    }
    
    // Convert to grayscale
    cv::Mat grayImage;
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
    
    // Detect face
    cv::Rect faceRect;
    if (DetectFace(grayImage, faceRect))
    {
        // Extract face region
        cv::Mat face = grayImage(faceRect);
        return PreprocessFace(face);
    }
    else
    {
        // If no face detected, use the whole image (fallback)
        std::cout << "No face detected in " << imagePath << ", using whole image" << std::endl;
        return PreprocessFace(grayImage);
    }
}

bool ImageClassifier::DetectFace(const cv::Mat& image, cv::Rect& faceRect)
{
    if (m_faceCascade.empty())
    {
        return false;
    }
    
    std::vector<cv::Rect> faces;
    m_faceCascade.detectMultiScale(image, faces, 1.1, 3, 0, cv::Size(30, 30));
    
    if (faces.size() > 0)
    {
        // Use the largest face
        faceRect = faces[0];
        for (const auto& face : faces)
        {
            if (face.area() > faceRect.area())
            {
                faceRect = face;
            }
        }
        return true;
    }
    
    return false;
}

cv::Mat ImageClassifier::PreprocessFace(const cv::Mat& face)
{
    cv::Mat processedFace;
    
    // Resize to standard size
    cv::resize(face, processedFace, cv::Size(100, 100));
    
    // Histogram equalization for better lighting normalization
    cv::equalizeHist(processedFace, processedFace);
    
    return processedFace;
}

bool ImageClassifier::SaveModel(const std::string& modelPath)
{
    try
    {
        m_faceRecognizer->write(modelPath);
        std::cout << "Model saved to: " << modelPath << std::endl;
        return true;
    }
    catch (const cv::Exception& e)
    {
        std::cerr << "Failed to save model: " << e.what() << std::endl;
        return false;
    }
}

bool ImageClassifier::LoadModel(const std::string& modelPath)
{
    try
    {
        m_faceRecognizer->read(modelPath);
        std::cout << "Model loaded from: " << modelPath << std::endl;
        return true;
    }
    catch (const cv::Exception& e)
    {
        std::cerr << "Failed to load model: " << e.what() << std::endl;
        return false;
    }
}

std::string ImageClassifier::FootballerIDToString(FootballerID id)
{
    switch (id)
    {
        case MESSI: return "Messi";
        case RONALDO: return "Ronaldo";
        case NEYMAR: return "Neymar";
        case MBAPPE: return "Mbappe";
        case HAALAND: return "Haaland";
        default: return "Unknown";
    }
}

FootballerID ImageClassifier::StringToFootballerID(const std::string& name)
{
    if (name == "Messi") return MESSI;
    if (name == "Ronaldo") return RONALDO;
    if (name == "Neymar") return NEYMAR;
    if (name == "Mbappe") return MBAPPE;
    if (name == "Haaland") return HAALAND;
    return UNKNOWN;
}
