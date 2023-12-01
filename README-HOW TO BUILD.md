# 1. Setup
## 1.1 Linux
1. Install CMake via [insert your favorite packet manager here]
2. (Optional, but highly recommended) Install neovim
3. Run the example application
    ``` console
        $ mkdir build && cd build
        $ cmake [-DEXMPLE=1] ..
            -- The C compiler identification is GNU 13.2.1
            [...]
        $ cmake --build .
            [  6%] Building C object src/portable/CMakeFiles/freertos_kernel_port.dir/Posix/port.c.o
            [...]
        $ ./example/example 
            Message received from task
            [...]
    ```

## 1.2 Windows
1. Install mingw from http://sourceforge.net/projects/mingw/ 
    - mark `mingw32-base` and `mingw32-gcc-g++` for installation
    - in the `Installation` menu, press `Apply Changes`
2. Add mingw to PATH
    - e.g. add `C:\MinGW\bin` to PATH
3. Install CMake from https://cmake.org/download/
4. Test the setup and paths
    - run `gcc --version`
    - run `cmake --version`
5. Open the project, e.g., in VSCode
6. Run the example application
    ``` powershell
        PS C:\Users\User\Desktop\rtc_freertos> mkdir build
            [...]
        PS C:\Users\User\Desktop\rtc_freertos> cd build
            [...]
        PS C:\Users\User\Desktop\rtc_freertos\build> cmake -G "MinGW Makefiles" [-DEXMPLE=1] ..
            -- The C compiler identification is GNU 6.3.0
            [...]

        PS C:\Users\User\Desktop\rtc_freertos\build> cmake --build .
            [  7%] Building C object src/portable/CMakeFiles/freertos_kernel_port.dir/MSVC-MingW/port.c.obj
            [...]
        PS C:\Users\User\Desktop\rtc_freertos\build> .\example\example.exe
            Message received from task
            [...]
    ```
