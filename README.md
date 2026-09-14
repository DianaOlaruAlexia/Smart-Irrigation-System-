# Smart-Irrigation-System-
A smart irrigation prototype built with C++ and Arduino that uses soil moisture data and weather forcasts to calculate the required irrigation amount.
The desktop application retrieves weather data from the Open-Meteo REST API, calculates the final water requirement, and communicates with an Arduino over Serial to control the irrigation hardware.

## Project Status 
It is currently a work in progress.

## Implemented
 - Arduino to C++ serial communication
 - C++ to Arduino ON/OFF commands
 - Open-Meteo REST API integration
 - JSON parsing with 'nlohmann/json'
 - Irrigation calculation based on soil moisture and weather forecast
 - Irrigation logic that determines a base water amount from soil moisture levels

## Next Steps
 - Replace simulated soil moisture values with real soil moisture sensors
 - Read sensor values continuously in real time
 - Add a continuous control loop in the C++ application
 - Integrate a real water pump
 - Calibrate pump flow rate and convert water volume into pump runtime
 - Add a simple dashboard for monitoring and manual control

## Architecture
The Arduino reads the soil moisture value and sends it to the C++ application through USB Serial communication.
The C++ application retrieves weather data from the Open-Meteo API, calculates the required irrigation amount, and sends an ON/OFF command back to the Arduino.

The Arduino then controls the LED, which currently simulates the future water pump.
## How It Works
1. The Arduino reads the soil moisture values and calculates the average moisture level.
2. The average value is sent to the C++ application through USB Serial communication.
3. The C++ application retrieves precipitation data from the Open-Meteo REST API.
4. The application determines a base irrigation amount from the soil moisture level.
5. Expected rainfall is converted from millimeters to liters for the irrigation area.
6. The expected rainfall is subtracted from the base irrigation amount.
7. If irrigation is required, the application sends an 'ON' command to the Arduino. Otherwise, it sends 'OFF'.
8. The Arduino currently uses its built-in LED to simulate the future pump control.

## Technologies
 - C++
 - Arduino
 - Visual Studio
 - Open-Meteo REST API
 - libcurl
 - nlohmann/json
 - Windows Serial Communication
 - vcpkg

## Irrigation Calculation
The prototype assumes an irrigation area of 150 m² that is irrigated once in 2 days.
Weather forecasts provide precipitation in millimeters. Since 1 mm of rain over 1 m² corresponds to approximately 1 l of water, forecast precipitation is converted to liters using:
'rainWater = precipitation × 150'
The expected rain contribution is then adjusted using the forecast rain probability:
'expectedRain = rainWater × rainProbability / 100'
The base irrigation amount is selected according to the average soil moisture:

- Below 40% → 1500 L (that is 5 L x 2 days x 150 m²)
- 40%–49% → 1200 L (that is 4 L x 2 days x 150 m²)
- 50%–69% → 600 L (that is 2 L x 2 days x 150 m²)
- 70% or higher → 0 L

The final irrigation amount is calculated by subtracting the expected rainfall from the base irrigation amount. Negative results are set to 0.