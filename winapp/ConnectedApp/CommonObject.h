#pragma once
#include <atomic>
#include <string>
#include <vector>

struct Article
{
	std::string id;
	std::string name;
	std::string author;
	std::string title;
	std::string description;
	std::string publishedAt;
	std::string url;
	std::string content;
	bool display_content;
};

struct CommonObjects
{
	std::atomic_bool start_download = false;
	std::atomic_bool data_ready = false;
	std::atomic_bool user_search = false;
	bool display_favorites = false;
	std::string userSearch;
	std::vector<Article> articles;
	std::vector<Article> favorite_articles;

	void copyArticle(CommonObjects& common, std::string title);
	void removeArticle(CommonObjects& common, std::string title);
};
