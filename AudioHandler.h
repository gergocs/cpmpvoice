#ifndef CPMPVOICE_AUDIOHANDLER_H
#define CPMPVOICE_AUDIOHANDLER_H

#define SAMPLE_RATE (44100)
#define NUM_CHANNELS    (2)

#include <queue>
#include <map>
#include <xstring>
#include "portaudio/include/portaudio.h"

class AudioHandler {
public:
    explicit AudioHandler(int inputDeviceID = -1, int outputDeviceID = -1);

    static std::map<int, std::string> getInputDeviceInfo();
    static std::map<int, std::string> getOutputDeviceInfo();

    /**
     * If error happens this will return the error code.
     * To get a normal error message you need to call Pa_GetErrorText(errorCode)
     * @return error code
     */
    [[nodiscard]] bool isError() const{
        return this->err;
    }

#ifdef DEBUG
    /**
     * If debug is enabled this will instantly return the error message
     * @return error message
     */
    [[nodiscard]] const char* isOutputSteamHappy() const{
        return Pa_GetErrorText(Pa_IsStreamStopped(this->outputStream));
    }
#endif

    [[maybe_unused]] void startOutputStream();

    [[maybe_unused]] void startInputStream();

    [[maybe_unused]] void stopInputStream();

    [[maybe_unused]] void stopOutputStream();

    /**
     * add value to receiver
     * @param value
     */
    [[maybe_unused]] void addToReceiver(float *value){
        this->received.push( value);
    }

    /**
     * get value from recorded
     * @return
     */
    [[maybe_unused]] float* getFromRecorder(){
        if (this->recorded.empty()){
            return nullptr;
        }
        float* tmp = this->recorded.front();
        this->recorded.pop();
        return tmp;
    }

    virtual ~AudioHandler();

private:
    static int recordCallback(const void *inputBuffer,
                              void *outputBuffer,
                              unsigned long framesPerBuffer,
                              const PaStreamCallbackTimeInfo* timeInfo,
                              PaStreamCallbackFlags statusFlags,
                              void *userData );
    static int playCallback(const void *inputBuffer,
                            void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData );

    PaStreamParameters inputParameters{};
    PaStreamParameters outputParameters{};
    std::queue<float*> recorded;
    std::queue<float*> received;
    PaStream *inputStream{};
    PaStream *outputStream{};
    PaError err;
};


#endif //CPMPVOICE_AUDIOHANDLER_H
