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