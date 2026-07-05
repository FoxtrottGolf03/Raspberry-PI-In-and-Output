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
