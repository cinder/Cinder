
This is a bit of a re-thinking of one possibility of how audio could be re-done for the web 
once ScriptProcessorNode is finally not useable anywhere. 


Essentially the basic idea is that, unlike the current implementation, everything is as native as possible and centered around the idea of the AudioWorklet; this is a new way to process audio on the web and offers advantages that the ScriptProcessorNode does not, for example, the ability to run things on a separate thread.


The disadvantage however is that it is a bit tedious to set up due to the face that there aren't any native Emscripten apis and a lot of things are designed to be asyncrounous. Also in this context, things are especially complicated for any kind of custom work that someone might want to do. 

This hopefully presents one path to implementing an api on the C++ side while keeping a fairly similar api. 
  
  
The basic idea 
=====
  
There is an audio context and 2 types of nodes that can be used. 

__Audio Node Type 1 : NativeNode__
The first type of node would be called a `NativeNode`. Essentially this would reference any kind of buit-in node within the WebAudio API like a delay node. 

__Audio node Type 2 : CustomNode__
The second type of node would be called a `CustomNode`. This would refer to a node that users can create and do custom processing on. 

Note that in this case, there is no output object on the context, the context itself serves as the output object and knows how to grab the global AudioContext's destination attribute.

Things that still need to be thought out
===
* It doesn't look like it's possible to extract any kind of a buffer out of built-in nodes, making it hard to tweak the output 
* Can we just pass a function to a custom AudioWorkletNode that gets bound to the process function? 


References
===
* [https://developer.mozilla.org/en-US/docs/Web/API/ScriptProcessorNode](https://developer.mozilla.org/en-US/docs/Web/API/ScriptProcessorNode)
* [https://developers.google.com/web/updates/2017/12/audio-worklet](https://developers.google.com/web/updates/2017/12/audio-worklet)
* [https://googlechromelabs.github.io/web-audio-samples/audio-worklet/](https://googlechromelabs.github.io/web-audio-samples/audio-worklet/)
