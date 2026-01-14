# AIoT Sound Classification System for Education

## 📖 Overview
This project implements an Artificial Intelligence of Things (AIoT) system designed for educational environments. The system captures lecturer voice through a microphone connected to an ESP32 microcontroller, processes the audio using a lightweight AI model, and automatically extracts keywords and categories to Blynk platform.

## 🎯 Purpose
- Help students **stay aware** of key topics during lectures
- Reduce the burden of manual note-taking
- Provide **real-time feedback** on lecture content
- Support **personalised learning** by highlighting important keywords
- Enhance **accessibility**

## 📁 File Structure
```
Repository 1: Iot-Sound-Classifier
├── sketch.ino     # Main Arduino code
├── model.h        # Pre-trained AI model
├── diagram.json   # Wokwi circuit layout
├── libraries.txt  # Required libraries
└── README.md
Repository 2: Sound-Classification-AI
├── AI_training_model.ipynb 
└── README.md
```

## 🔌 Hardware Components
- ESP32 board, 1 microphone, 1 piezoelectric buzzer, 1 16x2 LCD (I2C), 2 buttons, 2LEDs, 1  1kΩ resistor

## ⚙️Simulation
Built and tested on [Wokwi]([https://wokwi.com/)
## 📊 AI Model Performance
- **Dataset Used:** AG News Classification Dataset (Kaggle- (https://www.kaggle.com/datasets/amananandrai/ag-news-classification-dataset)
- **Algorithm:** Logistic Regression with TF-IDF vectorisation
- **Accuracy:** 90.42%

## 🚀 How It Works
1. Press **Blue Button** or **Blynk App** to start recording
2. System displays countdown and begins capturing audio
3. Press **Red Button** to stop recording
4. AI model extracts **keywords** and predicts **category**
5. Results displayed on **LCD** and sent to **Blynk Cloud**

## ⚠️ Limitations
- Wokwi simulator does not support real-time audio capture
- Model size constrained by ESP32's 4MB flash memory
- TF-IDF used instead of advanced models due to hardware limitations

## 🔮 Future Improvements
- Deploy on physical ESP32 hardware for real audio capture
- Implement speech-to-text conversion
- Explore lighter deep learning models for better accuracy

## 👤 Author
Cassandra HEE Poh Sham 


