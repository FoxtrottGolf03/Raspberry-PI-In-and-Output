# GPIO INPUT and OUTPUT Raspberry PI (Model 4B)
## Intstallation von libgpiod v2 (C++)
>Standartmaeßig kommt libgpiod nur mit C Bindings deswegen Installation fuer C++   
> Diese Douke setzt sich aus Chat und Foren zusammen!   
> libgpiod-Doku: [Originale Dokumentation](https://libgpiod.readthedocs.io/en/master/building.html)   

1. Build-Tools
```bash
   sudo apt update
   sudo apt install -y git build-essential autoconf automake libtool pkg-config -autoconf-archive
   ```
2. Quellcode aus Git laden
```bash
    git clone https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git/
    cd libgpiod
```
3. Zweig fuer V2 auswaehlen
```bash
    git checkout v2.3.1
```
4. C++ aktivieren
```bash
    ./autogen.sh --enable-bindings-cxx --enable-tools
```
5. compilieren und installiere
```bash
    make -j$(nproc)
    sudo make install
    sudo ldconfig
```
6. (optional - falls CMAKE schon vorhanden) Clion -> Tools -> CMake -> Reset Cache and Reload Project
## CMakeList.txt einrichten
1.
```CMake
    cmake_minimum_required(VERSION 3.31)
    project("PROJECTNAME" CXX)
    
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    #in /usr/local/include MUSS gpiodcxx liegen!
    include_directories(/usr/local/include)
    
    find_library(GPIOD_CXX_LIB gpiodcxx REQUIRED)
    
    add_executable(("PROJECTNAME" main.cpp)
    
    target_link_libraries(("PROJECTNAME" PRIVATE ${GPIOD_CXX_LIB})
```
2. Clion -> Tools -> CMake -> Reset Cache and Reload Project

## C++ Beispiele
### LED blinken lassen
```C++
#include <iostream>
#include <chrono>
#include <thread>
#include <gpiod.hpp>
int main()
{
    std::string chip_path = "/dev/gpiochip0";
    unsigned int led = 17;

    try
    {
        auto chip = gpiod::chip(chip_path);
        auto settings = gpiod::line_settings().set_direction(gpiod::line::direction::OUTPUT);
        auto line_cfg = std::move(gpiod::line_config().add_line_settings(led, settings));

        auto request = chip.prepare_request()
            .set_consumer("blink_example")
            .set_line_config(line_cfg)
            .do_request();
        std::cout << "Blinke GPIO " << led << std::endl;
        request.set_value(led, gpiod::line::value::INACTIVE);

        while (true)
        {
            request.set_value(led, gpiod::line::value::ACTIVE);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            request.set_value(led, gpiod::line::value::INACTIVE);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

        }
        request.set_value(led, gpiod::line::value::INACTIVE);

    }
    catch (const std::exception &e)
    {
        std::cerr << "Fehler:" << e.what() << std::endl;
        return 1;
    }
}
```
### Auf Taster reagiren
```C++
#include <iostream>
#include <chrono>
#include <thread>
#include <gpiod.hpp>
int main()
{
    std::string chip_path = "/dev/gpiochip0";
    unsigned int taster = 17;

    try
    {
        auto chip = gpiod::chip(chip_path);
        auto inputSettings = gpiod::line_settings().set_direction(gpiod::line::direction::INPUT).set_bias(gpiod::line::bias::PULL_UP);
        auto input_line_cfg = std::move(gpiod::line_config().add_line_settings(taster, inputSettings));


        auto inputRequest = chip.prepare_request()
            .set_consumer("button_reader")
            .set_line_config(input_line_cfg)
            .do_request();



        std::cout << "Lese GPIO " << taster << std::endl;


        int counter = 0;
        while (true)
        {
            auto val = inputRequest.get_value(taster);

            if (val == gpiod::line::value::INACTIVE)
            {
                counter++;
                std::cout << "Taste gedrückt!" <<" "<< counter <<  std::endl;

            }

            std::this_thread::sleep_for(std::chrono::milliseconds(200));


        }


    }
    catch (const std::exception &e)
    {
        std::cerr << "Fehler:" << e.what() << std::endl;
        return 1;
    }
}
```
### Auf Taster mit Led reagieren
```C++
#include <iostream>
#include <chrono>
#include <thread>
#include <gpiod.hpp>
int main()
{
    std::string chip_path = "/dev/gpiochip0";
    unsigned int taster = 17;
    unsigned int led = 27;

    try
    {
        auto chip = gpiod::chip(chip_path);
        auto inputSettings = gpiod::line_settings().set_direction(gpiod::line::direction::INPUT).set_bias(gpiod::line::bias::PULL_UP);
        auto outputSettings = gpiod::line_settings().set_direction(gpiod::line::direction::OUTPUT);
        auto input_line_cfg = std::move(gpiod::line_config().add_line_settings(taster, inputSettings));
        auto output_line_cfg = std::move(gpiod::line_config().add_line_settings(led, outputSettings));

        auto inputRequest = chip.prepare_request()
            .set_consumer("button_reader")
            .set_line_config(input_line_cfg)
            .do_request();

        auto outputRequest = chip.prepare_request()
            .set_consumer("led")
            .set_line_config(output_line_cfg)
            .do_request();

        std::cout << "Lese GPIO " << taster << " und reagiere mit: " << led << std::endl;


        int counter = 0;
        while (true)
        {
            auto val = inputRequest.get_value(taster);

            if (val == gpiod::line::value::INACTIVE)
            {
                counter++;
                std::cout << "Taste gedrückt!" <<" "<< counter <<  std::endl;
                outputRequest.set_value(led, gpiod::line::value::ACTIVE);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            auto info = outputRequest.get_value(led);
            if (info == gpiod::line::value::ACTIVE)
            {
                std::cout << "Taste gedrckt!" <<" "<< counter <<  std::endl;
            }
            std::cout << info << std::endl;

        }


    }
    catch (const std::exception &e)
    {
        std::cerr << "Fehler:" << e.what() << std::endl;
        return 1;
    }
}
```