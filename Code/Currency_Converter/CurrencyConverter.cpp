#include <CurrencyConverter.h>
#define NOMINMAX  // Prevent Windows.h from defining min/max macros
#include <Windows.h>
#undef max  // Undefine Windows' max macro


namespace CurrencyConverter
{

	// cURL Callback Function to Store Response Data
	size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) 
	{
		size_t totalSize = size * nmemb;
		output->append((char*)contents, totalSize);
		return totalSize;
	}

	// Fetch Available Currencies from API
	void AvailableCurrencies::fetchAvailableCurrencies()
	{
		CURL* curl;
		CURLcode res;
		std::string readBuffer;
		std::string apiUrl = "https://api.exchangerate-api.com/v4/latest/EUR"; // EUR used to get all rates

		curl = curl_easy_init();
		if (curl) {
			curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

			res = curl_easy_perform(curl);
			if (res != CURLE_OK)
			{
				std::cerr << "cURL Error: " << curl_easy_strerror(res) << std::endl;
				curl_easy_cleanup(curl);
				return;
			}

			curl_easy_cleanup(curl);
		}

		// Parse JSON Response
		try
		{
			nlohmann::json jsonData = nlohmann::json::parse(readBuffer);
			for (auto& currency : jsonData["rates"].items())
			{
				currenciesAvailable.push_back(currency.key());
			}
		}
		catch (nlohmann::json::parse_error& e)
		{
			std::cerr << "JSON Parse Error: " << e.what() << std::endl;
		}
	}

	// Function to Get Exchange Rate from API
	std::string RT_ExchangeCurrency::getExchangeRate(const std::string& base, const std::string& target) 
	{
		CURL* curl;
		CURLcode res;
		std::string readBuffer;
		std::string apiUrl = "https://api.exchangerate-api.com/v4/latest/" + base;

		curl = curl_easy_init();
		if (curl) 
		{
			curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

			res = curl_easy_perform(curl);
			if (res != CURLE_OK) 
			{
				std::cerr << "cURL Error: " << curl_easy_strerror(res) << std::endl;
				curl_easy_cleanup(curl);
				return "Error";
			}

			curl_easy_cleanup(curl);
		}

		// Parse JSON Response
		try 
		{
			nlohmann::json jsonData = nlohmann::json::parse(readBuffer);
			if (jsonData["rates"].contains(target)) {
				return std::to_string(jsonData["rates"][target].get<double>());
			}
			else 
			{
				return "Not found";
			}
		}
		catch (nlohmann::json::parse_error& e) 
		{
			std::cerr << "JSON Parse Error: " << e.what() << std::endl;
			return "Error";
		}
	}

	// Convert Currency
	void Calculator::convert(const std::string& base, const std::string& target, double& money)
	{
		RT_ExchangeCurrency exchangeRateFetcher;
		std::string rateStr = exchangeRateFetcher.getExchangeRate(base, target);


		rate = std::stod(rateStr);

		resultConversion = money * rate;

	}

}//CurrencyConverter