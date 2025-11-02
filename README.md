# Splash!

Using the [LiquidFun](https://github.com/google/liquidfun) Box2D fork with Raylib to render a fluid simulation.

![video demo](/media/demo.mp4)

## Building

On windows you can just run/doubleclick `run.bat` in the project root.

```console
git clone https://github.com/zeozeozeo/splash.git --recursive
cd splash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```
