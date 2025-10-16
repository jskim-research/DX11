## DirectX 11 Toy Project

Currently, I'm intereseted in NPR(Non-Photo Rendering).

## Directory Structure

```
DX11/
│
├─ main.cpp # Entry point of the application
├─ main.h # Main application header
│
├─ data/ # Static resources used by the application
│
├─ hlsl/ # HLSL shader source files
│
├─ include/ # Public header files (accessible by all modules)
│ ├─ camera/ 
│ ├─ framework/ # Core framework headers (e.g., applicationclass.h) 
│ ├─ light/
│ ├─ model/  # Classes representing models (e.g., Bitmap, GLB)
│ ├─ shader/  # Classes for rendering
│ ├─ texture/
│ ├─ utility/ # General-purpose utilities (json, parser, tiny_gltf, etc.)
│
├─ log/ # Application log files and debug outputs
│
├─ source/ # Source (.cpp) implementations
│ ├─ The folder structure is the same as in the include/ directory
```