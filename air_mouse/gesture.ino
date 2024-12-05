#include <LSM6DS3.h>
#include <Wire.h>

#include <TensorFlowLite.h>
#include <tensorflow/lite/micro/all_ops_resolver.h>
#include <tensorflow/lite/micro/micro_error_reporter.h>
#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/schema/schema_generated.h>
//#include <tensorflow/lite/version.h>

#include "model.h"
#include "air_mouse.h"

const float accelerationThreshold = 2.5; // threshold of significant in G's
const int numSamples = 119;

int samplesRead = numSamples;
// global variables used for TensorFlow Lite (Micro)
tflite::MicroErrorReporter tflErrorReporter;

// pull in all the TFLM ops, you can remove this line and
// only pull in the TFLM ops you need, if would like to reduce
// the compiled size of the sketch.
tflite::AllOpsResolver tflOpsResolver;

const tflite::Model* tflModel = nullptr;
tflite::MicroInterpreter* tflInterpreter = nullptr;
TfLiteTensor* tflInputTensor = nullptr;
TfLiteTensor* tflOutputTensor = nullptr;

// Create a static memory buffer for TFLM, the size may need to
// be adjusted based on the model you are using
constexpr int tensorArenaSize = 8 * 1024;
byte tensorArena[tensorArenaSize] __attribute__((aligned(16)));



void setup_gesture() {
  // get the TFL representation of the model byte array
  tflModel = tflite::GetModel(model);
  if ((tflModel)->version() != TFLITE_SCHEMA_VERSION) {
    Serial.println("Model schema mismatch!");
    while (1)
      ;
  }

  // Create an interpreter to run the model
  tflInterpreter = new tflite::MicroInterpreter(tflModel, tflOpsResolver, tensorArena, tensorArenaSize, &tflErrorReporter);

  // Allocate memory for the model's input and output tensors
  tflInterpreter->AllocateTensors();

  // Get pointers for the model's input and output tensors
  tflInputTensor = tflInterpreter->input(0);
  tflOutputTensor = tflInterpreter->output(0);
}

const char* handle_gesture(LSM6DS3 myIMU) {
  float aX, aY, aZ, gX, gY, gZ;

  // wait for significant motion
  while (samplesRead == numSamples) {
    // read the acceleration data
    aX = myIMU.readFloatAccelX();
    aY = myIMU.readFloatAccelY();
    aZ = myIMU.readFloatAccelZ();

    // sum up the absolutes
    float aSum = fabs(aX) + fabs(aY) + fabs(aZ);

    // check if it's above the threshold
    if (aSum >= accelerationThreshold) {
      // reset the sample read count
      samplesRead = 0;
      break;
    }
  }

  // check if the all the required samples have been read since
  // the last time the significant motion was detected
  while (samplesRead < numSamples) {
    // check if new acceleration AND gyroscope data is available
    // read the acceleration and gyroscope data
    aX = myIMU.readFloatAccelX();
    aY = myIMU.readFloatAccelY();
    aZ = myIMU.readFloatAccelZ();

    gX = myIMU.readFloatGyroX();
    gY = myIMU.readFloatGyroY();
    gZ = myIMU.readFloatGyroZ();

    // normalize the IMU data between 0 to 1 and store in the model's
    // input tensor
    tflInputTensor->data.f[samplesRead * 6 + 0] = (aX + 4.0) / 8.0;
    tflInputTensor->data.f[samplesRead * 6 + 1] = (aY + 4.0) / 8.0;
    tflInputTensor->data.f[samplesRead * 6 + 2] = (aZ + 4.0) / 8.0;
    tflInputTensor->data.f[samplesRead * 6 + 3] = (gX + 2000.0) / 4000.0;
    tflInputTensor->data.f[samplesRead * 6 + 4] = (gY + 2000.0) / 4000.0;
    tflInputTensor->data.f[samplesRead * 6 + 5] = (gZ + 2000.0) / 4000.0;

    samplesRead++;

    if (samplesRead == numSamples) {
      // Run inferencing
      TfLiteStatus invokeStatus = tflInterpreter->Invoke();
      if (invokeStatus != kTfLiteOk) {
        Serial.println("Invoke failed!");
        while (1);
        return NULL;
      }

      // Loop through the output tensor values from the model
      for (int i = 0; i < NUM_GESTURES; i++) {
        if (tflOutputTensor->data.f[i] > 0.98) {
          return GESTURES[i];
        }
        /*
          Serial.print(GESTURES[i]);
          Serial.print(": ");
          Serial.println(tflOutputTensor->data.f[i], 6);
          */
      }
    }
  }
  return NULL;
}