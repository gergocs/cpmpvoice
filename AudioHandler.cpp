#include "AudioHandler.h"

/**
 * Create a new instance of the audiohandler
 * @param inputDeviceID if not present takes the default Input device
 * @param outputDeviceID if not present takes the default Output device
 */
AudioHandler::AudioHandler(int inputDeviceID, int outputDeviceID) : err(Pa_Initialize()) {
    if (isError()) {
        throw std::exception(Pa_GetErrorText(this->err));
    }

    this->inputParameters.device = (inputDeviceID == -1 ? Pa_GetDefaultInputDevice() : inputDeviceID);
    if (this->inputParameters.device == paNoDevice) {
        throw std::exception("Input device not found");
    }
    this->inputParameters.channelCount = Pa_GetDeviceInfo(inputParameters.device)->maxInputChannels;
    this->inputParameters.sampleFormat = paFloat32;
    this->inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
    this->inputParameters.hostApiSpecificStreamInfo = nullptr;

    this->outputParameters.device = (outputDeviceID == -1 ? Pa_GetDefaultOutputDevice() : outputDeviceID);
    if (this->outputParameters.device == paNoDevice) {
        throw std::exception("Output device not found");
    }
    this->outputParameters.channelCount = Pa_GetDeviceInfo(outputParameters.device)->maxOutputChannels;
    this->outputParameters.sampleFormat = paFloat32;
    this->outputParameters.suggestedLatency = Pa_GetDeviceInfo(this->outputParameters.device)->defaultLowOutputLatency;
    this->outputParameters.hostApiSpecificStreamInfo = nullptr;
}


/**
 * Starts the input stream (recording)
 * The data is stored in recorded queue
 */
[[maybe_unused]] void AudioHandler::startInputStream() {
    this->err = Pa_OpenStream(&this->inputStream,
                              &this->inputParameters,
                              nullptr,
                              SAMPLE_RATE,
                              512,
                              paClipOff,
                              recordCallback,
                              &this->recorded);

    if (isError()) {
        throw std::exception(Pa_GetErrorText(this->err));
    }

    this->err = Pa_StartStream(this->inputStream);
    if (isError()) {
        throw std::exception(Pa_GetErrorText(this->err));
    }
}

/**
 * Starts the output stream (playing)
 * The data is read from received queue
 */
[[maybe_unused]] void AudioHandler::startOutputStream() {
    this->err = Pa_OpenStream(&this->outputStream,
                              nullptr,
                              &this->outputParameters,
                              SAMPLE_RATE,
                              512,
                              paClipOff,
                              playCallback,
                              &this->received);

    if (isError()) {
        throw std::exception(Pa_GetErrorText(this->err));
    }

    this->err = Pa_StartStream(this->outputStream);
    if (isError()) {
        throw std::exception(Pa_GetErrorText(this->err));
    }
}

/**
 * Stops the input stream if not needed
 */
[[maybe_unused]] void AudioHandler::stopInputStream() {
    this->err = Pa_CloseStream(this->inputStream);
    if (isError()) {
        throw std::exception(Pa_GetErrorText(this->err));
    }
}

/**
 * Stops the output stream if not needed
 */
[[maybe_unused]] void AudioHandler::stopOutputStream() {
    this->err = Pa_CloseStream(this->outputStream);
    if (isError()) {
        throw std::exception(Pa_GetErrorText(this->err));
    }
}

AudioHandler::~AudioHandler() {
    this->err = Pa_Terminate();
}

/**
 * callback function for the input stream
 * @param inputBuffer
 * @param outputBuffer
 * @param framesPerBuffer
 * @param timeInfo
 * @param statusFlags
 * @param userData
 * @return
 */
int AudioHandler::recordCallback(const void *inputBuffer,
                                 [[maybe_unused]] void *outputBuffer,
                                 [[maybe_unused]] unsigned long framesPerBuffer,
                                 [[maybe_unused]] const PaStreamCallbackTimeInfo *timeInfo,
                                 [[maybe_unused]] PaStreamCallbackFlags statusFlags,
                                 void *userData) {
    auto* queue = static_cast<std::queue<float*>*>(userData);
    const auto *rptr = (const float *) inputBuffer;
    unsigned int size = 512 * NUM_CHANNELS;
    auto *wptr = new float[size];
    float *tmp = wptr;

    for (unsigned int i = 0; i < size; i++) {
        *wptr++ = *rptr++;
    }

    queue->push(tmp);

    return paContinue;
}

/**
 * callback function for the output stream
 * @param inputBuffer
 * @param outputBuffer
 * @param framesPerBuffer
 * @param timeInfo
 * @param statusFlags
 * @param userData
 * @return
 */
int AudioHandler::playCallback([[maybe_unused]] const void *inputBuffer,
                               void *outputBuffer,
                               [[maybe_unused]] unsigned long framesPerBuffer,
                               [[maybe_unused]] const PaStreamCallbackTimeInfo *timeInfo,
                               [[maybe_unused]] PaStreamCallbackFlags statusFlags,
                               void *userData) {
    auto* queue = static_cast<std::queue<float*>*>(userData);
    if (queue->empty()) {
        return paComplete;
    }
    float *rptr = queue->front();
    auto *wptr = (float *) outputBuffer;

    for (unsigned int i = 0; i < 512; i++) {
        *wptr++ = *rptr++;
    }

    delete[] queue->front();
    queue->pop();

    return paContinue;
}

/**
 * list input devices
 * @return std::map the key needed for the constructor.
 */
std::map<int, std::string> AudioHandler::getInputDeviceInfo() {
    int numDevices = Pa_GetDeviceCount();
    std::map<int, std::string> retVal;

    if(numDevices < 0)
    {
        throw std::exception( "ERROR: Pa_CountDevices returned 0x%x\n", numDevices );
    }

    for (int i = 0; i < numDevices; ++i) {
        const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo->maxInputChannels != 0){
            retVal.insert({i, std::string(deviceInfo->name)});
        }
    }

    return retVal;
}

/**
 * list output devices
 * @return std::map the key needed for the constructor.
 */

std::map<int, std::string> AudioHandler::getOutputDeviceInfo() {
    int numDevices = Pa_GetDeviceCount();
    std::map<int, std::string> retVal;

    if(numDevices < 0)
    {
        throw std::exception( "ERROR: Pa_CountDevices returned 0x%x\n", numDevices );
    }

    for (int i = 0; i < numDevices; ++i) {
        const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo->maxOutputChannels != 0){
            retVal.insert({i, std::string(deviceInfo->name)});
        }
    }

    return retVal;
}
