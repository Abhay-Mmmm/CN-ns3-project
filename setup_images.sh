#!/bin/bash

# Setup script for footballer image classification project
# This script creates directory structure and downloads sample images

echo "Setting up Footballer Network Simulation Project..."

# Create necessary directories
mkdir -p images
mkdir -p training_images/Messi
mkdir -p training_images/Ronaldo
mkdir -p training_images/Neymar
mkdir -p training_images/Mbappe
mkdir -p training_images/Haaland
mkdir -p results
mkdir -p models

echo "Created directory structure:"
echo "  - images/           (for images to classify and send)"
echo "  - training_images/  (organized by player for training)"
echo "  - results/          (simulation results)"
echo "  - models/           (trained classifier models)"

# Create a sample configuration file for image paths
cat > image_config.txt << EOF
# Footballer Image Configuration
# Format: PlayerName=ImagePath1,ImagePath2,...

Messi=training_images/Messi/messi1.jpg,training_images/Messi/messi2.jpg
Ronaldo=training_images/Ronaldo/ronaldo1.jpg,training_images/Ronaldo/ronaldo2.jpg
Neymar=training_images/Neymar/neymar1.jpg,training_images/Neymar/neymar2.jpg
Mbappe=training_images/Mbappe/mbappe1.jpg,training_images/Mbappe/mbappe2.jpg
Haaland=training_images/Haaland/haaland1.jpg,training_images/Haaland/haaland2.jpg
EOF

# Create README for image setup
cat > IMAGE_SETUP.md << EOF
# Image Setup Instructions

## Directory Structure

- \`training_images/\`: Organized training images by player name
  - \`Messi/\`: Messi's images for training
  - \`Ronaldo/\`: Ronaldo's images for training
  - \`Neymar/\`: Neymar's images for training
  - \`Mbappe/\`: Mbappe's images for training
  - \`Haaland/\`: Haaland's images for training

- \`images/\`: Mixed images to classify and transmit
- \`models/\`: Saved classifier models
- \`results/\`: Simulation results

## Setting Up Images

1. **Training Images**: Place at least 5-10 images of each player in their respective folders
2. **Test Images**: Place mixed images in the \`images/\` folder
3. **Image Formats**: Supported formats are JPG, JPEG, PNG

## Running with Real Images

\`\`\`bash
# Train and run with real images
./footballer-network-sim --useRealImages=true --trainDir=./training_images --imageDir=./images

# Run with pre-trained model
./footballer-network-sim --useRealImages=true --modelPath=./models/footballer_model.yml --imageDir=./images
\`\`\`

## Simulation Modes

1. **Simulated Mode** (default): Uses generated patterns instead of real images
2. **Real Image Mode**: Uses OpenCV for actual image classification

## OpenCV Requirements

Make sure OpenCV is installed with face recognition support:
\`\`\`bash
sudo apt-get install libopencv-dev libopencv-contrib-dev
\`\`\`
EOF

echo "Created configuration files:"
echo "  - image_config.txt  (sample image configuration)"
echo "  - IMAGE_SETUP.md    (setup instructions)"

# Download Haar cascade if not present
HAAR_CASCADE_URL="https://raw.githubusercontent.com/opencv/opencv/master/data/haarcascades/haarcascade_frontalface_alt.xml"
if [ ! -f "haarcascade_frontalface_alt.xml" ]; then
    echo "Downloading Haar cascade for face detection..."
    wget -q "$HAAR_CASCADE_URL" -O haarcascade_frontalface_alt.xml
    if [ $? -eq 0 ]; then
        echo "Downloaded haarcascade_frontalface_alt.xml"
    else
        echo "Warning: Could not download Haar cascade. Face detection may be limited."
    fi
fi

echo ""
echo "Setup completed!"
echo ""
echo "Next steps:"
echo "1. Place footballer images in training_images/ folders"
echo "2. Place test images in images/ folder"
echo "3. Build the project: make"
echo "4. Run simulation: ./footballer-network-sim --useRealImages=true"
echo ""
echo "For detailed instructions, see IMAGE_SETUP.md"
