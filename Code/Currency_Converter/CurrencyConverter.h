#include <iostream>
#include <curl/curl.h>
#include <vector>
#include <nlohmann/json.hpp>
#include <algorithm>
#include <limits> // For std::numeric_limits

namespace CurrencyConverter 
{
	class AvailableCurrencies
	{
	public:

		std::vector<std::string> currenciesAvailable; // Store available currencies

		void fetchAvailableCurrencies();

	};

	class RT_ExchangeCurrency
	{
	public:
		static std::string getExchangeRate(const std::string& base, const std::string& target);
	};

	class Console
	{
	public:

		std::string baseCurrency;
		std::string targetCurrency;

	};

	class Calculator 
	{
	public:

		double rate = 0.00000000f;
		double resultConversion = 0.00000000f;

		void convert(const std::string& base, const std::string& target, double& money);

	};


}//CurrencyConverter
