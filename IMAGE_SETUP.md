# Image Setup Instructions

## Directory Structure

- `training_images/`: Organized training images by player name
  - `Messi/`: Messi's images for training
  - `Ronaldo/`: Ronaldo's images for training
  - `Neymar/`: Neymar's images for training
  - `Mbappe/`: Mbappe's images for training
  - `Haaland/`: Haaland's images for training

- `images/`: Mixed images to classify and transmit
- `models/`: Saved classifier models
- `results/`: Simulation results

## Setting Up Images

1. **Training Images**: Place at least 5-10 images of each player in their respective folders
2. **Test Images**: Place mixed images in the `images/` folder
3. **Image Formats**: Supported formats are JPG, JPEG, PNG

## Running with Real Images

```bash
# Train and run with real images
./footballer-network-sim --useRealImages=true --trainDir=./training_images --imageDir=./images

# Run with pre-trained model
./footballer-network-sim --useRealImages=true --modelPath=./models/footballer_model.yml --imageDir=./images
```

## Simulation Modes

1. **Simulated Mode** (default): Uses generated patterns instead of real images
2. **Real Image Mode**: Uses OpenCV for actual image classification

## OpenCV Requirements

Make sure OpenCV is installed with face recognition support:
```bash
sudo apt-get install libopencv-dev libopencv-contrib-dev
```
