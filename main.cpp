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
