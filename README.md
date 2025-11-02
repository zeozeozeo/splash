# Splash!

Using the [LiquidFun](https://github.com/google/liquidfun) Box2D fork with Raylib to render a fluid simulation.

https://github.com/user-attachments/assets/42f51715-1968-4ab4-8b59-ddc847602a22

## Building

On windows you can just run/doubleclick `run.bat` in the project root (or use one of the [releases](https://github.com/zeozeozeo/splash/releases)).

```console
git clone https://github.com/zeozeozeo/splash.git --recursive
cd splash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```
