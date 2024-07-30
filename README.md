# VG-Lite Simulator
Software rendering of VG-Lite API function based on [ThorVG](https://github.com/thorvg/thorvg), simplify VG-Lite upper-layer application development and debugging.

![tiger](https://github.com/user-attachments/assets/964e1831-d297-4580-9829-7480eae2a3cc)

## Clone
```bash
git clone https://github.com/FASTSHIFT/vg_lite_tvg.git --recursive
```

## Build
```bash
mkdir build
cd build
cmake ..
make -j
```

## Run
```bash
./build/vg_lite
```
View the PNG file in the current directory to get the drawn image.

Add the `-h` parameter to get the complete list of parameters.

## LVGL integration
The simulator is integrated into [LVGL](https://github.com/lvgl/lvgl) and participates in CI compilation and automated testing.

See the [LVGL documentation](https://docs.lvgl.io/master/overview/vg_lite_tvg.html) for more information.
