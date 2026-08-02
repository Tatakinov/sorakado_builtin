# SORAKADO\_builtin

## これは何？

伺かのSORAKADOプロトコル上のAO(シェル描画)/AI(バルーン描画)を
行うプログラムのリファレンス実装です。

ベースウェアからGUI周りの処理を切り離すことを目的として作られています。

現状ninix-kagariのみの対応です。

## Requirements

- SDL3

- SDL3-image

- SDL3-ttf

- jsoncpp

- fontconfig(Unix)

- wayland-client(Unix)

- onnxruntime(Optional)

## How to Build

```bash
$ git clone --recursive https://github.com/Tatakinov/sorakado_builtin
$ cd sorakado_builtin
$ cmake -S . -B build
$ cmake --build
```

## Usage

`sorakado_builtin.exe`を適当なディレクトリ`/path/to/sorakado`に放り込みます。

ninixを次のように呼び出します。

```bash
NINIX_ENABLE_SORAKADO=1 SORAKADO_PATH="/path/to/sorakado" ninix
```

## ONNX Runtimeによる超解像を用いた拡大処理

`sorakado_builtin.exe`と同じディレクトリに所定の形式の`model.onnx`を
置くことにより、シェルを拡大した場合にいい感じの拡大処理が行われます。

現在この`model.onnx`の形式は[sr-impl](https://github.com/Tatakinov/sr-impl)
で生成出来るものに限られます。

## LICENSE

基本的にはMIT-0です。

また、以下のソースコードを参考にしています。
[ONNXRuntime-example](https://github.com/microsoft/onnxruntime-inference-examples/blob/main/c_cxx/MNIST/MNIST.cpp) / MIT License (C) Microsoft

