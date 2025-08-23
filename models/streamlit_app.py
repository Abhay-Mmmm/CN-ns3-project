import streamlit as st
import numpy as np
import cv2
from tensorflow.keras.models import load_model

# Load the trained model
model = load_model('footballer_face_classifier.h5')

# List of class names (update if needed)
classes = [
    'Cristiano_Ronaldo', 'Erling_Haaland', 'Kylian_Mbappe', 'Lionel_Messi', 'Neymar_Jr'
]

# Face detection function (same as notebook)
def detect_face(image, cascade_path='haarcascade_frontalface_default.xml', target_size=(128, 128)):
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    face_cascade = cv2.CascadeClassifier(cascade_path)
    faces = face_cascade.detectMultiScale(gray, scaleFactor=1.1, minNeighbors=5)
    if len(faces) == 0:
        return None
    (x, y, w, h) = faces[0]
    face = image[y:y+h, x:x+w]
    face = cv2.resize(face, target_size)
    return face

st.title('Footballer Face Classifier')
st.write('Upload a footballer image to identify the player.')

uploaded_file = st.file_uploader('Choose an image...', type=['jpg', 'jpeg', 'png'])

if uploaded_file is not None:
    file_bytes = np.asarray(bytearray(uploaded_file.read()), dtype=np.uint8)
    img = cv2.imdecode(file_bytes, 1)
    st.image(cv2.cvtColor(img, cv2.COLOR_BGR2RGB), caption='Uploaded Image', use_column_width=True)
    face = detect_face(img)
    if face is not None:
        face_norm = face / 255.0
        face_input = np.expand_dims(face_norm, axis=0)
        pred = model.predict(face_input)
        pred_class = np.argmax(pred, axis=1)[0]
        st.success(f'Predicted: {classes[pred_class]}')
    else:
        st.error('No face detected. Please upload a clear footballer face image.')
