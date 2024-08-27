#include "CommonObject.h"
#include <iostream>
#include <fstream>
#include <direct.h>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "nlohmann/json.hpp"
#include "Favorite.h"

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Article, name, id, author, title, publishedAt, url, content)

bool titleFinder(std::string title);

//Saves article to the favorites list and the favorites file
void CommonObjects::copyArticle(CommonObjects& common, std::string title) {
	int result;

	std::fstream file("FavoriteNews.txt", std::ios::in | std::ios::out | std::ios::app);

	if (!file.is_open()) {
		std::cerr << "Error opening or creating file" << std::endl;
		exit(1);
	}

	if (titleFinder(title)) { //checks if article is already saved
		file.close();
		return;
	}

	for (auto a : common.articles) { //finds the article and adds it to file and favorite vector
		if (a.title == title) {
			a.display_content = false;
			common.favorite_articles.push_back(a);
		}
	}

	file << title << std::endl;
	file.close();
}

void CommonObjects::removeArticle(CommonObjects& common, std::string title) {
	
	std::ifstream file("FavoriteNews.txt", std::ios::in);
	if (!file.is_open()) {
		std::cerr << "Error opening or creating file" << std::endl;
		exit(1);
	}

	//removes the article from the vector list
	std::string content;
	int iterator = 0;
	while (getline(file, content)) {
		if (content == title) {
			auto it = common.favorite_articles.begin() + iterator;
			common.favorite_articles.erase(it);
		}
		iterator++;
	}

	file.close();

	//removes the article from the favorite folder
	std::ofstream outputFile("FavoriteNews.txt", std::ios::out | std::ios::trunc);
	if (!outputFile.is_open()) {
		std::cerr << "Error opening or creating file" << std::endl;
		exit(1);
	}

	for (auto a : common.favorite_articles) {
		outputFile << a.title << std::endl;
	}
	outputFile.close();
}

//finds if the title exists and returns true or false
bool titleFinder(std::string title) {
	std::ifstream file("FavoriteNews.txt", std::ios::in | std::ios::app);

	if (!file.is_open()) {
		std::cerr << "Error opening or creating file" << std::endl;
		exit(1);
	}

	std::string content;
	while (getline(file, content)) {
		if (content == title) {
			return true;
		}
	}
	return false;

	file.close();
}

//Loads the user's favorites from the previous sessions
void FavoriteNews::operator()(CommonObjects& common) {
	std::string title;
	std::string subString;

	httplib::Client cli("https://newsapi.org");
	std::string apiKey = "&apiKey=5ed8335161d842a08e17b264a91c5f39"/*a7a2c41f30664d47ad2a97e434f5c893*/;
	std::string keyword = "/v2/everything?q=";

	std::ifstream file("FavoriteNews.txt", std::ios::in | std::ios::app);
	if (!file.is_open()) {
		std::cerr << "Error openning file" << std::endl;
		exit(1);
	}

	while (getline(file, title)) {
		//checks if the title is already in the vector to not download it twice
		bool exists = false;
		for (auto a : common.favorite_articles) {
			if (title == a.title) {
				exists = true;
			}
		}
		if (exists) {
			continue;
		}


		auto res = cli.Get(keyword + title + apiKey);


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

					if (articleJson["content"] != nullptr)
						a.content = articleJson["content"];

					if (articleJson["author"] != nullptr) {
						a.author = articleJson["author"];
					}
					else {
						a.author = "No author";
					}

					if (articleJson["description"] != nullptr)
						a.description = articleJson["description"];

					if (articleJson["url"] != nullptr) {
						a.url = articleJson["url"];
					}

					a.display_content = false;
					common.favorite_articles.push_back(a);
					break;
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
}