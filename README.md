# SHWManager

## Introduce

- SHWManager is advanced file manager for windows, Linux, Mac(Linux and mac is not supported yet)

- Taskmanager supported

  - Process information(CPU Usage, Memory Usage, File Description, Company Name, Path etc)

- Resource Usage(CPU, Memory, Network, Pagefile) and Usage Graph supported

- It is supported Disk usage View

  

## Build

- Compiler : gcc 12.2.0
- Gui Library : wxWidgets 3.2.1

```shell
> Release build
  mingw32-make -j8 -f makefile.gcc SHARED=1 UNICODE=1 CFG=x64 CXXFLAGS="-std=c++20" BUILD=release clean
> Debug build
  mingw32-make -j8 -f makefile.gcc SHARED=1 UNICODE=1 CFG=x64 CXXFLAGS="-std=c++20" BUILD=debug clean  
```



## 3rd Library

- json : [rapidjson](https://rapidjson.org/)
- zip : [zlib](https://zlib.net/), [info-zip](https://infozip.sourceforge.net/)



## License

- MIT License

  

## History

- 2022.12.23 : Readonly file operation(copy, paste) bug fixed
- 2022.12.20 : PathView 
- 2022.12.15 : Implement PathView escape key event
- 2022.12.12 : Version 1.0.0
