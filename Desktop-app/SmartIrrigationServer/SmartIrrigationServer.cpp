
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <windows.h>

struct WeatherForecast {
	double expectPrecipitation;
	int rainProbability;
};

class IrrigationEngine {
public:
	int computeBaseWaterAmount(int average) {
		if (average < 40){
			return 1500;
		}
		else if (average<50){
			return 1200;
		}
		else if (average<70){
			return 600;
		}
		else{
			return 0;
		}
	}
};

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp){
	size_t totalSize = size * nmemb;
	std::string* response = static_cast<std::string*>(userp);
	response->append(static_cast<char*>(contents), totalSize);
	return totalSize;
}


int main()
{
	//open COM3 
	HANDLE serialPort = CreateFileA(
		"\\\\.\\COM3",
		GENERIC_READ | GENERIC_WRITE,
		0,
		nullptr,
		OPEN_EXISTING,
		0,
		nullptr
	);
	DCB serialParameters = { 0 };
	serialParameters.DCBlength = sizeof(serialParameters);
	GetCommState(serialPort, &serialParameters);

	serialParameters.BaudRate = CBR_9600;
	serialParameters.ByteSize = 8; //each character uses 8 bits
	serialParameters.StopBits = ONESTOPBIT;
	serialParameters.Parity = NOPARITY;

	if (!SetCommState(serialPort, &serialParameters)) {
		std::cout << "Could not set serial settings\n";
	}


	COMMTIMEOUTS timeouts = { 0 };

	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 1000;
	timeouts.ReadTotalTimeoutMultiplier = 0;

	SetCommTimeouts(serialPort, &timeouts);

	Sleep(2000);
	
	//take the values from arduino
	char buffer[64];
	DWORD bytesRead = 0;
	ReadFile(
		serialPort,
		buffer,
		sizeof(buffer) - 1,
		&bytesRead,
		nullptr
	);

	buffer[bytesRead] = '\0';

	std::string received = buffer;

	//declaration
	IrrigationEngine engine;
	int average = std::stoi(received);
	int baseWaterAmount = engine.computeBaseWaterAmount(average);

	//take weather from API
	CURL* curl = curl_easy_init();

	if (curl != nullptr) {
		CURLOPT_URL;
	}

	curl_easy_setopt(  
		curl,
		CURLOPT_URL,
		"https://api.open-meteo.com/v1/forecast?latitude=44.44&longitude=26.10&daily=precipitation_sum,precipitation_probability_max&forecast_days=3"
	);

	std::string response;

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

	curl_easy_perform(curl);

	nlohmann::json data = nlohmann::json::parse(response);

	curl_easy_cleanup(curl);

	
	double tomorrowPrecipitation = data["daily"]["precipitation_sum"][1].get<double>();
	int tomorrowRainProbability = data["daily"]["precipitation_probability_max"][1].get<int>();
	
	double dayAfterTomorrowPrecipitation = data["daily"]["precipitation_sum"][2].get<double>();
	int dayAfterTomorrowRainProbability = data["daily"]["precipitation_probability_max"][2].get<int>();
	
	WeatherForecast tomorrow;

	tomorrow.expectPrecipitation = tomorrowPrecipitation;
	tomorrow.rainProbability = tomorrowRainProbability;

	double rainWaterTomorrow = tomorrow.expectPrecipitation * 150;
	double expectedRain = rainWaterTomorrow * tomorrow.rainProbability / 100.0;

	
	WeatherForecast dayAfterTomorrow;

	dayAfterTomorrow.expectPrecipitation = dayAfterTomorrowPrecipitation;
	dayAfterTomorrow.rainProbability = dayAfterTomorrowRainProbability;

	double rainWaterDayAfterTomorrow = dayAfterTomorrow.expectPrecipitation * 150;
	double expectedRainDayAfter = rainWaterDayAfterTomorrow * dayAfterTomorrow.rainProbability / 100.0;

	double totalExpectedRain = expectedRain + expectedRainDayAfter;
	double finalWaterAmount = baseWaterAmount - totalExpectedRain;
	if (finalWaterAmount < 0)
		finalWaterAmount = 0;

	//send command to arduino
	std::string command;

	if (finalWaterAmount > 0) {
		command = "ON\n";
	}
	else {
		command = "OFF\n";
	}
	DWORD bytesWritten;

	if (WriteFile(
		serialPort,
		command.c_str(),
		command.size(),
		&bytesWritten,
		nullptr))
	{
		std::cout << "Command sent. Bytes written: "
			<< bytesWritten << "\n";
	}else{
		std::cout << "Failed to send command\n";
	}

	CloseHandle(serialPort);
	
	std::cout <<"Average moisture : "<<average<<"%\n";

	std::cout << "Base water amount: " << baseWaterAmount << " l\n";

	std::cout<<"Tomorrow: \n";
	std::cout<<"Precipitation: "<<tomorrow.expectPrecipitation<<" mm\n";
	std::cout<<"Rain probability: "<<tomorrow.rainProbability<<"%\n";

	std::cout << "Day After Tomorrow: \n";
	std::cout << "Precipitation: " << dayAfterTomorrow.expectPrecipitation << " mm\n";
	std::cout << "Rain probability: " << dayAfterTomorrow.rainProbability<< "%\n";

	std::cout << "The final water amount is: " << finalWaterAmount << " l\n";
}