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
            outputRequest.set_value(led, gpiod::line::value::INACTIVE);
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



    // try
    // {
    //     auto chip = gpiod::chip(chip_path);
    //     auto settings = gpiod::line_settings().set_direction(gpiod::line::direction::OUTPUT);
    //     auto line_cfg = std::move(gpiod::line_config().add_line_settings(taster, settings));
    //
    //     auto request = chip.prepare_request()
    //         .set_consumer("blink_example")
    //         .set_line_config(line_cfg)
    //         .do_request();
    //     std::cout << "Blinke GPIO " << taster << std::endl;
    //     request.set_value(taster, gpiod::line::value::INACTIVE);
    //
    //     while (true)
    //     {
    //         request.set_value(taster, gpiod::line::value::ACTIVE);
    //         std::this_thread::sleep_for(std::chrono::milliseconds(500));
    //
    //         request.set_value(taster, gpiod::line::value::INACTIVE);
    //         std::this_thread::sleep_for(std::chrono::milliseconds(500));
    //
    //     }
    //     request.set_value(taster, gpiod::line::value::INACTIVE);
    //
    // }
    // catch (const std::exception &e)
    // {
    //     std::cerr << "Fehler:" << e.what() << std::endl;
    //     return 1;
    // }
}


