## Precondition
### Install dependencies
```
C++ 15 (Visual Studio 2017) or newer
SFML environment: https://www.sfml-dev.org/download/
```
## Configure SFML library in Visual Studio
```
# Debug configuration: 
1. C/C++ → General → Additional Include Directories → Edit: $(SolutionDir)\External\include
2. Linker → General → Additional Library Directories → Edit: $(SolutionDir)\External\lib
3. Linker → Input → Additional Dependencies → Edit:
sfml-system-d.lib
sfml-graphics-d.lib
sfml-window-d.lib
sfml-audio-d.lib
sfml-network-d.lib
# Release configuration:
1. Setup C++ Additional Include Directories and Linker Additional Library Directories same as Debug configuration
2. 3. Linker → Input → Additional Dependencies → Edit:
sfml-system.lib
sfml-graphics.lib
sfml-window.lib
sfml-audio.lib
sfml-network.lib
```

