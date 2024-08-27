#include "DownloadThread.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "nlohmann/json.hpp"

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Article, name, id, author, title, publishedAt, url, content)

//Displays trending news articles on launch
void DownloadThread::operator()(CommonObjects& common)
{
	httplib::Client cli("https://newsapi.org");
	std::string apiKey = "&apiKey=5ed8335161d842a08e17b264a91c5f39"/*a7a2c41f30664d47ad2a97e434f5c893*/;
	std::string keyword = "/v2/top-headlines?country=us";

	auto res = cli.Get(keyword + apiKey);
	if (res->status == 200)
	{

		try {
			auto j = nlohmann::json::parse(res->body);

			common.articles.clear();
			//populating the data structure
			for (auto& articleJson : j["articles"]) {
				Article a;

				if (articleJson["title"] != nullptr)
					a.title = articleJson["title"];

				if (articleJson["source"]["name"] != nullptr)
					a.name = articleJson["source"]["name"];

				if (articleJson["publishedAt"] != nullptr) 
					a.publishedAt = articleJson["publishedAt"];
				
				if (articleJson["content"] != nullptr) {
					a.content = articleJson["content"];
				}
				else {
					a.content = "[No Content]";
				}

				if (articleJson["author"] != nullptr) {
					a.author = articleJson["author"];
				}
				else {
					a.author = "[No Author]";
				}

				if (articleJson["description"] != nullptr)
					a.description = articleJson["description"];

				if (articleJson["url"] != nullptr) {
					a.url = articleJson["url"];
				}
				
				a.display_content = false;
				//std::cout << a.title << std::endl;
				//std::cout << a.name << std::endl;
				common.articles.push_back(a);
			}
			common.data_ready = true;
		}
		catch (const nlohmann::json::exception& e) {
			std::cerr << "JSON parsing error: " << e.what() << std::endl;
		}
		
	} 
	else {
		std::cerr << "HTTP request failed: " << res->status << std::endl;
	}
}

void DownloadThread::SetUrl(std::string_view new_url)
{
	_download_url = new_url;
}

