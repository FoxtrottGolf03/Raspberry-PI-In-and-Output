# GPIO INPUT and OUTPUT Raspberry PI 

## Inhaltsverzeichnis
1. [libgpiod - Installation](#Intstallation-von-libgpiod-v2-(Cpp)) 
2. [Beispiele](#Cpp-Beispiele) 
3. [Projectansatz](#Ansatz-fuer-das-Azubiproject)
## Intstallation von libgpiod v2 (Cpp)
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

## Cpp Beispiele
> originale API-Doku [API-Doku](https://libgpiod.readthedocs.io/en/master/core_line_request.html)
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
### Auf Taster reagieren - verkürzte Version
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
        auto inputSettings = gpiod::line_settings().set_direction(gpiod::line::direction::INPUT).set_bias(
            gpiod::line::bias::PULL_UP);
        auto input_line_cfg = std::move(gpiod::line_config().add_line_settings(taster, inputSettings));

        auto inputRequest = chip.prepare_request()
                                .set_consumer("button_reader")
                                .set_line_config(gpiod::line_config().add_line_settings(taster, inputSettings))
                                .do_request();
        std::cout << "Lese GPIO " << taster << std::endl;

        int counter = 0;
        while (true)
        {
            auto val = inputRequest.get_value(taster);

            if (val == gpiod::line::value::INACTIVE)
            {
                counter++;
                std::cout << "Taste gedrückt!" << " " << counter << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fehler:" << e.what() << std::endl;
        return 1;
    }
}


```
## Ansatz fuer das Azubiprojekt
```C++
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <map>
#include <gpiod.hpp>

int main()
{
    std::string chip_path = "/dev/gpiochip0"; //TODO Autodetect dafuer bauen

    std::vector<unsigned int> taster_pins = {17, 14}; //TODO Mapping dafuer bzw. wie Weitergabe an Handy?

    try
    {
        auto chip = gpiod::chip(chip_path);

        //alle Taster/Infrarot erhalten die selbe Einstellung
        auto inputSettings = gpiod::line_settings()
                             .set_direction(gpiod::line::direction::INPUT)
                             .set_bias(gpiod::line::bias::PULL_UP)
                             .set_edge_detection(gpiod::line::edge::FALLING)
                             .set_debounce_period(std::chrono::milliseconds(30));
        //TODO testen mit den Sensoren - sorgt fuer eine 'Sleeptime' zwischen den Events

        auto input_line_cfg = gpiod::line_config();

        //jeder Pin erhaelt die gleiche Einstellung
        for (unsigned int pin : taster_pins)
        {
            input_line_cfg.add_line_settings(pin, inputSettings);
        }

        auto inputRequest = chip.prepare_request()
                                .set_consumer("multi_button_reader")
                                //unter welchem Namen sind die PINs reserviert mit `gpioinfo` kann man einsehen welcher PIN grad wovon genutzt wird
                                .set_line_config(input_line_cfg) //welchen PINs wie arbeiten sollen
                                .do_request(); //sendet an den Kernel

        std::cout << "Warte auf 'Falling Edges' erkennung an den Pins: ";
        for (unsigned int pin : taster_pins)
        {
            std::cout << pin << " ";
        }
        std::cout << std::endl;

        std::map<unsigned int, int> counters;
        for (unsigned int pin : taster_pins)
        {
            counters[pin] = 0;
        }

        gpiod::edge_event_buffer event_buffer;
        //sorgt dafuer, dass Events sauber hintereinander abgefragt werden koennen

        while (true)
        {
            if (inputRequest.wait_edge_events(std::chrono::milliseconds(30)))
            //TODO Sensoraktivitaet abfragen und mit set_debounce_period abstimmen
            {
                inputRequest.read_edge_events(event_buffer);

                for (const auto& event : event_buffer) // Events abarbeiten
                {
                    if (event.type() == gpiod::edge_event::event_type::FALLING_EDGE)
                    {
                        unsigned int ausgeloester_pin = event.line_offset();

                        counters[ausgeloester_pin]++;

                        std::cout << "Taster an GPIO " << ausgeloester_pin
                            << " gedrückt! (Insgesamt: " << counters[ausgeloester_pin] << "x)"
                            << std::endl;
                    }
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fehler: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
```
