# Speech Recognition Library

Speech recognition Library provides voice interaction to let you control your device with your voice.

Currently our speech library includes:
- *Microphone APIs*: provide API to get WAVE file of the input audio.
- *Speech recognition APIs*: leverage [Microsoft Cognitive Service](https://www.microsoft.com/cognitive-services/en-us/Speech-api/documentation/overview) to convert words spoken by the user into text for form input, for text dictation, to specify an action or command, and to accomplish tasks. 

This library is still being developed, and more cognitive APIs will be supported in the near future:
- *Text-To-Speech API*: 
- *Speaker Recognition API*:

## API Definition
- **recognizeSpeech** API
```cpp
SpeechResponse* recognizeSpeech(char * audioFileBinary, int length); 

typedef struct
{
    char * status;  //success or error
    char * text;
    double confidence;
}SpeechResponse;
```
